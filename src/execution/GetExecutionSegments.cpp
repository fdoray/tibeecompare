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
#include "execution/GetExecutionSegments.hpp"

#include "execution/GenerateExecutionGraph.hpp"
#include "execution/GetExecutionSegmentsFollow.hpp"

namespace tibee
{
namespace execution
{

bool GetExecutionSegments(
    const Execution& execution,
    const Stacks& stacks,
    std::vector<Link>* links,
    ExecutionSegments* executionSegments)
{
    if (execution.threads().size() <= 1)
    {
        // Only one thread specified: we follow links to find more.

        VerticesPerThread vertices;
        Vertex* startVertex = nullptr;
        Vertex* endVertex = nullptr;
        
        if (!GenerateExecutionGraph(
            execution,
            stacks,
            &vertices,
            &startVertex,
            &endVertex)) {
            return false;
        }
        GetExecutionSegmentsFollow(
            vertices,
            links,
            executionSegments);
    }
    else
    {
        // Many threads specified: use them.

        for (const auto& thread : execution.threads())
        {
            ExecutionSegment executionSegment;
            executionSegment.set_thread(thread);
            executionSegment.set_startTs(execution.startTs());
            executionSegment.set_endTs(execution.endTs());
            executionSegments->push_back(executionSegment);
        }
    }
    return true;
}

}  // namespace execution
}  // namespace tibee
