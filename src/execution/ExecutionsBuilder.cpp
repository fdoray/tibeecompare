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

void ExecutionsBuilder::StartThreadExecution(thread_t thread, const std::string& name)
{
    Execution::UP execution(new Execution);
    execution->set_name(name);
    execution->set_trace(_trace);
    execution->set_startTs(_ts);
    execution->set_startThread(thread);

    _currentExecutions[thread] = std::move(execution);
}

void ExecutionsBuilder::EndThreadExecution(thread_t thread)
{
    auto look = _currentExecutions.find(thread);
    if (look == _currentExecutions.end())
        return;

    Execution::UP execution = std::move(look->second);
    execution->set_endTs(_ts);
    execution->set_endThread(thread);

    _completedExecutions.push_back(std::move(execution));
}

}  // namespace execution
}  // namespace tibee
