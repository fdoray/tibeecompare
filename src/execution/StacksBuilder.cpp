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
#include "execution/StacksBuilder.hpp"

#include <algorithm>

namespace tibee
{
namespace execution
{

StacksBuilder::StacksBuilder()
{
}

StacksBuilder::~StacksBuilder()
{
}

void StacksBuilder::SetStack(
    thread_t thread,
    StackId stackId)
{
    auto& stacks = _stacks[thread];

    if (!stacks.empty())
        stacks.back().endTs = _ts;

    if (stacks.empty() || stacks.back().stackId != stackId)
    {
        StackWrapper wrapper;
        wrapper.stackId = stackId;
        wrapper.startTs = _ts;
        wrapper.endTs = _ts;
        stacks.push_back(wrapper);
    }
}

void StacksBuilder::SetStack(
    thread_t thread,
    const std::vector<std::string>& stack)
{
    // Get the stack identifier.
    // The first element of the |stack| is the top of the stack.
    StackId previousStackId = kEmptyStackId;
    for (size_t i = 0; i < stack.size(); ++i)
    {
        Stack stackStep;
        stackStep.set_bottom(previousStackId);
        stackStep.set_function(_db->AddFunctionName(stack[stack.size() - i - 1]));
        previousStackId = _db->AddStack(stackStep);
    }

    // Set the stack for the thread.
    SetStack(thread, previousStackId);
}

void StacksBuilder::EnumerateStacks(
    thread_t thread, timestamp_t start, timestamp_t end,
    const EnumerateStacksCallback& callback)
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

bool StacksBuilder::StacksComparator::operator() (
    const StackWrapper& stack, timestamp_t ts) const
{
    return stack.startTs < ts;
}

void StacksBuilder::Terminate()
{
    for (auto& stacks : _stacks)
    {
        if (!stacks.second.empty())
            stacks.second.back().endTs = _ts;
    }
}

}  // namespace execution
}  // namespace tibee
