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
    void SetTrace(const std::string& trace) { _trace = trace; }

    // Create execution.
    bool StartExecution(
        thread_t thread,
        const std::string& name,
        bool needsToEnd);

    // Start a segment.
    // If a segment already exists on the thread, it is
    // ended and a new segment is started.
    void StartSegment(thread_t parent, thread_t child);

    // End a segment.
    void EndSegment(thread_t thread);

    // Complete all executions that don't need to end.
    void Terminate();

    // Traverse executions.
    size_t ExecutionsCount() const {
        return _executions.size();
    }
    Executions::iterator begin() {
        return _executions.begin();
    }
    Executions::iterator end() {
        return _executions.end();
    }

private:
    typedef size_t ExecutionIndex;

    // Current timestamp.
    timestamp_t _ts;

    // Current trace.
    std::string _trace;

    // Executions.
    Executions _executions;

    // Indicates which executions need to end.
    std::vector<bool> _executionNeedsToEnd;

    // A segment wrapper contains a segment and the
    // index of the execution it belongs to.
    struct SegmentWrapper
    {
        ExecutionIndex executionIndex;
        Segment segment;
    };

    // Current segment per thread.
    typedef std::unordered_map<thread_t, SegmentWrapper> SegmentPerThread;
    SegmentPerThread _segments;
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_EXECUTIONSBUILDER_HPP
