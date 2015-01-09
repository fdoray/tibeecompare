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

namespace
{

void EnumerateStacksCallbackWrapper(
    const containers::RedBlackIntervalTree<StackItem>::ElementPair& pair,
    const StacksBuilder::EnumerateStacksCallback& callback)
{
    callback(pair.second);
}

class LinksComparator
{
public:
    bool operator() (const Link& link, timestamp_t ts) const
    {
        return link.sourceTs() < ts;
    }
};

}

StacksBuilder::StacksBuilder()
    : _ts(0)
{
}

StacksBuilder::~StacksBuilder()
{
}

void StacksBuilder::PushStack(thread_t thread, const std::string& name)
{
    auto& stack = _stacks[thread];

    StackItem stackItem;
    stackItem.set_name(name);
    stackItem.set_depth(stack.size());
    stackItem.set_start(_ts);
    stack.push(stackItem);
}

void StacksBuilder::PopStack(thread_t thread)
{
    auto it = _stacks.find(thread);
    if (it == _stacks.end() || it->second.empty())
        return;

    auto& stack = it->second;
    stack.top().set_end(_ts);

    auto& history = _histories[thread];
    if (history.get() == nullptr)
        history.reset(new containers::RedBlackIntervalTree<StackItem>);
    history->Insert(
        containers::Interval(stack.top().start(), stack.top().end()),
        stack.top());

    stack.pop();
}

void StacksBuilder::AddLink(thread_t sourceThread, thread_t targetThread)
{
    _links.push_back(Link(sourceThread, _ts, targetThread, _ts));
}

void StacksBuilder::EnumerateStacks(thread_t thread,
                                    const containers::Interval& interval,
                                    const EnumerateStacksCallback& callback) const
{
    auto threadHistoryIt = _histories.find(thread);
    if (threadHistoryIt == _histories.end())
        return;

    const auto& threadHistory = *threadHistoryIt->second;
    threadHistory.EnumerateIntersection(
        interval,
        std::bind(&EnumerateStacksCallbackWrapper,
                  std::placeholders::_1,
                  callback));
}

void StacksBuilder::EnumerateLinks(const containers::Interval& interval,
                                   const EnumerateLinksCallback& callback) const
{
    LinksComparator comparator;
    auto it = std::lower_bound(_links.begin(), _links.end(), interval.low(), comparator);

    for (; it != _links.end(); ++it)
    {
        if (it->sourceTs() > interval.high())
            break;
        if (it->targetTs() <= interval.high())
            callback(*it);
    }
}

}  // namespace execution
}  // namespace tibee
