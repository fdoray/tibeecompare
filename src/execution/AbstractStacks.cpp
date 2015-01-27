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
#include "execution/AbstractStacks.hpp"

namespace tibee
{
namespace execution
{

namespace
{

void EnumerateStacksCallbackWrapper(
    const containers::RedBlackIntervalTree<StackItem>::ElementPair& pair,
    const AbstractStacks::EnumerateStacksCallback& callback)
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

size_t AbstractStacks::StackItemsCount(thread_t thread) const
{
    auto look = _histories.find(thread);
    if (look == _histories.end())
        return 0;
    return look->second->size();
}

void AbstractStacks::EnumerateStacks(thread_t thread,
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

size_t AbstractStacks::LinksCount() const
{
    return _links.size();
}

void AbstractStacks::EnumerateLinks(const containers::Interval& interval,
                                   const EnumerateLinksCallback& callback) const
{
    LinksComparator comparator;
    auto it = std::lower_bound(_links.begin(), _links.end(), interval.low(), comparator);

    for (; it != _links.end(); ++it)
    {
        if (it->sourceTs() > interval.high())
            break;
        callback(*it);
    }
}

void AbstractStacks::EnumerateThreads(const EnumerateThreadsCallback& callback) const
{
    for (const auto& thread : _threads)
    {
        callback(thread.second);
    }
}

const Thread& AbstractStacks::GetThread(thread_t thread) const
{
    return _threads.find(thread)->second;
}

}  // namespace execution
}  // namespace tibee
