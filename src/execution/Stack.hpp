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
#ifndef _TIBEE_EXECUTION_STACK_HPP
#define _TIBEE_EXECUTION_STACK_HPP

#include "execution/Identifiers.hpp"

namespace tibee
{
namespace execution
{

class Stack
{
public:
    Stack() : _function(-1), _bottom(-1) {}
    Stack(FunctionNameId function, StackId bottom)
        : _function(function), _bottom(bottom) {}

    // Function on top of the stack.
    FunctionNameId function() const { return _function; }
    void set_function(FunctionNameId function) { _function = function; }

    // Stack below the top function.
    StackId bottom() const { return _bottom; }
    void set_bottom(StackId bottom) { _bottom = bottom; }

    bool operator==(const Stack& other) const
    {
        return _function == other._function && _bottom == other._bottom;
    }

private:
    FunctionNameId _function;
    StackId _bottom;
};

}  // namespace execution
}  // namespace tibee

#endif // _TIBEE_EXECUTION_STACK_HPP
