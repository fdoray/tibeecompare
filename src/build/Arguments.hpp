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
#ifndef _TIBEE_BUILD_ARGUMENTS_HPP
#define _TIBEE_BUILD_ARGUMENTS_HPP

#include <vector>
#include <string>

namespace tibee
{
namespace build
{

/**
 * Program arguments.
 *
 * @author Francois Doray
 */
struct Arguments
{
    // Name of the executions.
    std::string name;

    // Event beginning an execution. Prepend with ust/ or kernel/.
    std::string beginEvent;

    // Event ending an execution. Prepend with ust/ or kernel/.
    std::string endEvent;

    // Executable to analyze (optional).
    std::string exec;

    // Traces to analyze.
    std::vector<std::string> traces;

    // Dump the stacks found in the trace, do not track executions.
    bool dumpStacks;

    // Show execution duration statistics, do not track executions.
    bool stats;

    // Execute the special block!
    bool special;

    // Verbose flag.
    bool verbose;
};

}  // namespace build
}  // namespace tibee

#endif // _TIBEE_BUILD_ARGUMENTS_HPP
