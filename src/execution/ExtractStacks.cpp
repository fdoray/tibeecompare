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
#include "critical/GetStatusString.hpp"
#include "stacks/Identifiers.hpp"

namespace tibee
{
namespace execution
{

namespace
{

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
                            const std::string& function,
                            db::Database*db)
{
    stacks::Stack stack;
    stack.set_bottom(bottom);
    stack.set_function(db->AddFunctionName(function));
    return db->AddStack(stack);
}

stacks::StackId ConcatenateStacks(stacks::StackId bottom,
                                  stacks::StackId top,
                                  db::Database* db)
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
                    db::Database* db,
                    Execution* execution,
                    uint64_t* total)
{
    assert(threads.back().tid == thread);

    // Concatenate the stack with the stack of the previous threads.
    stackId = ConcatenateStacks(
        threads.back().cleanStack, stackId, db);

    // Increment the sample count for this stack.
    execution->IncrementSample(stackId, value);

    *total += value;
}

}  // namespace

void ExtractStacks(
    const stacks::StacksBuilder& stacks,
    const critical::CriticalPath& criticalPath,
    db::Database* db,
    Execution* execution)
{
    namespace pl = std::placeholders;

    std::vector<ThreadInfo> threads;

    // Traverse all segments of the critical path.
    for (const auto& segment : criticalPath)
    {
        uint64_t segmentDuration = segment.endTs() - segment.startTs();

        // Make sure that the current thread is on top of the stack of threads.
        if (threads.empty())
        {
            threads.push_back(ThreadInfo(
                segment.tid(),
                PushOnStack(stacks::kEmptyStackId, "[thread]", db)));
        }
        else
        {
            size_t depth = 0;
            for (; depth < threads.size(); ++depth)
            {
                if (threads[depth].tid == segment.tid())
                    break;
            }

            if (depth == threads.size())
            {
                // This thread is not on the stack of threads yet: add it.
                threads.push_back(ThreadInfo(
                    segment.tid(),
                    PushOnStack(threads.back().stack, "[thread]", db)));
            }
            else
            {
                // This thread is on the stack of threads: remove everything
                // that is above it.
                threads.resize(depth + 1);
            }
        }

        // Increment execution samples.
        if (segment.type() == critical::kRun)
        {
            // Find samples that belong to this segment.
            uint64_t total = 0;
            auto callback = std::bind(
                &SampleCallback, pl::_1, pl::_2, std::ref(threads),
                segment.tid(), db, execution, &total);
            stacks.EnumerateStacks(
                segment.tid(), segment.startTs(), segment.endTs(), callback);

            // If there was not enough samples, add a "No stack" sample.
            if (total < segmentDuration)
            {
                execution->IncrementSample(
                    PushOnStack(threads.back().cleanStack, "No stack", db),
                    segmentDuration - total);
            }
        }
        else if (segment.type() == critical::kWaitBlocked ||
                 segment.type() == critical::kTimer ||
                 segment.type() == critical::kNetwork ||
                 segment.type() == critical::kUserInput ||
                 segment.type() == critical::kBlockDevice)
        {
            auto stackId = PushOnStack(
                threads.back().stack,
                critical::GetStatusString(segment.type()),
                db);
            execution->IncrementSample(stackId, segmentDuration);
        }
        else
        {
            auto stackId = PushOnStack(
                threads.back().cleanStack,
                critical::GetStatusString(segment.type()),
                db);
            execution->IncrementSample(stackId, segmentDuration);
        }

        // Update the stack for this thread.
        threads.back().stack = ConcatenateStacks(
            threads.back().cleanStack,
            stacks.GetStack(segment.tid(), segment.endTs()),
            db);
    }
}

}  // namespace execution
}  // namespace tibee
