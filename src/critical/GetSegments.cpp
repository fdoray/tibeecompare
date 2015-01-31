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
#include "critical/GetSegments.hpp"

#include "critical/GenerateExecutionGraph.hpp"
#include "critical/Vertex.hpp"

namespace tibee
{
namespace critical
{

namespace
{

void GetSegmentsInternal(
    const VerticesPerThread& verticesPerThread,
    Segments* segments)
{
    for (const auto& threadVertices : verticesPerThread)
    {
        thread_t thread = threadVertices.first;
        const auto& vertices = threadVertices.second;

        timestamp_t currentSegmentStartTs = kInvalidTs;
        size_t index = 0;

        for (const auto& vertex : vertices)
        {
            if (currentSegmentStartTs == kInvalidTs)
            {
                if (vertex->level != kInvalidLevel)
                    currentSegmentStartTs = vertex->ts;
            }
            else
            {
                if ((vertex->hout != nullptr && (
                      (vertex->hout->vin != nullptr &&
                       vertex->hout->vin->level < vertex->hout->level) ||
                      vertex->hout->level == kInvalidLevel)) ||
                     index == vertices.size() - 1)
                {
                    Segment segment;
                    segment.set_thread(thread);
                    segment.set_startTs(currentSegmentStartTs);
                    segment.set_endTs(vertex->ts);
                    segments->push_back(segment);

                    currentSegmentStartTs = kInvalidTs;
                }
            }

            ++index;
        }
    }
}

}  // namespace

bool GetSegments(
    const execution::Execution& execution,
    const execution::LinksBuilder& links,
    Segments* segments)
{
    VerticesPerThread vertices;
    Vertex* startVertex = nullptr;
    Vertex* endVertex = nullptr;
        
    if (!GenerateExecutionGraph(
        execution,
        links,
        &vertices,
        &startVertex,
        &endVertex))
    {
        return false;
    }
    GetSegmentsInternal(
        vertices,
        segments);
    return true;
}

}  // namespace critical
}  // namespace tibee
