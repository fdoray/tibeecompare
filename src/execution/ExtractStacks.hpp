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
#ifndef _TIBEE_EXECUTION_EXTRACTSTACKS_HPP
#define _TIBEE_EXECUTION_EXTRACTSTACKS_HPP

#include "critical/Segment.hpp"
#include "execution/Execution.hpp"
#include "execution/StacksBuilder.hpp"

namespace tibee
{
namespace execution
{

void ExtractStacks(
    const StacksBuilder& stacks,
    const critical::Segments& segments,
    Execution* execution);

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_EXTRACTSTACKS_HPP
