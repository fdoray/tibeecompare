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

#include <iostream>
#include <vector>

#include "base/JsonWriter.hpp"

namespace tibee
{
namespace report
{

void WriteStacks(
    const std::string& name,
    const db::Database& db,
    const std::set<execution::StackId>& stacks)
{
    // Open Json file.
    base::JsonWriter writer;
    writer.Open(name + "-stacks.json");

    writer.BeginDict();

    std::vector<execution::StackId> stacksVec(
        stacks.begin(), stacks.end());
    std::set<execution::StackId> stacksDone;

    for (size_t i = 0; i < stacksVec.size(); ++i)
    {
        auto stackId = stacksVec[i];
        if (stacksDone.find(stackId) != stacksDone.end())
            continue;
        stacksDone.insert(stackId);

        // Read the stack and function name from database.
        auto stack = db.GetStack(stackId);
        auto functionName = db.GetFunctionName(stack.function());

        writer.KeyDictValue(std::to_string(stackId));

        writer.KeyValue("b", stack.bottom());
        writer.KeyValue("f", functionName);
        writer.EndDict();

        if (stack.bottom() != execution::kEmptyStackId)
            stacksVec.push_back(stack.bottom());
    }

    writer.EndDict();
}

}  // namespace report
}  // namespace tibee
