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
#ifndef _TIBEE_EXECUTION_EXECUTION_HPP
#define _TIBEE_EXECUTION_EXECUTION_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "base/BasicTypes.hpp"
#include "base/CompareConstants.hpp"
#include "stacks/Identifiers.hpp"

namespace tibee
{
namespace execution
{

class Execution
{
public:
    typedef std::unique_ptr<Execution> UP;
    typedef std::unordered_map<MetricId, uint64_t> Metrics;
    typedef std::unordered_map<stacks::StackId, uint64_t> Samples;

    Execution() 
        : _startTs(0), _startThread(-1),
          _endTs(0), _endThread(-1) {}
    ~Execution() {}

    // Name of the execution.
    const std::string& name() const { return _name; }
    void set_name(const std::string& name) { _name = name; }

    // Trace of the execution.
    const std::string& trace() const { return _trace; }
    void set_trace(const std::string& trace) { _trace = trace; }

    // Start time of the execution.
    timestamp_t startTs() const { return _startTs; }
    void set_startTs(timestamp_t startTs) { _startTs = startTs; }

    // Start thread of the execution.
    thread_t startThread() const { return _startThread; }
    void set_startThread(thread_t startThread) { _startThread = startThread; }

    // End time of the execution.
    timestamp_t endTs() const { return _endTs; }
    void set_endTs(timestamp_t endTs) { _endTs = endTs; }

    // End thread of the execution.
    thread_t endThread() const { return _endThread; }
    void set_endThread(thread_t endThread) { _endThread = endThread; }

    // Metrics of the execution.
    size_t metrics_size() const {
        return _metrics.size();
    }
    Metrics::const_iterator metrics_begin() const {
        return _metrics.begin();
    }
    Metrics::const_iterator metrics_end() const {
        return _metrics.end();
    }

    bool GetMetric(MetricId metricId, uint64_t* value) const {
        auto look = _metrics.find(metricId);
        if (look == _metrics.end())
            return false;
        *value = look->second;
        return true;
    }
    void SetMetric(MetricId metricId, uint64_t value) {
        _metrics[metricId] = value;
    }

    // Samples of the execution.
    size_t samples_size() const {
        return _samples.size();
    }
    Samples::const_iterator samples_begin() const {
        return _samples.begin();
    }
    Samples::const_iterator samples_end() const {
        return _samples.end();
    }
    void IncrementSample(stacks::StackId stackId, uint64_t value) {
        _samples[stackId] += value;
    }

    // Equal operator.
    bool operator==(const Execution& other) const {
        return _name == other._name &&
            _trace == other._trace &&
            _startTs == other._startTs &&
            _startThread == other._startThread &&
            _endTs == other._endTs &&
            _endThread == other._endThread &&
            _metrics == other._metrics &&
            _samples == other._samples;
    }

private:
    // Name of the execution.
    std::string _name;

    // Trace of the execution.
    std::string _trace;

    // Start time of the execution.
    timestamp_t _startTs;

    // Start thread of the execution.
    thread_t _startThread;

    // End time of the execution.
    timestamp_t _endTs;

    // End thread of the execution.
    thread_t _endThread;

    // Metrics of the execution.
    Metrics _metrics;

    // Samples of the execution.
    Samples _samples;
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_EXECUTION_HPP
