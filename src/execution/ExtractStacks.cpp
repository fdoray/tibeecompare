/* Copyright (c) 2015 Francois Doray <francois.pierre-doray@polymtl.ca>
 *
 * This file is part of tibeecompare.
 *
 * tibeecompare is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tibeecompare is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tibeecompare.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "execution/ExtractStacks.hpp"

#include <assert.h>
#include <deque>
#include <iostream>
#include <vector>

#include "base/BasicTypes.hpp"
#include "base/BindObject.hpp"
#include "base/Constants.hpp"
#include "critical/ComputeCriticalPath.hpp"
#include "critical/GetStatusString.hpp"
#include "stacks/Identifiers.hpp"

namespace tibee
{
namespace execution
{

namespace
{

class StacksExtractor
{
public:
    StacksExtractor(
        const stacks::StacksBuilder& stacks,
        const critical::CriticalGraph& graph,
        const state::StateHistory& stateHistory,
        state::CurrentState* currentState,
        db::Database* db,
        Execution* execution)
        : stacks(stacks), graph(graph), stateHistory(stateHistory),
          currentState(currentState), db(db), execution(execution)
    {
        state::AttributePathStr threadsPath { kStateLinux, kStateThreads };
        threadsPathKey = currentState->GetAttributeKeyStr(threadsPath);
        currentCpuQuark = currentState->Quark(kStateCurCpu);

        state::AttributePathStr cpusPath { kStateLinux, kStateCpus };
        cpusPathKey = currentState->GetAttributeKeyStr(cpusPath);
        currentThreadQuark = currentState->Quark(kStateCurThread);
    }

    void ExtractStacks(const critical::CriticalPath& criticalPath,
                       stacks::StackId baseStackId)
    {
        namespace pl = std::placeholders;

        // Stack of threads.
        std::vector<ThreadInfo> threads;

        // Traverse all segments of the critical path.
        for (const auto& segment : criticalPath)
        {
            uint64_t segmentDuration = segment.endTs() - segment.startTs();

            // Make sure that the current thread is on top of the stack of threads.
            EnsureCurrentThreadIsOnThreadsStack(segment, baseStackId, &threads);

            if (segment.type() == critical::kEpsilon)
                continue;

            // Increment execution samples.
            if (segment.type() == critical::kRun)
            {
                // Find samples that belong to this segment.
                uint64_t total = 0;
                auto callback = std::bind(
                    &StacksExtractor::SampleCallback, this, pl::_1, pl::_2,
                    std::ref(threads), segment.tid(), &total);
                stacks.EnumerateStacks(
                    segment.tid(), segment.startTs(), segment.endTs(), callback);

                // If there was not enough samples, add a "No stack" sample.
                if (total < segmentDuration)
                {
                    execution->IncrementSample(
                        PushOnStack(threads.back().cleanStack, "No stack"),
                        segmentDuration - total);
                }
            }
            else if (segment.type() == critical::kWaitBlocked ||
                     segment.type() == critical::kTimer ||
                     segment.type() == critical::kNetwork ||
                     segment.type() == critical::kUserInput ||
                     segment.type() == critical::kBlockDevice)
            {
                // Update the stack for this thread.
                if (threads.back().stack == threads.back().cleanStack)
                {
                    threads.back().stack = ConcatenateStacks(
                        threads.back().cleanStack,
                        stacks.GetStack(segment.tid(), segment.startTs()));
                }

                auto stackId = PushOnStack(
                    threads.back().stack,
                    critical::GetStatusString(segment.type()));
                execution->IncrementSample(stackId, segmentDuration);
            }
            else if (segment.type() == critical::kWaitCpu)
            {
                // Create a stack with "wait-cpu" on top of it.
                auto stackId = PushOnStack(
                    threads.back().cleanStack,
                    critical::GetStatusString(segment.type()));

                // Find the last CPU on which this thread was running.
                auto lastCpuKey = currentState->GetAttributeKey(
                    threadsPathKey,
                    {currentState->IntQuark(segment.tid()), currentCpuQuark});

                uint32_t lastCpu = -1;

                if (stateHistory.GetUIntegerValue(lastCpuKey, segment.startTs(), &lastCpu))
                {
                    // Find out what was running while we were waiting for the CPU.
                    auto curThreadKey = currentState->GetAttributeKey(
                        cpusPathKey,
                        {currentState->IntQuark(lastCpu), currentThreadQuark});

                    uint64_t total = 0;
                    stateHistory.EnumerateUIntegerValues(
                        curThreadKey, segment.startTs(), segment.endTs(),
                        std::bind(&StacksExtractor::PreemptedCallback,
                                  this, pl::_1, pl::_2, pl::_3, stackId, &total));

                    // Add a simple "wait-cpu" segment.
                    if (segmentDuration > total)
                        execution->IncrementSample(stackId, segmentDuration - total);
                }
                else
                {
                    // Add a simple "wait-cpu" segment.
                    execution->IncrementSample(stackId, segmentDuration);
                }
            }
            else
            {
                auto stackId = PushOnStack(
                    threads.back().cleanStack,
                    critical::GetStatusString(segment.type()));
                execution->IncrementSample(stackId, segmentDuration);
            }

            // Update the stack for this thread.
            threads.back().stack = ConcatenateStacks(
                threads.back().cleanStack,
                stacks.GetStack(segment.tid(), segment.endTs()));
        }
    }

private:
    struct ThreadInfo
    {
        ThreadInfo() : tid(kInvalidThread), stack(stacks::kEmptyStackId),
                       cleanStack(stacks::kEmptyStackId), isSyscall(false) {}
        ThreadInfo(thread_t tid, stacks::StackId stack)
            : tid(tid), stack(stack), cleanStack(stack), isSyscall(false) {}
        thread_t tid;
        stacks::StackId stack;
        stacks::StackId cleanStack;
        bool isSyscall;
    };

    stacks::StackId PushOnStack(stacks::StackId bottom,
                                const std::string& function)
    {
        stacks::Stack stack;
        stack.set_bottom(bottom);
        stack.set_function(db->AddFunctionName(function));
        return db->AddStack(stack);
    }

    stacks::StackId ConcatenateStacks(stacks::StackId bottom,
                                      stacks::StackId top)
    {
        if (top == stacks::kEmptyStackId)
            return bottom;

        // Get the functions from the top stack.
        std::deque<stacks::FunctionNameId> topStackFunctions;
        while (top != stacks::kEmptyStackId)
        {
            auto step = db->GetStack(top);
            topStackFunctions.push_front(step.function());
            top = step.bottom();
        }

        // Push the functions of the top stack on the bottom stack.
        stacks::StackId fullStack = bottom;
        for (auto function : topStackFunctions)
        {
            stacks::Stack step;
            step.set_bottom(fullStack);
            step.set_function(function);
            fullStack = db->AddStack(step);
        }

        return fullStack;
    }

    void SampleCallback(stacks::StackId stackId,
                        uint64_t value,
                        const std::vector<ThreadInfo>& threads,
                        thread_t thread,
                        uint64_t* total)
    {
        assert(threads.back().tid == thread);

        // Concatenate the stack with the stack of the previous threads.
        stackId = ConcatenateStacks(threads.back().cleanStack, stackId);

        // Increment the sample count for this stack.
        execution->IncrementSample(stackId, value);

        *total += value;
    }

    void PreemptedCallback(uint32_t tid,
                           timestamp_t start,
                           timestamp_t end,
                           stacks::StackId baseStackId,
                           uint64_t* total)
    {
        if (tid == static_cast<uint32_t>(-1))
            return;

        // We were preempted by a thread |tid|.
        // Compute the critical path for this thread.
        critical::CriticalPath criticalPath;
        critical::ComputeCriticalPath(graph, start, end, tid, &criticalPath);

        // Recursive call to find the stacks of the thread that preempted us.
        ExtractStacks(criticalPath, baseStackId);

        // Keep track of the duration that has been resolved.
        *total += (end - start);
    }

    void EnsureCurrentThreadIsOnThreadsStack(
        const critical::CriticalPathSegment& segment,
        stacks::StackId baseStackId,
        std::vector<ThreadInfo>* threads)
    {
        if (threads->empty())
        {
            auto cleanStack = PushOnStack(
                baseStackId, std::string("[thread ") + std::to_string(segment.tid()) + "]");
            threads->push_back(ThreadInfo(segment.tid(), cleanStack));
        }
        else
        {
            size_t depth = 0;
            for (; depth < threads->size(); ++depth)
            {
                if ((*threads)[depth].tid == segment.tid())
                    break;
            }

            if (depth == threads->size())
            {
                // This thread is not on the stack of threads yet: add it.
                auto cleanStack = PushOnStack(
                    threads->back().stack, std::string("[thread ") + std::to_string(segment.tid()) + "]");
                threads->push_back(ThreadInfo(segment.tid(), cleanStack));
            }
            else
            {
                // This thread is already on the stack of threads: remove
                // everything that is above it.
                threads->resize(depth + 1);
            }
        }

        // Set the full stack.
        threads->back().stack = ConcatenateStacks(
            threads->back().cleanStack,
            stacks.GetStack(segment.tid(), segment.endTs()));
    }

    // Stacks.
    const stacks::StacksBuilder& stacks;

    // Critical graph, to compute the critical path of tasks that preempt us.
    const critical::CriticalGraph& graph;

    // State history, to resolve preempted states.
    const state::StateHistory& stateHistory;

    // Current state, to generate keys to query the state history.
    state::CurrentState* currentState;

    // Database, to find the content of stacks.
    db::Database* db;

    // Execution, to which samples are added.
    Execution* execution;

    // Keys and quarks to access the state history.
    state::AttributeKey threadsPathKey;
    quark::Quark currentCpuQuark;
    state::AttributeKey cpusPathKey;
    quark::Quark currentThreadQuark;
};

}  // namespace

void ExtractStacks(
    const critical::CriticalPath& criticalPath,
    const stacks::StacksBuilder& stacks,
    const critical::CriticalGraph& graph,
    const state::StateHistory& stateHistory,
    state::CurrentState* currentState,
    db::Database* db,
    Execution* execution)
{
    StacksExtractor extractor(
        stacks, graph, stateHistory, currentState, db, execution);
    extractor.ExtractStacks(criticalPath, stacks::kEmptyStackId);
}

}  // namespace execution
}  // namespace tibee
