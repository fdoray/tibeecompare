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
#include "critical/ComputeCriticalPath.hpp"

#include <algorithm>
#include <assert.h>

namespace tibee
{
namespace critical
{

namespace
{

void InsertCriticalPathSegment(
    const CriticalPathSegment& segment,
    CriticalPath* path)
{
    assert(path != nullptr);

    if (!path->empty() &&
        path->back().type() == segment.type() &&
        path->back().tid() == segment.tid())
    {
        path->back().set_endTs(segment.endTs());
    }
    else
    {
        path->push_back(segment);
    }
}

void ComputeCriticalPathRecursive(
    const CriticalGraph& graph,
    timestamp_t startTs,
    timestamp_t endTs,
    thread_t tid,
    CriticalPath* path);

void ResolvedBlockedEdge(
    const CriticalGraph& graph,
    const CriticalEdge& edge,
    timestamp_t startTs,
    timestamp_t endTs,
    CriticalPath* path)
{
    assert(edge.type() == kWaitBlocked || edge.type() == kNetwork);
    auto* toNode = edge.to();
    assert(toNode != nullptr);
    auto wakeUpEdgeId = toNode->edge(kCriticalEdgeInVertical);

    // If there is no wake-up edge, simply insert the blocked edge in the
    // critical path.
    if (wakeUpEdgeId == kInvalidCriticalEdgeId)
    {
        InsertCriticalPathSegment(CriticalPathSegment(
            startTs, endTs, toNode->tid(), edge.type()), path);
        return;
    }

    // There is a wake-up edge.
    auto& wakeUpEdge = graph.GetEdge(wakeUpEdgeId);
    thread_t sourceThread = wakeUpEdge.from()->tid();

    // Special case for the network thread.
    if (sourceThread == critical::CriticalGraph::kNetworkThread)
    {
        // Get the edge spent on the network thread.
        auto* networkEndNode = wakeUpEdge.from();
        auto& networkEdge = graph.GetEdge(
            networkEndNode->edge(kCriticalEdgeInHorizontal));
        auto* networkStartNode = networkEdge.from();

        // Compute the critical path on the thread that sent network data.
        auto networkWakeUpEdgeId = networkStartNode->edge(kCriticalEdgeInVertical);
        auto& networkWakeUpEdge = graph.GetEdge(networkWakeUpEdgeId);
        thread_t networkSourceThread = networkWakeUpEdge.from()->tid();

        ComputeCriticalPathRecursive(
            graph, startTs, networkStartNode->ts(), networkSourceThread, path);

        // Add a segment for the time spent waiting on the network thread.
        InsertCriticalPathSegment(CriticalPathSegment(
            networkStartNode->ts(), networkEndNode->ts(),
            networkStartNode->tid(), networkEdge.type()), path);
    }
    else
    {
        // Compute the critical path on the source thread.
        ComputeCriticalPathRecursive(graph, startTs, endTs, sourceThread, path);
    }
}

void ComputeCriticalPathRecursive(
    const CriticalGraph& graph,
    timestamp_t startTs,
    timestamp_t endTs,
    thread_t tid,
    CriticalPath* path)
{
    if (endTs <= startTs)
        return;

    // Find a node on thread |tid| that is at timestamp |startTs| or that has
    // an outgoing edge that overlaps this timestamp.
    auto* node = graph.GetNodeIntersecting(startTs, tid);

    // If no node was found for the thread at timestamp |startTs|, it must be
    // because the thread didn't exist at that time.
    if (node == nullptr)
    {
        // Look for the first node of the thread that has a timestamp > |startTs|
        // and that has an input edge.
        node = graph.GetNodeStartingAfter(startTs, tid);
        while (node != nullptr && node->ts() <= endTs)
        {
            auto wakeUpEdgeId = node->edge(kCriticalEdgeInVertical);
            if (wakeUpEdgeId != kInvalidCriticalEdgeId)
            {
                // We found a thread that sent a wake-up to this thread: compute
                // the critical path on this thread.
                auto& wakeUpEdge = graph.GetEdge(wakeUpEdgeId);
                ComputeCriticalPathRecursive(
                    graph, startTs, node->ts(), wakeUpEdge.from()->tid(), path);
                break;
            }
        }
    }

    // Traverse all nodes of the thread |tid| that are between |startTs|
    // and |endTs|.
    while (node != nullptr &&
           node->ts() <= endTs &&
           node->edge(kCriticalEdgeOutHorizontal) != kInvalidCriticalEdgeId)
    {
        auto edgeId = node->edge(kCriticalEdgeOutHorizontal);
        assert(edgeId != kInvalidCriticalEdgeId);
        auto& edge = graph.GetEdge(edgeId);
        auto* nextNode = edge.to();
        assert(nextNode != nullptr);

        timestamp_t edgeStartTs = std::max(startTs, node->ts());
        timestamp_t edgeEndTs = std::min(endTs, nextNode->ts());

        if (edge.type() == kWaitBlocked || edge.type() == kNetwork)
        {
            ResolvedBlockedEdge(graph, edge, edgeStartTs, edgeEndTs, path);
        }
        else
        {
            if (edgeStartTs != edgeEndTs)
            {
                InsertCriticalPathSegment(CriticalPathSegment(
                    edgeStartTs, edgeEndTs, tid, edge.type()), path);
            }
        }

        node = nextNode;
    }
}

}  // namespace

void ComputeCriticalPath(
    const CriticalGraph& graph,
    timestamp_t startTs,
    timestamp_t endTs,
    thread_t tid,
    CriticalPath* path)
{
    assert(path != nullptr);
    assert(path->empty());
    ComputeCriticalPathRecursive(graph, startTs, endTs, tid, path);
}

}  // namespace critical
}  // namespace tibee
