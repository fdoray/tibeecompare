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
#ifndef _TIBEE_EXECUTION_EXECUTIONSBUILDER_HPP
#define _TIBEE_EXECUTION_EXECUTIONSBUILDER_HPP

#include <unordered_map>
#include <vector>

#include "base/BasicTypes.hpp"
#include "execution/Execution.hpp"

namespace tibee
{
namespace execution
{

class ExecutionsBuilder
{
public:
    typedef std::vector<Execution::UP> Executions;

    ExecutionsBuilder();
    ~ExecutionsBuilder();

    // Set current state.
    void SetTimestamp(timestamp_t ts) { _ts = ts; }

    // Create execution.
    bool StartExecution(
        thread_t thread,
        const std::string& name,
        bool needsToEnd);

    // End an execution.
    timestamp_t EndExecution(thread_t thread);

    // Complete active executions that don't need to end.
    void Terminate();

    // Clear the list of completed executions.
    void Flush() {
        _completedExecutions.clear();
    }

    // Traverse completed executions.
    Executions::iterator begin() {
        return _completedExecutions.begin();
    }
    Executions::iterator end() {
        return _completedExecutions.end();
    }

private:
    // Current timestamp.
    timestamp_t _ts;

    // Completed executions.
    Executions _completedExecutions;

    // Active executions.
    std::unordered_map<thread_t, Execution::UP> _activeExecutions;

    // Indicates whether the current executions needs to end.
    std::unordered_map<thread_t, bool> _needsToEnd;
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_EXECUTIONSBUILDER_HPP
