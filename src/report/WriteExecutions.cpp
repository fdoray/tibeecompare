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
#include "report/WriteExecutions.hpp"

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "base/CompareConstants.hpp"
#include "base/JsonWriter.hpp"

namespace tibee
{
namespace report
{

namespace
{

const size_t kNumDesiredExecutions = 50000;

typedef std::vector<stacks::StackId> StacksVector;
typedef std::unordered_map<stacks::StackId, StacksVector> ReverseStacksMap;

void EnsureStacksInMap(
    const execution::Execution& execution,
    const db::Database& db,
    StacksMap* stacks,
    ReverseStacksMap* reverseStacks)
{
    std::vector<stacks::StackId> stacksToDo;
    for (auto it = execution.samples_begin();
         it != execution.samples_end();
         ++it)
    {
        stacksToDo.push_back(it->first);
    }

    for (size_t i = 0; i < stacksToDo.size(); ++i)
    {
        stacks::StackId stackId = stacksToDo[i];
        if (stackId == stacks::kEmptyStackId)
            continue;

        // Check whether the stack is already in the maps.
        auto look = stacks->find(stackId);
        if (look != stacks->end())
            continue;

        // If the stack is not already in the maps, query it from the database.
        auto stack = db.GetStack(stackId);

        // Add the stack to |stacks|.
        (*stacks)[stackId] = stack;

        // Add |stackId| to the list of children of the parent stack in
        // |reverseStacks|.
        (*reverseStacks)[stack.bottom()].push_back(stackId);

        // Add the parent stack to |stacksToDo|.
        stacksToDo.push_back(stack.bottom());
    }
}

uint64_t WriteSamples(
    stacks::StackId stackId,
    const execution::Execution& execution,
    base::JsonWriter* writer,
    ReverseStacksMap* reverseStacks)
{
    // Compute the counts for the children of this stack.
    uint64_t childrenCount = 0;

    auto look = reverseStacks->find(stackId);
    if (look != reverseStacks->end())
    {
        const auto& children = look->second;
        for (stacks::StackId childStackId : children)
        {
            childrenCount += WriteSamples(
                childStackId, execution, writer, reverseStacks);
        }
    }

    // Compute the count for this stack, including its children.
    uint64_t inclusiveCount = execution.GetSample(stackId) + childrenCount;
    uint64_t inclusiveCountMicroseconds = inclusiveCount / 1000;

    // Write the count to the JSon file.
    if (stackId != stacks::kEmptyStackId && inclusiveCountMicroseconds != 0) {
        writer->KeyValue(std::to_string(stackId), inclusiveCountMicroseconds);
    }

    return inclusiveCount;
}

void WriteExecution(
    const execution::Execution& execution,
    const db::Database& db,
    base::JsonWriter* writer,
    StacksMap* stacks,
    ReverseStacksMap* reverseStacks)
{
    writer->BeginDict();

    // Write metrics.
    for (auto it = execution.metrics_begin();
         it != execution.metrics_end();
         ++it)
    {
        // Timing metric.
        if (it->first < kPerformanceCounterFirstMetricId)
        {
            uint64_t microsecondsValue = it->second / 1000;
            std::string key(1, static_cast<char>(it->first) + 'a');
            writer->KeyValue(key, microsecondsValue);
        }
        else
        {
            std::string key = std::string("p") +
                std::to_string(it->first - kPerformanceCounterFirstMetricId);
            writer->KeyValue(key, it->second);
        }
    }

    // Make sure that all stacks of this execution are in |stacks| and
    // |reverseStacks|.
    EnsureStacksInMap(execution, db, stacks, reverseStacks);

    // Write samples.
    writer->KeyDictValue("samples");
    WriteSamples(stacks::kEmptyStackId, execution, writer, reverseStacks);
    writer->EndDict();

    writer->EndDict();
}

}  // namespace

void WriteExecutions(
    const std::string& name,
    const db::Database& db,
    StacksMap* stacks,
    base::JsonWriter* writer)
{
    namespace pl = std::placeholders;

    writer->KeyArrayValue("executions");

    ReverseStacksMap reverseStacks;

    db.EnumerateExecutions(
        name, kNumDesiredExecutions,
        std::bind(&WriteExecution, pl::_1, std::ref(db), writer,
                  stacks, &reverseStacks));

    writer->EndArray();
}

}  // namespace report
}  // namespace tibee
