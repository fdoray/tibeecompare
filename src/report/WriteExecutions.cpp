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

#include "base/JsonWriter.hpp"

namespace tibee
{
namespace report
{

namespace
{

void WriteExecution(
    const execution::Execution& execution,
    base::JsonWriter* writer,
    std::set<execution::StackId>* stacks)
{
    writer->BeginDict();

    // Write metrics.
    for (auto it = execution.metrics_begin();
         it != execution.metrics_end();
         ++it)
    {
        std::string key(1, static_cast<char>(it->first) + 'a');
        writer->KeyValue(key, it->second);
    }

    // Write samples.
    writer->KeyDictValue("samples");

    for (auto it = execution.samples_begin();
         it != execution.samples_end();
         ++it)
    {
        writer->KeyValue(std::to_string(it->first), it->second);
        stacks->insert(it->first);
    }

    writer->EndDict();

    writer->EndDict();
}

}  // namespace

void WriteExecutions(
    const std::string& name,
    const db::Database& db,
    std::set<execution::StackId>* stacks)
{
    namespace pl = std::placeholders;

    base::JsonWriter writer;
    writer.Open(name + "-executions.json");

    writer.BeginArray();

    db.EnumerateExecutions(
        name,
        std::bind(&WriteExecution, pl::_1, &writer, stacks));

    writer.EndArray();
}

}  // namespace report
}  // namespace tibee
