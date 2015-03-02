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
#include "base/CompareConstants.hpp"

namespace tibee
{
const char kExecutionsBuilderServiceName[] = "executions-builder";
const char kStacksBuilderServiceName[] = "stacks-builder";
const char kCriticalGraphServiceName[] = "critical-graph";
const char kStateHistoryServiceName[] = "state-history";

const char kInstructions[] = "instructions";
const char kCacheReferences[] = "cache-references";
const char kCacheMisses[] = "cache-misses";
const char kBranchInstructions[] = "branch-instructions";
const char kBranches[] = "branches";
const char kBranchMisses[] = "branch-misses";
const char kBranchLoads[] = "branch-loads";
const char kBranchLoadMisses[] = "branch-load-misses";
const char kPageFault[] = "page-fault";
const char kFaults[] = "faults";
const char kMajorFaults[] = "major-faults";
const char kMinorFaults[] = "minor-faults";

const char* kMetricNames[] = {
    "duration",
    "timestamp",
    "unknown",
    "vertical",
    "run",
    "interrupted",
    "waitcpu",
    "waitblocked",
    "timer",
    "network",
    "blockdevice",
    "input",
};
const size_t kNumMetrics = sizeof(kMetricNames) / sizeof(char*);

uint32_t kPerformanceCounterFirstMetricId = kNumMetrics;
const char* kPerformanceCounters[] = {
    "instructions",
    "cache-references",
    "cache-misses",
    "branch-instructions",
    "branches",
    "branch-misses",
    "L1-dcache-loads",
    "L1-dcache-load-misses",
    "L1-dcache-stores",
    "L1-dcache-store-misses",
    "L1-dcache-prefetches",
    "L1-dcache-prefetch-misses",
    "L1-icache-loads",
    "L1-icache-load-misses",
    "L1-icache-stores",
    "L1-icache-store-misses",
    "L1-icache-prefetches",
    "L1-icache-prefetch-misses",
    "LLC-loads",
    "LLC-load-misses",
    "LLC-stores",
    "LLC-store-misses",
    "LLC-prefetches",
    "LLC-prefetch-misses",
    "dTLB-loads",
    "perf:thread:dTLB-load-misses",
    "dTLB-stores",
    "dTLB-store-misses",
    "dTLB-prefetches",
    "dTLB-prefetch-misses",
    "iTLB-loads",
    "iTLB-load-misses",
    "branch-loads",
    "branch-load-misses",
    "cpu-clock",
    "task-clock",
    "page-fault",
    "faults",
    "major-faults",
    "minor-faults",
    "context-switches",
    "cs",
    "cpu-migrations",
    "migrations",
    "alignment-faults",
    "emulation-faults"
};
const size_t kNumPerformanceCounters =
        sizeof(kPerformanceCounters) / sizeof(char*);

}
