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
#ifndef _TIBEE_EXECUTION_LINK_HPP
#define _TIBEE_EXECUTION_LINK_HPP

#include "base/BasicTypes.hpp"

namespace tibee
{
namespace execution
{

class Link
{
public:
    Link()
        : _sourceThread(-1), _sourceTs(-1),
          _targetThread(-1), _targetTs(-1) {}
    Link(thread_t sourceThread, timestamp_t sourceTs,
         thread_t targetThread, timestamp_t targetTs)
        : _sourceThread(sourceThread), _sourceTs(sourceTs),
          _targetThread(targetThread), _targetTs(targetTs) {}

    // Source thread.
    thread_t sourceThread() const { return _sourceThread; }

    // Source timestamp.
    timestamp_t sourceTs() const { return _sourceTs; }

    // Target thread.
    thread_t targetThread() const { return _targetThread; }

    // Target timestamp.
    timestamp_t targetTs() const { return _targetTs; }

    bool operator==(const Link& other) const {
        return _sourceThread == other._sourceThread &&
            _sourceTs == other._sourceTs &&
            _targetThread == other._targetThread &&
            _targetTs == other._targetTs;
    }

private:
    thread_t _sourceThread;
    timestamp_t _sourceTs;

    thread_t _targetThread;
    timestamp_t _targetTs;
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_LINK_HPP
