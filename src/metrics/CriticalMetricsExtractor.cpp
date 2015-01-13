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
#include "metrics/CriticalMetricsExtractor.hpp"

#include <unordered_set>

#include "value/MakeValue.hpp"

namespace tibee
{
namespace metrics
{

/**
 * Extracts metrics from an execution.
 */
void ExtractCriticalMetrics(
    const critical::CriticalPath& criticalPath,
    timestamp_t endTime,
    quark::DiskQuarkDatabase* quarks,
    execution::Execution* execution)
{
    std::unordered_map<uint32_t, quark::Quark> typeQuarks;
    typeQuarks[static_cast<uint32_t>(critical::CriticalEdgeType::kUnknown)] = quarks->StrQuark("critical-unknown");
    typeQuarks[static_cast<uint32_t>(critical::CriticalEdgeType::kVertical)] = quarks->StrQuark("critical-vertical");
    typeQuarks[static_cast<uint32_t>(critical::CriticalEdgeType::kRunUsermode)] = quarks->StrQuark("critical-run-usermode");
    typeQuarks[static_cast<uint32_t>(critical::CriticalEdgeType::kRunSyscall)] = quarks->StrQuark("critical-run-syscall");
    typeQuarks[static_cast<uint32_t>(critical::CriticalEdgeType::kInterrupted)] = quarks->StrQuark("critical-interrupted");
    typeQuarks[static_cast<uint32_t>(critical::CriticalEdgeType::kWaitCpu)] = quarks->StrQuark("critical-wait-cpu");
    typeQuarks[static_cast<uint32_t>(critical::CriticalEdgeType::kWaitBlocked)] = quarks->StrQuark("critical-wait-blocked");
    typeQuarks[static_cast<uint32_t>(critical::CriticalEdgeType::kTimer)] = quarks->StrQuark("critical-timer");
    typeQuarks[static_cast<uint32_t>(critical::CriticalEdgeType::kNetwork)] = quarks->StrQuark("critical-network");
    typeQuarks[static_cast<uint32_t>(critical::CriticalEdgeType::kBlockDevice)] = quarks->StrQuark("critical-block-device");
    typeQuarks[static_cast<uint32_t>(critical::CriticalEdgeType::kUserInput)] = quarks->StrQuark("critical-user-input");

    size_t i = 0;
    for (const auto& segment : criticalPath)
    {
        timestamp_t start = segment.ts;
        timestamp_t end = endTime;
        if (i != criticalPath.size() - 1) {
            end = (criticalPath.begin() + i + 1)->ts;
        }
        timestamp_t duration = end - start;

        quark::Quark metricQuark = typeQuarks[static_cast<uint32_t>(segment.type)];

        uint64_t currentValue = 0;
        execution->GetMetric(metricQuark, &currentValue);
        currentValue += duration;
        execution->SetMetric(metricQuark, currentValue);

        ++i;
    }
}

}  // namespace metrics
}  // namespace tibee