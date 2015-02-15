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
#include <queue>
#include <unordered_set>

#include "base/print.hpp"

namespace tibee
{
namespace critical
{

namespace
{

using base::tbendl;
using base::tberror;

struct NodeTsComparatorReverse
{
    bool operator() (timestamp_t ts, const CriticalNode* node) const
    {
        return ts < node->ts();
    }
};

}  // namespace

CriticalGraph::CriticalGraph()
    : _ts(0)
{
}

CriticalGraph::~CriticalGraph()
{
}

CriticalNode* CriticalGraph::CreateNode(uint32_t tid)
{
    // Create node.
    CriticalNode::UP node(new CriticalNode(_ts, tid));
    auto node_ptr = node.get();
    _nodes.push_back(std::move(node));

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
    look->second->push_back(node_ptr);

    return node_ptr;
}

const CriticalNode* CriticalGraph::GetNodeIntersecting(timestamp_t ts, thread_t tid) const
{
    auto thread_nodes_it = _tid_to_nodes.find(tid);
    if (thread_nodes_it == _tid_to_nodes.end())
        return nullptr;
    const auto& thread_nodes = thread_nodes_it->second;
    NodeTsComparatorReverse comparator;
    auto node_it = std::upper_bound(thread_nodes->begin(), thread_nodes->end(), ts, comparator);

    --node_it;

    if (node_it == thread_nodes->end() || node_it == (thread_nodes->begin() - 1))
        return nullptr;

    auto edgeId = (*node_it)->edge(kCriticalEdgeOutHorizontal);
    if (edgeId == kInvalidCriticalEdgeId)
        return (*node_it)->ts() == ts ? (*node_it) : nullptr;
    if (GetEdge(edgeId).to()->ts() < ts)
        return nullptr;

    return *node_it;
}

const CriticalNode* CriticalGraph::GetNodeStartingAfter(timestamp_t ts, thread_t tid) const
{
    auto thread_nodes_it = _tid_to_nodes.find(tid);
    if (thread_nodes_it == _tid_to_nodes.end())
        return nullptr;
    const auto& thread_nodes = thread_nodes_it->second;
    NodeTsComparatorReverse comparator;
    auto node_it = std::upper_bound(thread_nodes->begin(), thread_nodes->end(), ts, comparator);

    if (node_it == thread_nodes->end())
      return nullptr;

    return *node_it;
}

CriticalNode* CriticalGraph::GetLastNodeForThread(uint32_t tid)
{
    auto thread_nodes_it = _tid_to_nodes.find(tid);
    if (thread_nodes_it == _tid_to_nodes.end() || thread_nodes_it->second->empty())
        return nullptr;
    return thread_nodes_it->second->back();
}

CriticalEdgeId CriticalGraph::CreateHorizontalEdge(
    CriticalEdgeType type,
    CriticalNode* from,
    CriticalNode* to)
{
    CriticalEdgeId id = _edges.size();
    _edges.push_back(CriticalEdge(type, from, to));
    from->set_edge(kCriticalEdgeOutHorizontal, id);
    to->set_edge(kCriticalEdgeInHorizontal, id);
    return id;
}

CriticalEdgeId CriticalGraph::CreateVerticalEdge(
    CriticalNode* from,
    CriticalNode* to)
{
    CriticalEdgeId id = _edges.size();
    _edges.push_back(CriticalEdge(CriticalEdgeType::kVertical, from, to));
    from->set_edge(kCriticalEdgeOutVertical, id);
    to->set_edge(kCriticalEdgeInVertical, id);
    return id;
}

}  // namespace critical
}  // namespace tibee

