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

    UIntegerEntryComparator comparator;
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

}  // namespace state
}  // namespace tibee
