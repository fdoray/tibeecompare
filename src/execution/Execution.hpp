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
#include <set>
#include <string>
#include <unordered_map>

#include "base/BasicTypes.hpp"
#include "execution/ExecutionId.hpp"
#include "quark/Quark.hpp"

namespace tibee
{
namespace execution
{

class Execution
{
public:
    typedef std::unique_ptr<Execution> UP;
    typedef std::unordered_map<quark::Quark, uint64_t> Metrics;

    Execution() 
        : _needsToEnd(true),
          _startTs(0), _startThread(-1),
          _endTs(0), _endThread(-1) {}
    ~Execution() {}

    // Identifier of the execution.
    const ExecutionId& id() const { return _id; }
    void set_id(const ExecutionId& id) { _id = id; }

    // Name of the execution.
    const std::string& name() const { return _name; }
    void set_name(const std::string& name) { _name = name; }

    // Trace of the execution.
    const std::string& trace() const { return _trace; }
    void set_trace(const std::string& trace) { _trace = trace; }

    // Indicates whether the execution needs to end.
    bool needsToEnd() const { return _needsToEnd; }
    void set_needsToEnd(bool needsToEnd) { _needsToEnd = needsToEnd; }

    // Threads that belong to the execution.
    const std::set<thread_t>& threads() const { return _threads; }
    void AddThread(thread_t tid) { _threads.insert(tid); }

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

    // Metrics.
    bool GetMetric(quark::Quark key, uint64_t* value) const {
        auto look = _metrics.find(key);
        if (look == _metrics.end())
            return false;
        *value = look->second;
        return true;
    }
    void SetMetric(quark::Quark key, uint64_t value) {
        _metrics[key] = value;
    }
    Metrics::const_iterator metrics_begin() const {
        return _metrics.begin();
    }
    Metrics::const_iterator metrics_end() const {
        return _metrics.end();
    }
    size_t MetricsCount() const {
        return _metrics.size();
    }

private:
    // Identifier of the execution.
    ExecutionId _id;

    // Name of the execution.
    std::string _name;

    // Trace of the execution.
    std::string _trace;

    // Indicates whether the execution needs to end.
    bool _needsToEnd;

    // Threads that belong to the execution.
    std::set<thread_t> _threads;

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
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_EXECUTION_HPP
