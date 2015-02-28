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

#include "base/print.hpp"
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
    typedef std::vector<CriticalNode::UP> OrderedNodes;
    typedef std::unordered_map<uint32_t, std::unique_ptr<OrderedNodes>> TidToNodesMap;

    CriticalGraph();
    ~CriticalGraph();

    // Set the current timestamp.
    void SetTimestamp(timestamp_t ts) { _ts = ts; }

    // Removes everything that is before the specified timestamp.
    void Cleanup(timestamp_t ts);

    // Create a node.
    // The node is not linked to any other node.
    CriticalNode* CreateNode(uint32_t tid);

    // Get a node which has an horizontal out edge that overlaps |ts|.
    const CriticalNode* GetNodeIntersecting(timestamp_t ts, thread_t tid) const;

    // Get a node that starts after the specified timestamp.
    const CriticalNode* GetNodeStartingAfter(timestamp_t ts, thread_t tid) const;

    // Get the last created node for the given thread.
    CriticalNode* GetLastNodeForThread(uint32_t tid);

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
        auto look = _edges.find(id);
        if (look == _edges.end())
        {
            base::tberror() << "Request for invalid edge " << id << "."  << base::tbendl();
            exit(1);
        }
        return look->second;
    }

    // Maximum tid value.
    static const thread_t kMaxTid = 32768;

    // Special thread for network operations.
    static const thread_t kNetworkThread = kMaxTid + 1;

private:
    // Timestamp.
    timestamp_t _ts;

    // Nodes, organized by tid and timestamp.
    TidToNodesMap _tid_to_nodes;

    // Edges.
    typedef std::unordered_map<CriticalEdgeId, CriticalEdge> EdgesMap;
    EdgesMap _edges;

    // Id of the next edge.
    CriticalEdgeId _nextEdgeId;
};

}  // namespace critical
}  // namespace tibee

#endif  // TIBEE_SRC_CRITICAL_CRITICALGRAPH_HPP_
