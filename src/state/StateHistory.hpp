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
#ifndef TIBEE_STATE_STATEHISTORY_HPP_
#define TIBEE_STATE_STATEHISTORY_HPP_

#include <functional>
#include <unordered_map>
#include <vector>

#include "base/BasicTypes.hpp"
#include "state/AttributeKey.hpp"

namespace tibee
{
namespace state
{

class StateHistory {
public:
    typedef std::function<void (
        uint32_t value, timestamp_t start, timestamp_t end)> EnumerateUIntegerValuesCallback;

    StateHistory();
    ~StateHistory();

    // Set the current timestamp.
    void SetTimestamp(timestamp_t ts) { _ts = ts; }

    // Set/get the current value for an unsigned integer entry.
    void SetUIntegerValue(AttributeKey key, uint32_t value);
    bool GetUIntegerValue(AttributeKey key, timestamp_t ts, uint32_t* value) const;

    // Enumerate the values of a state in the specified interval.
    void EnumerateUIntegerValues(
        AttributeKey key, timestamp_t start, timestamp_t end,
        const EnumerateUIntegerValuesCallback& callback) const;

    // Set/get the current value for an unsigned integer entry.
    void SetULongValue(AttributeKey key, uint64_t value);
    bool GetULongValue(AttributeKey key, timestamp_t ts, uint64_t* value) const;

    // Helpers for performance counters.
    void SetPerfCounterCpuBaseValue(AttributeKey key, uint64_t value);
    void SetPerfCounterCpuValue(AttributeKey key, uint64_t value);
    void SetPerfCounterThreadValue(AttributeKey key, uint64_t value);

private:
    uint64_t GetULongLastValue(AttributeKey key) const;

    // Current timestamp.
    timestamp_t _ts;

    // Entry.
    struct Entry
    {
        Entry() : ts(0) {}
        timestamp_t ts;
    };
    class EntryComparator {
    public:
        bool operator()(const Entry& stack, timestamp_t ts) const {
            return stack.ts < ts;
        }
        bool operator()(timestamp_t ts, const Entry& stack) const {
            return ts < stack.ts;
        }
    };

    // History of unsigned integer values.
    struct UIntegerEntry : Entry
    {
        UIntegerEntry() : value(0) {}
        uint32_t value;
    };
    typedef std::unordered_map<AttributeKey, std::vector<UIntegerEntry>> UIntegerHistory;
    UIntegerHistory _uIntegerHistory;

    // History of long unsigned values.
    struct ULongEntry : Entry {
        ULongEntry() : value(0) {}
        uint64_t value;
    };
    typedef std::unordered_map<AttributeKey, std::vector<ULongEntry>> ULongHistory;
    ULongHistory _uLongHistory;

    // Previous values for performance counters.
    static const uint64_t kInvalid = -1;
    struct PerfCounterState
    {
        PerfCounterState()
            : cpuAbsolute(kInvalid), cpuReal(kInvalid),
              threadAbsolute(kInvalid), threadReal(kInvalid) {}
        uint64_t cpuAbsolute;
        uint64_t cpuReal;
        uint64_t threadAbsolute;
        uint64_t threadReal;
    };
    typedef std::unordered_map<AttributeKey, PerfCounterState> PerfCounterStates;
    PerfCounterStates _perfCounterStates;
};

}  // namespace state
}  // namespace tibee

#endif  // TIBEE_STATE_STATEHISTORY_HPP_
