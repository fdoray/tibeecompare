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
    // Cannot create an execution if there is
    // already a segment on the thread.
    auto look = _segments.find(thread);
    if (look != _segments.end())
        return false;

    // Create execution.
    Execution::UP execution(new Execution);
    execution->set_name(name);
    execution->set_trace(_trace);
    execution->set_startTs(_ts);
    execution->set_startThread(thread);
    execution->set_endTs(_ts);
    execution->set_endThread(thread);

    auto executionIndex = _executions.size();
    _executions.push_back(std::move(execution));
    _executionNeedsToEnd.push_back(needsToEnd);

    // Create initial segment.
    SegmentWrapper segment;
    segment.executionIndex = executionIndex;
    segment.segment.set_thread(thread);
    segment.segment.set_startTs(_ts);
    _segments[thread] = segment;

    return true;
}

void ExecutionsBuilder::StartSegment(thread_t parent, thread_t child)
{
    // If there is no segment on the parent, we cannot
    // create the child segment.
    auto lookParent = _segments.find(parent);
    if (lookParent == _segments.end())
        return;
    auto executionIndex = lookParent->second.executionIndex;

    auto lookChild = _segments.find(child);
    if (lookChild != _segments.end())
    {
        // If there is already a segment on the thread,
        // end it at (_ts - 1).
        --_ts;
        EndSegment(child);
        ++_ts;
    }

    // Start the new segment.
    SegmentWrapper segment;
    segment.executionIndex = executionIndex;
    segment.segment.set_thread(child);
    segment.segment.set_startTs(_ts);
    segment.segment.set_endTs(_ts);
    _segments[child] = segment;
}

void ExecutionsBuilder::EndSegment(thread_t thread)
{
    auto look = _segments.find(thread);
    if (look == _segments.end())
        return;

    auto executionIndex = look->second.executionIndex;
    auto& execution = _executions[executionIndex];
    auto& segment = look->second.segment;
    segment.set_endTs(_ts);
    execution->AddSegment(segment);
    execution->set_endTs(_ts);

    _segments.erase(look);
}

void ExecutionsBuilder::Terminate()
{
    std::vector<thread_t> threads;
    for (const auto& segment : _segments)
    {
        auto executionIndex = segment.second.executionIndex;
        if (!_executionNeedsToEnd[executionIndex])
            threads.push_back(segment.first);
    }

    for (const auto& thread : threads)
        EndSegment(thread);
}

}  // namespace execution
}  // namespace tibee
