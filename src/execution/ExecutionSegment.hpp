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
#ifndef _TIBEE_EXECUTION_EXECUTIONSEGMENT_HPP
#define _TIBEE_EXECUTION_EXECUTIONSEGMENT_HPP

#include "base/BasicTypes.hpp"

namespace tibee
{
namespace execution
{

class ExecutionSegment
{
public:
    ExecutionSegment()
        : _thread(-1), _startTs(0), _endTs(0) {}

    thread_t thread() const { return _thread; }
    void set_thread(thread_t thread) { _thread = thread; }

    timestamp_t startTs() const { return _startTs; }
    void set_startTs(timestamp_t startTs) { _startTs = startTs; }

    timestamp_t endTs() const { return _endTs; }
    void set_endTs(timestamp_t endTs) { _endTs = endTs; }

    bool operator==(const ExecutionSegment& other) const {
        return _thread == other._thread &&
            _startTs == other._startTs &&
            _endTs == other._endTs;
    }

    bool operator<(const ExecutionSegment& other) const {
        if (_thread < other._thread)
            return true;
        if (_thread > other._thread)
            return false;
        if (_startTs < other._startTs)
            return true;
        if (_startTs > other._startTs)
            return false;
        return _endTs < other._endTs;
    }

private:
    // Thread.
    thread_t _thread;

    // Start timestamp.
    timestamp_t _startTs;

    // End timestamp.
    timestamp_t _endTs;
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_EXECUTIONSEGMENT_HPP
