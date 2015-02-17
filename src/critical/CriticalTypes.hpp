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
#ifndef TIBEE_CRITICAL_CRITICALTYPES_HPP_
#define TIBEE_CRITICAL_CRITICALTYPES_HPP_

#include <stddef.h>

namespace tibee
{
namespace critical
{

typedef size_t CriticalEdgeId;

const CriticalEdgeId kInvalidCriticalEdgeId = -1;

enum CriticalEdgePosition {
    kCriticalEdgeOutVertical = 0,
    kCriticalEdgeInVertical = 1,
    kCriticalEdgeOutHorizontal = 2,
    kCriticalEdgeInHorizontal = 3,
    kCriticalEdgePositionCount = 4,
};

enum CriticalEdgeType {
    kUnknown = 0,     // Unknown type.
    kVertical,        // Vertical edge.
    kRun,             // Run.
    kInterrupted,     // Interrupted.
    kWaitCpu,         // Waiting for CPU.
    kWaitBlocked,     // Waiting blocked.
    kTimer,			  // Waiting on a timer.
    kNetwork,         // Waiting for the network.
    kBlockDevice,     // Waiting for a block device.
    kUserInput,       // Waiting for user input.
};

}  // namespace critical
}  // namespace tibee

#endif  // TIBEE_CRITICAL_CRITICALTYPES_HPP_
