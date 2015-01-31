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
#include "execution/ExecutionsBuilder.hpp"

namespace tibee
{
namespace execution
{

ExecutionsBuilder::ExecutionsBuilder()
    : _ts(0)
{
}

ExecutionsBuilder::~ExecutionsBuilder()
{
}

bool ExecutionsBuilder::StartExecution(
    thread_t thread,
    const std::string& name,
    bool needsToEnd)
{
    // Create execution.
    Execution::UP& execution = _activeExecutions[thread];
    execution.reset(new Execution);

    execution->set_name(name);
    execution->set_startTs(_ts);
    execution->set_startThread(thread);
    execution->set_endTs(_ts);
    execution->set_endThread(thread);

    // Remember whether the execution needs to end.
    _needsToEnd[thread] = needsToEnd;

    return true;
}

void ExecutionsBuilder::EndExecution(thread_t thread)
{
    auto look = _activeExecutions.find(thread);
    if (look == _activeExecutions.end())
        return;

    look->second->set_endTs(_ts);
    look->second->set_endThread(thread);
    _completedExecutions.push_back(std::move(look->second));

    _activeExecutions.erase(look);
}

void ExecutionsBuilder::Terminate()
{
    for (const auto& needsToEnd : _needsToEnd)
    {
        if (!needsToEnd.second)
        {
            auto thread = needsToEnd.first;
            EndExecution(thread);
        }
    }
}

}  // namespace execution
}  // namespace tibee
