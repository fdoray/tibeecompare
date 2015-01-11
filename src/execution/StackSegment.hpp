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
#ifndef _TIBEE_EXECUTION_STACKSEGMENT_HPP
#define _TIBEE_EXECUTION_STACKSEGMENT_HPP

#include "base/BasicTypes.hpp"

namespace tibee
{
namespace execution
{

class StackSegment
{
public:
    typedef std::vector<StackItem> DepthStackItems;

    StackSegment()
        : _thread(-1) {}

    thread_t thread() const { return _thread; }
    void set_thread(thread_t thread) { _thread = thread; } 

    void AddStackItem(const StackItem& item)
    {
        if (_stackItems.size() <= item.depth())
            _stackItems.resize(item.depth() + 1);
        _stackItems[item.depth()].push_back(item);
    }

    size_t MaxDepth() const { return _stackItems.size(); }

    DepthStackItems::const_iterator DepthBegin(size_t depth) const {
        return _stackItems[depth].begin();
    }
    DepthStackItems::const_iterator DepthEnd(size_t depth) const {
        return _stackItems[depth].end();
    }

private:
    // Thread.
    thread_t _thread;

    // Stack items, per depth, sorted by timestamp.
    typedef std::vector<DepthStackItems> StackItems;
    StackItems _stackItems;
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_STACKSEGMENT_HPP
