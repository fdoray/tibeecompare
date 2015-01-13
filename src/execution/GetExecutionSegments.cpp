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

#include "execution/Vertex.hpp"

namespace tibee
{
namespace execution
{

void GetExecutionSegments(
    const VerticesPerThread& verticesPerThread,
    std::vector<Link>* links,
    ExecutionSegments* executionSegments)
{
    for (const auto& threadVertices : verticesPerThread)
    {
        thread_t thread = threadVertices.first;
        const auto& vertices = threadVertices.second;

        timestamp_t startTs = kInvalidTs;
        size_t index = 0;

        for (const auto& vertex : vertices)
        {
            if (startTs == kInvalidTs)
            {
                if (vertex->level != kInvalidLevel)
                    startTs = vertex->ts;
            }
            else
            {
                if ((vertex->hout != nullptr && (
                      (vertex->hout->vin != nullptr &&
                       vertex->hout->vin->level < vertex->hout->level) ||
                      vertex->hout->level == kInvalidLevel)) ||
                     index == vertices.size() - 1)
                {
                    ExecutionSegment executionSegment;
                    executionSegment.set_thread(thread);
                    executionSegment.set_startTs(startTs);
                    executionSegment.set_endTs(vertex->ts);
                    executionSegments->push_back(executionSegment);

                    startTs = kInvalidTs;
                }

                if (vertex->vout != nullptr)
                {
                    links->push_back(Link(
                        vertex->thread,
                        vertex->ts,
                        vertex->vout->thread,
                        vertex->vout->ts));
                }
            }

            ++index;
        }
    }
}

}  // namespace execution
}  // namespace tibee
