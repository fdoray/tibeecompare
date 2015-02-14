/* Copyright (c) 2014 Francois Doray <francois.pierre-doray@polymtl.ca>
 *
 * This file is part of tigerbeetle.
 *
 * tigerbeetle is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tigerbeetle is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tigerbeetle.    If not, see <http://www.gnu.org/licenses/>.
 */
#include <algorithm>

#include "gtest/gtest.h"
#include "critical/CriticalGraph.hpp"

namespace tibee {
namespace critical {

TEST(CriticalGraph, GetNodeIntersecting)
{
    // Create the graph.
    CriticalGraph graph;

    std::vector<CriticalNode*> nodes;

    graph.SetTimestamp(10);
    nodes.push_back(graph.CreateNode(1)); // 0
    graph.SetTimestamp(12);
    nodes.push_back(graph.CreateNode(1)); // 1
    graph.SetTimestamp(14);
    nodes.push_back(graph.CreateNode(1)); // 2
    graph.SetTimestamp(16);
    nodes.push_back(graph.CreateNode(1)); // 3

    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[0], nodes[1]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[1], nodes[2]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[2], nodes[3]);

    EXPECT_EQ(nullptr, graph.GetNodeIntersecting(5, 1));
    EXPECT_EQ(nodes[0], graph.GetNodeIntersecting(10, 1));
    EXPECT_EQ(nodes[0], graph.GetNodeIntersecting(11, 1));
    EXPECT_EQ(nodes[1], graph.GetNodeIntersecting(12, 1));
    EXPECT_EQ(nodes[1], graph.GetNodeIntersecting(13, 1));
    EXPECT_EQ(nodes[2], graph.GetNodeIntersecting(14, 1));
    EXPECT_EQ(nodes[2], graph.GetNodeIntersecting(15, 1));
    EXPECT_EQ(nodes[3], graph.GetNodeIntersecting(16, 1));
    EXPECT_EQ(nodes[3], graph.GetNodeIntersecting(17, 1));
}

}    // namespace critical
}    // namespace tibee
