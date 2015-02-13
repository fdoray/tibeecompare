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
#ifndef TIBEE_CRITICAL_CRITICALGRAPH_HPP_
#define TIBEE_CRITICAL_CRITICALGRAPH_HPP_

#include <boost/noncopyable.hpp>
#include <unordered_map>
#include <vector>

#include "critical/CriticalEdge.hpp"
#include "critical/CriticalNode.hpp"

namespace tibee
{
namespace critical
{

/**
 * Critical node.
 *
 * @author Francois Doray
 */
class CriticalGraph :
    boost::noncopyable
{
public:
    typedef std::vector<CriticalNode*> OrderedNodes;
    typedef std::unordered_map<uint32_t, std::unique_ptr<OrderedNodes>> TidToNodesMap;

    CriticalGraph();
    ~CriticalGraph();

    // Create a node.
    // The node is not linked to any other node.
    CriticalNode* CreateNode(timestamp_t ts, uint32_t tid);

    // Get a node which has an horizontal out edge that overlaps |ts|.
    const CriticalNode* GetNodeIntersecting(timestamp_t ts, uint32_t tid) const;

    // Create an horizontal edge.
    CriticalEdgeId CreateHorizontalEdge(
        CriticalEdgeType type,
        CriticalNode* from,
        CriticalNode* to);

    // Create a vertical edge.
    CriticalEdgeId CreateVerticalEdge(
        CriticalNode* from,
        CriticalNode* to);

    // Get an edge by id.
    const CriticalEdge& GetEdge(CriticalEdgeId id) const {
        return _edges[id];
    }

private:
    // Timestamp of the last node created.
    timestamp_t _last_ts;

    // Nodes.
    std::vector<CriticalNode::UP> _nodes;

    // Nodes, organized by tid and timestamp.
    TidToNodesMap _tid_to_nodes;

    // Edges.
    std::vector<CriticalEdge> _edges;
};

}
}

#endif  // TIBEE_SRC_CRITICAL_CRITICALGRAPH_HPP_
