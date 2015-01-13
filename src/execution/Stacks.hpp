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
#ifndef _TIBEE_EXECUTION_STACKS_HPP
#define _TIBEE_EXECUTION_STACKS_HPP

#include <functional>

#include "base/BasicTypes.hpp"
#include "containers/Interval.hpp"
#include "execution/Link.hpp"
#include "execution/StackItem.hpp"
#include "execution/Thread.hpp"

namespace tibee
{
namespace execution
{

class Stacks
{
public:
    typedef std::function<void (const StackItem&)> EnumerateStacksCallback;
    typedef std::function<void (const Link&)> EnumerateLinksCallback;
    typedef std::function<void (const Thread& thread)> EnumerateThreadsCallback;

    // Returns the number of stack items for a thread.
    virtual size_t StackItemsCount(thread_t thread) const = 0;

    // Enumerate stack items that intersect the specified interval.
    virtual void EnumerateStacks(thread_t thread,
                                 const containers::Interval& interval,
                                 const EnumerateStacksCallback& callback) const = 0;

    // Returns the number of links.
    virtual size_t LinksCount() const = 0;

    // Enumerate links that start and end whithin the specified interval.
    virtual void EnumerateLinks(const containers::Interval& interval,
                                const EnumerateLinksCallback& callback) const = 0;

    // Enumerate threads with their names.
    virtual void EnumerateThreads(const EnumerateThreadsCallback& callback) const = 0;    

    // Get information about a thread.
    virtual const Thread& GetThread(thread_t thread) const = 0;
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_STACKS_HPP
