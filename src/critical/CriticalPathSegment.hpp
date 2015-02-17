/* Copyright (c) 2015 Francois Doray <francois.pierre-doray@polymtl.ca>
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
#ifndef TIBEE_CRITICAL_CRITICALPATHSEGMENT_HPP_
#define TIBEE_CRITICAL_CRITICALPATHSEGMENT_HPP_

#include "base/BasicTypes.hpp"
#include "critical/CriticalTypes.hpp"

namespace tibee
{
namespace critical
{

class CriticalPathSegment
{
public:
    CriticalPathSegment()
        : _startTs(0), _endTs(0), _tid(-1), _type(kUnknown) {}
    CriticalPathSegment(
            timestamp_t startTs, timestamp_t endTs, thread_t tid, CriticalEdgeType type)
            : _startTs(startTs), _endTs(endTs), _tid(tid), _type(type) {}

    timestamp_t startTs() const { return _startTs; }
    timestamp_t endTs() const { return _endTs; }
    thread_t tid() const { return _tid; }
    CriticalEdgeType type() const { return _type; }

    void set_endTs(timestamp_t endTs) { _endTs = endTs; }

    bool operator==(const CriticalPathSegment& other) const {
        return _startTs == other._startTs &&
               _endTs == other._endTs &&
               _tid == other._tid &&
               _type == other._type;
    }

private:
    // Start timestamp.
    timestamp_t _startTs;

    // End timestamp.
    timestamp_t _endTs;

    // Thread.
    thread_t _tid;

    // Type.
    CriticalEdgeType _type;
};

}  // namespace critical
}  // namespace tibee

#endif  // TIBEE_CRITICAL_CRITICALPATHSEGMENT_HPP_
