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

#include "base/BasicTypes.hpp"

namespace tibee
{

// Services.
extern const char kExecutionsBuilderServiceName[];
extern const char kStacksBuilderServiceName[];
extern const char kCriticalGraphServiceName[];

// Metrics.
typedef uint32_t MetricId;
extern const char* kMetricNames[];
extern const size_t kNumMetrics;
const uint32_t kDurationMetricId = 0;
const uint32_t kTsMetricId = 1;
const uint32_t kNumCustomMetrics = 2;

}  // namespace tibee

#endif // _TIBEE_BASE_COMPARECONSTANTS_HPP
