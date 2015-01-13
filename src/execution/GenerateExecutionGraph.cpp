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
#include "execution/GenerateExecutionGraph.hpp"

#include <iostream>
#include <queue>

#include "base/print.hpp"

namespace tibee
{
namespace execution
{

namespace
{

void CreateVertices(
    const Link& link,
    VerticesPerThread* verticesPerThread)
{
    if (link.sourceThread() == link.targetThread())
    {
        return;
    }

    auto& sourceThread = (*verticesPerThread)[link.sourceThread()];
    auto& targetThread = (*verticesPerThread)[link.targetThread()];

    Vertex::UP sourceVertex(new Vertex(link.sourceThread(), link.sourceTs()));
    Vertex::UP targetVertex(new Vertex(link.targetThread(), link.targetTs()));

    if (!sourceThread.empty())
    {
        auto* sourcePrevVertex = sourceThread.back().get();
        sourcePrevVertex->hout = sourceVertex.get();
        sourceVertex->hin = sourcePrevVertex;
    }

    if (!targetThread.empty())
    {
        auto* targetPrevVertex = targetThread.back().get();
        targetPrevVertex->hout = targetVertex.get();
        targetVertex->hin = targetPrevVertex;
    }

    sourceVertex->vout = targetVertex.get();
    targetVertex->vin = sourceVertex.get();

    sourceThread.push_back(std::move(sourceVertex));
    targetThread.push_back(std::move(targetVertex));
}

bool GenerateExecutionGraphInternal(
    const Execution& execution,
    const Stacks& stacks,
    VerticesPerThread* verticesPerThread,
    Vertex** start,
    Vertex** end)
{
    namespace pl = std::placeholders;

    // Add initial time point.
    Vertex::UP startUP(new Vertex(execution.startThread(), execution.startTs()));
    *start = startUP.get();
    (*verticesPerThread)[execution.startThread()].push_back(std::move(startUP));

    // Follow links to create time points.
    stacks.EnumerateLinks(
        containers::Interval(execution.startTs(), execution.endTs()),
        std::bind(&CreateVertices, pl::_1, verticesPerThread));

    // Add final time point.
    auto& prevThread = (*verticesPerThread)[execution.endThread()];
    if (prevThread.empty())
    {
        base::tberror() << "No previous time point on thread " << execution.endThread() << "." << base::tbendl();
        return false;
    }
    auto* prevVertex = prevThread.back().get();
    Vertex::UP endUP(new Vertex(execution.endThread(), execution.endTs()));
    *end = endUP.get();
    prevVertex->hout = endUP.get();
    endUP->hin = prevVertex;
    (*verticesPerThread)[execution.endThread()].push_back(std::move(endUP));

    return true;
}

void AssignLevels(Vertex* start, Vertex* end)
{
    std::queue<Vertex*> queue;
    std::queue<Vertex*> partialQueue;
    queue.push(start);

    bool reachedEnd = false;

    while (!queue.empty())
    {
        Vertex* point = queue.front();
        queue.pop();

        if (point->level == kInvalidLevel)
        {
            uint32_t hlevel = kInvalidLevel;
            uint32_t vlevel = kInvalidLevel;

            if (point->hin != nullptr)
                hlevel = point->hin->level;
            if (point->vin != nullptr)
                vlevel = point->vin->level;

            if (hlevel == kInvalidLevel && vlevel == kInvalidLevel)
                point->level = 0;
            else if (hlevel == kInvalidLevel || vlevel < hlevel)
                point->level = vlevel + 1;
            else
                point->level = hlevel;

            if (point->hout != nullptr)
            {
                if (point->hout->vin == nullptr || point->hout->vin->level != kInvalidLevel)
                {
                    queue.push(point->hout);
                }
                else
                {
                    partialQueue.push(point->hout);
                }
            }

            if (point->vout != nullptr)
            {
                queue.push(point->vout);
            }

            if (point == end)
            {
                reachedEnd = true;
            }
        }

        // Avoid being blocked before reaching the end...
        if (!reachedEnd && queue.empty())
        {
            queue.push(partialQueue.front());
            partialQueue.pop();
        }
    }
}

}  // namespace

bool GenerateExecutionGraph(
    const Execution& execution,
    const Stacks& stacks,
    VerticesPerThread* verticesPerThread,
    Vertex** start,
    Vertex** end)
{
    if (!GenerateExecutionGraphInternal(execution, stacks, verticesPerThread, start, end))
        return false;

    AssignLevels(*start, *end);

    return true;
}

}  // namespace execution
}  // namespace tibee
