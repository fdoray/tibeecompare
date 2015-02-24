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
#include "report/WriteStacks.hpp"

#include <vector>

namespace tibee
{
namespace report
{

void WriteStacks(
    const db::Database& db,
    const std::map<stacks::StackId, stacks::Stack>& stacks,
    base::JsonWriter* writer)
{
    writer->KeyDictValue("stacks");

    for (const auto& stack : stacks)
    {
        writer->KeyDictValue(std::to_string(stack.first));

        writer->KeyValue("b", stack.second.bottom());
        writer->KeyValue("f", db.GetFunctionName(stack.second.function()));
        writer->EndDict();
    }

    writer->EndDict();
}

}  // namespace report
}  // namespace tibee
