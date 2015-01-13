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

#include <iostream>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "base/Inserter.hpp"
#include "base/print.hpp"

namespace tibee
{
namespace execution
{

namespace
{

const timestamp_t kInvalidTs = -1;
const uint32_t kInvalidLevel = -1;

struct Vertex
{
    typedef std::unique_ptr<Vertex> UP;

    Vertex(thread_t thread, timestamp_t ts)
        : thread(thread), ts(ts),
          hout(nullptr), hin(nullptr),
          vout(nullptr), vin(nullptr),
          level(kInvalidLevel) {}

    thread_t thread;
    timestamp_t ts;

    Vertex* hout;
    Vertex* hin;
    Vertex* vout;
    Vertex* vin;

    uint32_t level;
};

typedef std::vector<std::unique_ptr<Vertex>> Vertices;
typedef std::unordered_map<thread_t, Vertices> VerticesPerThread;

void CreateVerticesInternal(
    const Link& link,
    VerticesPerThread* verticesPerThread)
{
    if (link.sourceThread() == link.targetThread())
    {
        base::tberror() << "Cannot have a link with the same source and "
                        << "target thread." << base::tbendl();
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

bool CreateVertices(
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
        std::bind(&CreateVerticesInternal, pl::_1, verticesPerThread));

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

void GetExecutionSegmentsInternal(
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

}  // namespace

void GetExecutionSegments(
    const Execution& execution,
    const Stacks& stacks,
    std::vector<Link>* links,
    ExecutionSegments* executionSegments)
{
    // Create graph.
    VerticesPerThread verticesPerThread;
    Vertex* start = nullptr;
    Vertex* end = nullptr;
    if (!CreateVertices(execution, stacks, &verticesPerThread, &start, &end))
        return;

    // Assign levels to the nodes of the graph.
    AssignLevels(start, end);

    // On each thread, find continuous segments without a
    // vertical-in link from a smaller level.
    GetExecutionSegmentsInternal(verticesPerThread, links, executionSegments);
}

}  // namespace execution
}  // namespace tibee
