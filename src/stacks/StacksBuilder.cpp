/* Copyright (c) 2014 Francois Doray <francois.pierre-doray@polymtl.ca>
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
#include "stacks/StacksBuilder.hpp"

#include <algorithm>

#include "base/CleanContainer.hpp"
#include "base/print.hpp"

namespace tibee
{
namespace stacks
{

namespace
{

using base::tberror;
using base::tbendl;

const uint64_t kMinSyscallDuration = 100000;  // 100 us

const size_t kMaxRewind = 20;

}  // namespace

StacksBuilder::StacksBuilder()
    : _ts(0),
      _db(nullptr)
{
}

StacksBuilder::~StacksBuilder()
{
}

void StacksBuilder::Cleanup(timestamp_t ts)
{
	for (auto& threadHistory : _stacks)
	{
		if (threadHistory.second.empty())
			continue;

		StacksComparator comparator;
		auto it = std::lower_bound(
		    threadHistory.second.begin(), threadHistory.second.end(), ts, comparator);
		if (it == threadHistory.second.begin())
			continue;
		--it;
		base::CleanVector(it, threadHistory.second.end(), &threadHistory.second);
	}
}

void StacksBuilder::SetStack(thread_t thread, StackId stackId, bool isSyscall)
{
    auto& stacks = _stacks[thread];

    // Update end timestamp for the previous stack.
    if (!stacks.empty())
        stacks.back().endTs = _ts;

    // Add the new stack, if it is different from the previous one.
    if (stacks.empty() || stacks.back().stackId != stackId)
    {
        StackWrapper wrapper;
        wrapper.stackId = stackId;
        wrapper.startTs = _ts;
        wrapper.endTs = _ts;
        wrapper.isSyscall = isSyscall;
        wrapper.isResolved = false;
        stacks.push_back(wrapper);
    }
}

void StacksBuilder::SetStack(thread_t thread, StackId stackId)
{
    SetStack(thread, stackId, false);
}

void StacksBuilder::SetStack(thread_t thread,
                             const std::vector<std::string>& stack)
{
    SetStack(thread, GetStackIdentifier(stack));
}

void StacksBuilder::SetNoStack(thread_t thread)
{
    std::vector<std::string> stack({std::string("No Stack")});
    SetStack(thread, GetStackIdentifier(stack), true);
}

void StacksBuilder::StartSystemCall(thread_t thread, const std::string& syscall)
{
    std::vector<std::string> stack({std::string("sys:") + syscall});
    SetStack(thread, GetStackIdentifier(stack), true);
}

void StacksBuilder::EndSytemCall(thread_t thread)
{
    // Get the stack from before the system call.
    auto& stacks = _stacks[thread];
    if (stacks.empty())
    {
        return;
    }

    if (!stacks.back().isSyscall)
    {
        tberror() << "Ending a system call on thread " << thread << ", but is "
                  << "inside a usermode stack." << tbendl();
        return;
    }

    if (stacks.size() == 1)
    {
        // If there is no stack before the system call, push "No stack"
        // function.
        SetNoStack(thread);
        return;
    }

    StackId previousStack = (stacks.end() - 2)->stackId;
    SetStack(thread, previousStack);
}

void StacksBuilder::SetLastSystemCallStack(thread_t thread, StackId stackId)
{
    auto& stacks = _stacks[thread];
    if (stacks.empty())
        return;

    // Find a system call of at least 100 us.
    auto it = stacks.end();
    --it;

    for (size_t i = 0; i < kMaxRewind; ++i)
    {
        if (it->isSyscall && (it->endTs - it->startTs) >= kMinSyscallDuration)
        {
            if (it->isResolved)
                return;

            Stack syscallOnlyStack = _db->GetStack(it->stackId);

            Stack stackStep;
            stackStep.set_bottom(stackId);
            stackStep.set_function(syscallOnlyStack.function());
            StackId syscallFullStack = _db->AddStack(stackStep);

            it->stackId = syscallFullStack;
            it->isResolved = true;

            return;
        }

        if (it == stacks.begin())
            return;

        --it;
    }
}

void StacksBuilder::SetLastSystemCallStack(
    thread_t thread, const std::vector<std::string>& stack)
{
    SetLastSystemCallStack(thread, GetStackIdentifier(stack));
}

void StacksBuilder::EnumerateStacks(
    thread_t thread, timestamp_t start, timestamp_t end,
    const EnumerateStacksCallback& callback) const
{
    auto look = _stacks.find(thread);
    if (look == _stacks.end())
        return;
    const auto& stacks = look->second;

    StacksComparator comparator;
    auto it = std::lower_bound(
        stacks.begin(), stacks.end(), start, comparator);
    if (it != stacks.begin())
        --it;

    for (; it != stacks.end(); ++it)
    {
        if (it->startTs > end)
            return;
        timestamp_t stackStart = std::max(start, it->startTs);
        timestamp_t stackEnd = std::min(end, it->endTs);

        callback(it->stackId, stackEnd - stackStart);
    }
}

stacks::StackId StacksBuilder::GetStack(thread_t thread, timestamp_t ts) const
{
    auto look = _stacks.find(thread);
    if (look == _stacks.end())
      return kEmptyStackId;
    const auto& stacks = look->second;

    StacksComparator comparator;
    auto it = std::upper_bound(
        stacks.begin(), stacks.end(), ts, comparator);
    if (it != stacks.begin())
        --it;
    if (it == stacks.end())
        return kEmptyStackId;

    if (it->startTs <= ts && it->endTs > ts)
        return it->stackId;
    return kEmptyStackId;
}

bool StacksBuilder::StacksComparator::operator() (
    const StackWrapper& stack, timestamp_t ts) const
{
    return stack.startTs < ts;
}

bool StacksBuilder::StacksComparator::operator() (
    timestamp_t ts, const StackWrapper& stack) const
{
    return ts < stack.startTs;
}

void StacksBuilder::Terminate()
{
    for (auto& stacks : _stacks)
    {
        if (!stacks.second.empty())
            stacks.second.back().endTs = _ts;
    }
}

StackId StacksBuilder::GetStackIdentifier(const std::vector<std::string>& stack)
{
    if (stack.empty())
        return GetStackIdentifier({std::string("empty")});

    StackId previousStackId = kEmptyStackId;
    for (size_t i = 0; i < stack.size(); ++i)
    {
        Stack stackStep;
        stackStep.set_bottom(previousStackId);
        stackStep.set_function(_db->AddFunctionName(stack[stack.size() - i - 1]));
        previousStackId = _db->AddStack(stackStep);
    }
    return previousStackId;
}

}  // namespace stacks
}  // namespace tibee
