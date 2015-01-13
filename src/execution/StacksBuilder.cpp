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
    : _ts(0)
{
}

StacksBuilder::~StacksBuilder()
{
}

void StacksBuilder::PushStack(thread_t thread, const quark::Quark& name)
{
    auto& stack = _stacks[thread];

    StackItem stackItem;
    stackItem.set_name(name);
    stackItem.set_depth(stack.size());
    stackItem.set_start(_ts);
    stack.push(stackItem);

    // Make sure the thread entry exists.
    auto look = _threads.find(thread);
    if (look == _threads.end())
        _threads[thread] = Thread(thread);
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

    if (stack.top().start() != stack.top().end())
    {
        history->Insert(
            containers::Interval(stack.top().start(), stack.top().end()),
            stack.top());
    }

    stack.pop();
}

void StacksBuilder::AddLink(thread_t sourceThread, thread_t targetThread)
{
    _links.push_back(Link(sourceThread, _ts, targetThread, _ts));
}

void StacksBuilder::SetThreadName(thread_t thread, const std::string& name)
{
    auto look = _threads.find(thread);
    if (look != _threads.end())
    {
        look->second.set_name(name);
    }
    else
    {
        Thread threadDesc(thread);
        threadDesc.set_name(name);
        _threads[thread] = threadDesc;
    }
}

bool StacksBuilder::HasStackForThread(thread_t thread) const
{
    return _threads.find(thread) != _threads.end();
}

void StacksBuilder::Terminate()
{
    for (auto& stack : _stacks)
    {
        auto& history = _histories[stack.first];
        if (history.get() == nullptr)
            history.reset(new containers::RedBlackIntervalTree<StackItem>);


        while (!stack.second.empty())
        {
            stack.second.top().set_end(_ts);

            history->Insert(
                containers::Interval(stack.second.top().start(),
                                     stack.second.top().end()),
                stack.second.top());

            stack.second.pop();
        }
    }
}

}  // namespace execution
}  // namespace tibee
