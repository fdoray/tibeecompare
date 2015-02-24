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
#include "execution/ExtractMetrics.hpp"

#include "base/CompareConstants.hpp"
#include "base/print.hpp"

namespace tibee
{
namespace execution
{

void ExtractMetrics(
    const critical::CriticalPath& criticalPath,
    Execution* execution)
{
    // Duration.
    auto duration = execution->endTs() - execution->startTs();
    execution->SetMetric(kDurationMetricId, duration);

    // Timestamp.
    execution->SetMetric(kTsMetricId, execution->startTs());

    // Status.
    uint64_t criticalPathDuration = 0;
    for (const auto& segment : criticalPath)
    {
        auto metricId = kNumCustomMetrics + segment.type();
        uint64_t segmentDuration = segment.endTs() - segment.startTs();
        uint64_t currentValue = 0;
        execution->GetMetric(metricId, &currentValue);
        execution->SetMetric(metricId, currentValue + segmentDuration);
        criticalPathDuration += segmentDuration;
    }

    if (duration != criticalPathDuration)
    {
        base::tberror() << "Critical path is not the same duration as the "
                           "execution..." << base::tbendl();
    }
}

}  // namespace execution
}  // namespace tibee
