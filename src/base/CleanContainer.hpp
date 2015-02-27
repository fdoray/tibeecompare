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
#ifndef TIBEE_BASE_CLEANCONTAINER_HPP_
#define TIBEE_BASE_CLEANCONTAINER_HPP_

#include <vector>

namespace tibee
{
namespace base
{

template <typename T>
void CleanVector(typename T::iterator begin,
		         typename T::iterator end,
				 T* vector)
{
	T(begin, end).swap(*vector);
}

}  // namespace base
}  // namespace tibee



#endif  // TIBEE_BASE_CLEANCONTAINER_HPP_
