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
    StateHistory();
    ~StateHistory();

    // Set the current timestamp.
    void SetTimestamp(timestamp_t ts) { _ts = ts; }

    // Set the current value for an unsigned integer entry.
    void SetUIntegerValue(AttributeKey key, uint32_t value);

    // Get the value for an unsigned integer entry.
    bool GetUIntegerValue(AttributeKey key, timestamp_t ts, uint32_t* value) const;

private:
    // Current timestamp.
    timestamp_t _ts;

    // History of unsigned integer values.
    struct UIntegerEntry
    {
        UIntegerEntry() : ts(0), value(0) {}
        timestamp_t ts;
        uint32_t value;
    };
    typedef std::unordered_map<AttributeKey, std::vector<UIntegerEntry>> UIntegerHistory;
    UIntegerHistory _uIntegerHistory;

    class UIntegerEntryComparator
    {
    public:
        bool operator() (const UIntegerEntry& stack, timestamp_t ts) const
        {
            return stack.ts < ts;
        }
        bool operator() (timestamp_t ts, const UIntegerEntry& stack) const
        {
            return ts < stack.ts;
        }
    };
};

}  // namespace state
}  // namespace tibee

#endif  // TIBEE_STATE_STATEHISTORY_HPP_
