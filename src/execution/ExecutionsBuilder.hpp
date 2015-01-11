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
    typedef std::vector<Execution::UP> CompletedExecutions;

    ExecutionsBuilder();
    ~ExecutionsBuilder();

    // Set current state.
    void SetTimestamp(timestamp_t ts) { _ts = ts; }
    void SetTrace(const std::string& trace) { _trace = trace; }

    // Create executions.
    void StartThreadExecution(thread_t thread, const std::string& name);
    void EndThreadExecution(thread_t thread);

    // Traverse executions.
    size_t ExecutionsCount() const { return _completedExecutions.size(); }
    CompletedExecutions::iterator begin() {
        return _completedExecutions.begin();
    }
    CompletedExecutions::iterator end() {
        return _completedExecutions.end();
    }

private:
    // Current timestamp.
    timestamp_t _ts;

    // Current trace.
    std::string _trace;

    // Current executions.
    typedef std::unordered_map<thread_t, Execution::UP> CurrentExecutions;
    CurrentExecutions _currentExecutions;

    // Completed executions.
    CompletedExecutions _completedExecutions;
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_EXECUTIONSBUILDER_HPP
