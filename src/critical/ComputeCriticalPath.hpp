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
#ifndef TIBEE_CRITICAL_COMPUTECRITICALPATH_HPP_
#define TIBEE_CRITICAL_COMPUTECRITICALPATH_HPP_

#include "base/BasicTypes.hpp"
#include "critical/CriticalGraph.hpp"
#include "critical/CriticalPath.hpp"

namespace tibee
{
namespace critical
{

void ComputeCriticalPath(
    const CriticalGraph& graph,
    timestamp_t startTs,
    timestamp_t endTs,
    thread_t tid,
    CriticalPath* path);

}  // namespace critical
}  // namespace tibee

#endif  // TIBEE_CRITICAL_COMPUTECRITICALPATH_HPP_
