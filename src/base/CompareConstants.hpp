/* Copyright (c) 2014 Francois Pierre Doray <francois.pierre-doray@polymtl.ca>
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
#ifndef _TIBEE_BASE_COMPARECONSTANTS_HPP
#define _TIBEE_BASE_COMPARECONSTANTS_HPP

#include <stddef.h>
#include <stdint.h>

namespace tibee
{

// A huge cost.
const uint64_t kHugeCost = -1;

// Mongo.
const size_t kOIDLength = 24;

// Services.
extern const char kStacksBuilderServiceName[];

// Block names.
// - Trace
extern const char kTraceBlockName[];
// - State
extern const char kCurrentStateBlockName[];
extern const char kLinuxSchedStateBlockName[];
// - Execution
extern const char kExecutionBlockName[];

// Execution: performance counters.
extern const char kInstructions[];
extern const char kCacheReferences[];
extern const char kCacheMisses[];
extern const char kBranchInstructions[];
extern const char kBranches[];
extern const char kBranchMisses[];
extern const char kBranchLoads[];
extern const char kBranchLoadMisses[];
extern const char kPageFault[];
extern const char kFaults[];
extern const char kMajorFaults[];
extern const char kMinorFaults[];

}  // namespace tibee

#endif // _TIBEE_BASE_COMPARECONSTANTS_HPP
