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
#ifndef _TIBEE_CONTAINERS_INTERVAL_HPP
#define _TIBEE_CONTAINERS_INTERVAL_HPP

#include "base/BasicTypes.hpp"

namespace tibee
{
namespace containers
{

// Interval. The 2 bounds of the interval are included in it.
class Interval
{
public:
    // Default constructor. Constructs the interval [0, 0].
    Interval();

    // Constructs the interval [|low|, |high|].
    // @param low the lower bound of the interval.
    // @param high the higher bound of the interval.
    Interval(uint64_t low, uint64_t high);

    // @returns the lower bound of the interval.
    uint64_t low() const { return _low; }

    // @returns the higer bound of the interval.
    uint64_t high() const { return _high; }

    // Indicates if the 2 intervals are equal, i.e. have
    // the same lower and higher bounds.
    bool operator==(const Interval& other) const {
        return low() == other.low() && high() == other.high();
    }

private:
    uint64_t _low;
    uint64_t _high;
};

}  // namespace containers
}  // namespace tibee

#endif // _TIBEE_CONTAINERS_INTERVAL_HPP
