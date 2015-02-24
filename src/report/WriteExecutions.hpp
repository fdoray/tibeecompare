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
#ifndef _TIBEE_REPORT_WRITEEXECUTIONS_HPP
#define _TIBEE_REPORT_WRITEEXECUTIONS_HPP

#include <set>
#include <string>
#include <map>

#include "base/JsonWriter.hpp"
#include "db/Database.hpp"
#include "stacks/Identifiers.hpp"
#include "stacks/Stack.hpp"

namespace tibee
{
namespace report
{

typedef std::map<stacks::StackId, stacks::Stack> StacksMap;

void WriteExecutions(
    const std::string& name,
    const db::Database& db,
    StacksMap* stacks,
    base::JsonWriter* writer);

}  // namespace report
}  // namespace tibee

#endif // _TIBEE_REPORT_WRITEEXECUTIONS_HPP
