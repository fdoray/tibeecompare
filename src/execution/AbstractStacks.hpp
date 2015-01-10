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
#ifndef _TIBEE_EXECUTION_ABSTRACTSTACKS_HPP
#define _TIBEE_EXECUTION_ABSTRACTSTACKS_HPP

#include <memory>
#include <stack>
#include <unordered_map>
#include <vector>

#include "base/BasicTypes.hpp"
#include "containers/RedBlackIntervalTree.hpp"
#include "execution/Link.hpp"
#include "execution/StackItem.hpp"
#include "execution/Stacks.hpp"
#include "execution/Thread.hpp"

namespace tibee
{
namespace execution
{

class AbstractStacks : public Stacks
{
public:
    typedef Stacks::EnumerateStacksCallback EnumerateStacksCallback;
    typedef Stacks::EnumerateLinksCallback EnumerateLinksCallback;
    typedef Stacks::EnumerateThreadsCallback EnumerateThreadsCallback;

    // Reading the stacks.
    virtual size_t StackItemsCount(thread_t thread) const override;

    virtual void EnumerateStacks(thread_t thread,
                                 const containers::Interval& interval,
                                 const EnumerateStacksCallback& callback) const override;

    virtual size_t LinksCount() const override;

    virtual void EnumerateLinks(const containers::Interval& interval,
                                const EnumerateLinksCallback& callback) const override;

    virtual void EnumerateThreads(const EnumerateThreadsCallback& callback) const override;

protected:
    // Stack history, per thread.
    typedef std::unordered_map<
        thread_t,
        std::unique_ptr<containers::RedBlackIntervalTree<StackItem>>> ThreadsHistories;
    ThreadsHistories _histories;

    // Links.
    std::vector<Link> _links;

    // Threads.
    std::unordered_map<thread_t, Thread> _threads;
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_ABSTRACTSTACKS_HPP
