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
#include "state/StateHistory.hpp"

#include <algorithm>
#include <iostream>

#include "base/CleanContainer.hpp"

namespace tibee
{
namespace state
{

namespace
{

}

StateHistory::StateHistory()
    : _ts(0)
{
}

StateHistory::~StateHistory()
{
}

void StateHistory::Cleanup(timestamp_t ts)
{
	for (auto& keyHistory : _uIntegerHistory)
	{
	    if (keyHistory.second.empty())
	        continue;

		EntryComparator comparator;
		auto it = std::lower_bound(
		    keyHistory.second.begin(), keyHistory.second.end(), ts, comparator);
		if (it == keyHistory.second.begin())
		    continue;
		--it;
		base::CleanVector(it, keyHistory.second.end(), &keyHistory.second);
	}

	for (auto& keyHistory : _uLongHistory)
	{
        if (keyHistory.second.empty())
            continue;

        EntryComparator comparator;
        auto it = std::lower_bound(
            keyHistory.second.begin(), keyHistory.second.end(), ts, comparator);
        if (it == keyHistory.second.begin())
            continue;
        --it;
        base::CleanVector(it, keyHistory.second.end(), &keyHistory.second);
	}
}

void StateHistory::SetUIntegerValue(AttributeKey key, uint32_t value)
{
    auto& keyHistory = _uIntegerHistory[key];
    if (!keyHistory.empty() && keyHistory.back().value == value)
        return;

    UIntegerEntry entry;
    entry.ts = _ts;
    entry.value = value;
    keyHistory.push_back(entry);
}

bool StateHistory::GetUIntegerValue(AttributeKey key, timestamp_t ts, uint32_t* value) const
{
    auto look = _uIntegerHistory.find(key);
    if (look == _uIntegerHistory.end())
        return false;

    const auto& keyHistory = look->second;

    EntryComparator comparator;
    auto it = std::upper_bound(
        keyHistory.begin(), keyHistory.end(), ts, comparator);
    if (it != keyHistory.begin())
        --it;
    if (it == keyHistory.end())
        return false;

    if (it->ts <= ts)
    {
        *value = it->value;
        return true;
    }
    return false;
}

void StateHistory::EnumerateUIntegerValues(
    AttributeKey key, timestamp_t start, timestamp_t end,
    const EnumerateUIntegerValuesCallback& callback) const
{
    auto look = _uIntegerHistory.find(key);
    if (look == _uIntegerHistory.end())
        return;

    const auto& keyHistory = look->second;

    EntryComparator comparator;
    auto it = std::upper_bound(keyHistory.begin(), keyHistory.end(), start,
            comparator);
    if (it != keyHistory.begin())
        --it;
    while (it != keyHistory.end())
    {
        if (it->ts >= end)
            return;

        timestamp_t intervalStart = std::max(it->ts, start);
        timestamp_t intervalEnd = end;
        auto next = it + 1;
        if (next != keyHistory.end())
            intervalEnd = std::min(next->ts, end);

        callback(it->value, intervalStart, intervalEnd);

        ++it;
    }
}

void StateHistory::SetULongValue(AttributeKey key, uint64_t value)
{
    auto& keyHistory = _uIntegerHistory[key];
    if (!keyHistory.empty() && keyHistory.back().value == value)
        return;

    UIntegerEntry entry;
    entry.ts = _ts;
    entry.value = value;
    keyHistory.push_back(entry);
}

bool StateHistory::GetULongValue(AttributeKey key, timestamp_t ts, uint64_t* value) const
{
    auto look = _uIntegerHistory.find(key);
    if (look == _uIntegerHistory.end())
        return false;

    const auto& keyHistory = look->second;

    EntryComparator comparator;
    auto it = std::upper_bound(
        keyHistory.begin(), keyHistory.end(), ts, comparator);
    if (it != keyHistory.begin())
        --it;
    if (it == keyHistory.end())
        return false;

    if (it->ts <= ts)
    {
        *value = it->value;
        return true;
    }
    return false;
}

void StateHistory::SetPerfCounterCpuBaseValue(AttributeKey key, uint64_t value)
{
    auto& state = _perfCounterStates[key];
    state.cpuAbsolute = value;
    state.cpuReal = GetULongLastValue(key);
}

void StateHistory::SetPerfCounterCpuValue(AttributeKey key, uint64_t value)
{
    // Make sure that this is not the first value we get for this attribute.
    auto& state = _perfCounterStates[key];
    if (state.cpuAbsolute == kInvalid || state.cpuAbsolute > value)
    {
        SetPerfCounterCpuBaseValue(key, value);
        return;
    }

    // Compute the minimum value that the thread perf counter must have now.
    auto minDiff = value - state.cpuAbsolute;
    auto minReal = state.cpuReal + minDiff;

    // Find the current value of the performance counter.
    uint64_t last = GetULongLastValue(key);

    // Set the new value of the performance counter.
    auto real = std::max(last, minReal);
    SetULongValue(key, real);

    // Keep track of the state.
    state.cpuAbsolute = value;
    state.cpuReal = real;
}

void StateHistory::SetPerfCounterThreadValue(AttributeKey key, uint64_t value)
{
    auto& state = _perfCounterStates[key];

    if (state.threadAbsolute == kInvalid)
    {
        // This is the first thread value we got. Keep track of it.
        state.threadAbsolute = value;
        state.threadReal = GetULongLastValue(key);
        return;
    }

    // Compute the minimum value that the thread perf counter must have now.
    auto minDiff = value - state.threadAbsolute;
    auto minReal = state.threadReal + minDiff;

    // Find the current value of the performance counter.
    uint64_t last = GetULongLastValue(key);

    // Set the new value of the performance counter.
    auto real = std::max(last, minReal);
    SetULongValue(key, real);

    // Keep track of the state.
    state.threadAbsolute = value;
    state.threadReal = real;
}

uint64_t StateHistory::GetULongLastValue(AttributeKey key) const
{
    uint64_t last = 0;
    auto look = _uLongHistory.find(key);
    if (look != _uLongHistory.end() && !look->second.empty())
        last = look->second.back().value;
    return last;
}

}  // namespace state
}  // namespace tibee
