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
#include "execution/StacksFromDisk.hpp"

#include "base/print.hpp"

namespace tibee
{
namespace execution
{

StacksFromDisk::StacksFromDisk()
{
}

StacksFromDisk::~StacksFromDisk()
{
}

void StacksFromDisk::AddThread(const Thread& thread)
{
    _threads[thread.thread()] = thread;
}

void StacksFromDisk::AddStackItem(thread_t thread, const StackItem& item)
{
    auto look = _histories.find(thread);
    if (look == _histories.end())
    {
        _histories[thread].reset(new containers::RedBlackIntervalTree<StackItem>);
        look = _histories.find(thread);
    }

    look->second->Insert(
        containers::Interval(item.start(), item.end()),
        item);
}

void StacksFromDisk::AddLink(const Link& link)
{
    if (!_links.empty())
    {
        auto lastLinkSourceTs = _links.front().sourceTs();
        if (lastLinkSourceTs > link.sourceTs())
        {
            base::tberror() << "Links must be inserted in order in StacksFromDisk." << base::tbendl();
            return;
        }
    }

    _links.push_back(link);
}

}  // namespace execution
}  // namespace tibee
