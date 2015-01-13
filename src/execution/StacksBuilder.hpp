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
#ifndef _TIBEE_EXECUTION_STACKSBUILDER_HPP
#define _TIBEE_EXECUTION_STACKSBUILDER_HPP

#include <memory>
#include <stack>
#include <unordered_map>
#include <vector>

#include "base/BasicTypes.hpp"
#include "execution/AbstractStacks.hpp"
#include "execution/StackItem.hpp"
#include "execution/Thread.hpp"

namespace tibee
{
namespace execution
{

class StacksBuilder : public AbstractStacks
{
public:
    StacksBuilder();
    ~StacksBuilder();

    // Construction of the stacks.
    void SetTimestamp(timestamp_t ts) { _ts = ts; }
    void PushStack(thread_t thread, const quark::Quark& name);
    void PopStack(thread_t thread);
    void AddLink(thread_t sourceThread, thread_t targetThread);
    void SetThreadName(thread_t thread, const std::string& name);

    bool HasStackForThread(thread_t thread) const;

    void Terminate();

private:
    // Current timestamp.
    timestamp_t _ts;

    // Current stack, per thread.
    typedef std::unordered_map<thread_t, std::stack<StackItem>> ThreadsStacks;
    ThreadsStacks _stacks;
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_STACKSBUILDER_HPP
