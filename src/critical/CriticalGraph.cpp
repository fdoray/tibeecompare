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

#include "critical/CriticalGraph.hpp"

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <queue>
#include <unordered_set>

#include "base/CleanContainer.hpp"
#include "base/print.hpp"

namespace tibee
{
namespace critical
{

namespace
{

using base::tbendl;
using base::tberror;

struct NodeTsComparator
{
    bool operator() (timestamp_t ts, const CriticalNode::UP& node) const
    {
        return ts < node->ts();
    }
};

}  // namespace

CriticalGraph::CriticalGraph()
    : _ts(0), _nextEdgeId(0)
{
}

CriticalGraph::~CriticalGraph()
{
}

void CriticalGraph::Cleanup(timestamp_t ts)
{
	for (auto& threadHistory : _tid_to_nodes)
	{
	    auto it = threadHistory.second->begin();
	    for (; it != threadHistory.second->end(); ++it)
	    {
	        if ((*it)->ts() >= ts)
	            break;

	        // Clean the edges connected to this node.
	        for (size_t i = 0; i < kCriticalEdgePositionCount; ++i)
	        {
	            auto edgeId = (*it)->edge(static_cast<CriticalEdgePosition>(i));
	            if (edgeId == kInvalidCriticalEdgeId)
	                continue;
	            auto& edge = GetEdge(edgeId);

	            if (i == kCriticalEdgeOutVertical)
	            {
	                const_cast<CriticalNode*>(edge.to())->set_edge(kCriticalEdgeInVertical, kInvalidCriticalEdgeId);
	            }
	            else if (i == kCriticalEdgeInVertical)
	            {
	                const_cast<CriticalNode*>(edge.from())->set_edge(kCriticalEdgeOutVertical, kInvalidCriticalEdgeId);
	            }
	            else if (i == kCriticalEdgeOutHorizontal)
	            {
	                const_cast<CriticalNode*>(edge.to())->set_edge(kCriticalEdgeInHorizontal, kInvalidCriticalEdgeId);
	            }
	            else
	            {
	                assert(i == kCriticalEdgeInHorizontal);
	                const_cast<CriticalNode*>(edge.from())->set_edge(kCriticalEdgeOutHorizontal, kInvalidCriticalEdgeId);
	            }

	            _edges.erase(edgeId);
	        }
	    }

	    // Clean the nodes.
	    OrderedNodes tmpThreadHistory;
	    tmpThreadHistory.reserve(std::distance(it, threadHistory.second->end()));
	    for (; it != threadHistory.second->end(); ++it)
	        tmpThreadHistory.push_back(std::move(*it));

	    threadHistory.second->swap(tmpThreadHistory);
	}
}

CriticalNode* CriticalGraph::CreateNode(uint32_t tid)
{
    // Create node.
    CriticalNode::UP node(new CriticalNode(_ts, tid));
    CriticalNode* node_ptr = node.get();

    // Keep track of nodes per thread.
    auto look = _tid_to_nodes.find(tid);
    if (look == _tid_to_nodes.end())
    {
        auto res = _tid_to_nodes.insert(
            TidToNodesMap::value_type {
                tid, std::unique_ptr<OrderedNodes>(new OrderedNodes)});
        assert(res.second == true);
        look = res.first;
    }
    look->second->push_back(std::move(node));

    return node_ptr;
}

const CriticalNode* CriticalGraph::GetNodeIntersecting(timestamp_t ts, thread_t tid) const
{
    auto thread_nodes_it = _tid_to_nodes.find(tid);
    if (thread_nodes_it == _tid_to_nodes.end())
        return nullptr;
    const auto& thread_nodes = thread_nodes_it->second;
    NodeTsComparator comparator;
    auto node_it = std::upper_bound(thread_nodes->begin(), thread_nodes->end(), ts, comparator);

    --node_it;

    if (node_it == thread_nodes->end() || node_it == (thread_nodes->begin() - 1))
        return nullptr;

    auto edgeId = (*node_it)->edge(kCriticalEdgeOutHorizontal);
    if (edgeId == kInvalidCriticalEdgeId)
        return (*node_it)->ts() == ts ? (node_it->get()) : nullptr;
    if (GetEdge(edgeId).to()->ts() < ts)
        return nullptr;

    return node_it->get();
}

const CriticalNode* CriticalGraph::GetNodeStartingAfter(timestamp_t ts, thread_t tid) const
{
    auto thread_nodes_it = _tid_to_nodes.find(tid);
    if (thread_nodes_it == _tid_to_nodes.end() || thread_nodes_it->second->empty()) {
        base::tberror() << "Querying node on thread that doesn't exist." << base::tbendl();
        return nullptr;
    }
    const auto& thread_nodes = thread_nodes_it->second;
    NodeTsComparator comparator;
    auto node_it = std::upper_bound(thread_nodes->begin(), thread_nodes->end(), ts, comparator);

    if (node_it == thread_nodes->end()) {
      base::tberror() << "First node on thread " << tid << ": " << thread_nodes->front()->ts() << base::tbendl();
      base::tberror() << "Last node on thread " << tid << ": " << thread_nodes->back()->ts() << base::tbendl();
      return nullptr;
    }

    return node_it->get();
}

CriticalNode* CriticalGraph::GetLastNodeForThread(uint32_t tid)
{
    auto thread_nodes_it = _tid_to_nodes.find(tid);
    if (thread_nodes_it == _tid_to_nodes.end() || thread_nodes_it->second->empty())
        return nullptr;
    return thread_nodes_it->second->back().get();
}

CriticalEdgeId CriticalGraph::CreateHorizontalEdge(
    CriticalEdgeType type,
    CriticalNode* from,
    CriticalNode* to)
{
    CriticalEdgeId id = _nextEdgeId;
    ++_nextEdgeId;
    _edges.insert(EdgesMap::value_type {id, CriticalEdge(type, from, to)});
    from->set_edge(kCriticalEdgeOutHorizontal, id);
    to->set_edge(kCriticalEdgeInHorizontal, id);
    return id;
}

CriticalEdgeId CriticalGraph::CreateVerticalEdge(
    CriticalNode* from,
    CriticalNode* to)
{
    CriticalEdgeId id = _nextEdgeId;
    ++_nextEdgeId;
    _edges.insert(EdgesMap::value_type {id, CriticalEdge(kVertical, from, to)});
    from->set_edge(kCriticalEdgeOutVertical, id);
    to->set_edge(kCriticalEdgeInVertical, id);
    return id;
}

}  // namespace critical
}  // namespace tibee

