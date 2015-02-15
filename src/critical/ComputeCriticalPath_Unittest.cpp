/* Copyright (c) 201 Francois Doray <francois.pierre-doray@polymtl.ca>
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
#include "gtest/gtest.h"

#include <iostream>

#include "critical/ComputeCriticalPath.hpp"
#include "critical/CriticalGraph.hpp"

namespace tibee {
namespace critical {

TEST(ComputeCriticalPath, Simple)
{
    // 123456789012
    // =====-----==
    //    |    |
    // ---=========

    // Create the graph.
    CriticalGraph graph;

    std::vector<CriticalNode*> nodes;

    graph.SetTimestamp(1);
    nodes.push_back(graph.CreateNode(1));  // 0
    nodes.push_back(graph.CreateNode(2));  // 1

    graph.SetTimestamp(4);
    nodes.push_back(graph.CreateNode(1));  // 2
    nodes.push_back(graph.CreateNode(2));  // 3

    graph.SetTimestamp(6);
    nodes.push_back(graph.CreateNode(1));  // 4

    graph.SetTimestamp(9);
    nodes.push_back(graph.CreateNode(1));  // 5
    nodes.push_back(graph.CreateNode(2));  // 6

    graph.SetTimestamp(11);
    nodes.push_back(graph.CreateNode(1));  // 7

    graph.SetTimestamp(13);
    nodes.push_back(graph.CreateNode(1));  // 8
    nodes.push_back(graph.CreateNode(2));  // 9

    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[0], nodes[2]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[2], nodes[4]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[4], nodes[5]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[5], nodes[7]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[7], nodes[8]);

    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[1], nodes[3]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[3], nodes[6]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[6], nodes[9]);

    graph.CreateVerticalEdge(nodes[2], nodes[3]);
    graph.CreateVerticalEdge(nodes[6], nodes[5]);

    CriticalPath path;
    ComputeCriticalPath(graph, 1, 12, 1, &path);

    CriticalPath expectedPath = {
        CriticalPathSegment(1, 6, 1, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(6, 9, 2, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(9, 11, 1, CriticalEdgeType::kWaitBlocked),
        CriticalPathSegment(11, 12, 1, CriticalEdgeType::kRunUsermode),
    };

    EXPECT_EQ(expectedPath, path);
}

TEST(ComputeCriticalPath, MultipleSegments)
{
    // 1234567890123456789
    // =====-----===----==
    //    |    |      |
    // ---================

    // Create the graph.
    CriticalGraph graph;

    std::vector<CriticalNode*> nodes;

    graph.SetTimestamp(1);
    nodes.push_back(graph.CreateNode(1));  // 0
    nodes.push_back(graph.CreateNode(2));  // 1

    graph.SetTimestamp(4);
    nodes.push_back(graph.CreateNode(1));  // 2
    nodes.push_back(graph.CreateNode(2));  // 3

    graph.SetTimestamp(6);
    nodes.push_back(graph.CreateNode(1));  // 4

    graph.SetTimestamp(9);
    nodes.push_back(graph.CreateNode(1));  // 5
    nodes.push_back(graph.CreateNode(2));  // 6

    graph.SetTimestamp(11);
    nodes.push_back(graph.CreateNode(1));  // 7

    graph.SetTimestamp(14);
    nodes.push_back(graph.CreateNode(1));  // 8
    nodes.push_back(graph.CreateNode(2));  // 9

    graph.SetTimestamp(16);
    nodes.push_back(graph.CreateNode(1));  // 10
    nodes.push_back(graph.CreateNode(2));  // 11

    graph.SetTimestamp(18);
    nodes.push_back(graph.CreateNode(1));  // 12

    graph.SetTimestamp(20);
    nodes.push_back(graph.CreateNode(1));  // 13
    nodes.push_back(graph.CreateNode(2));  // 14

    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[0], nodes[2]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[2], nodes[4]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[4], nodes[5]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[5], nodes[7]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[7], nodes[8]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[8], nodes[10]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[10], nodes[12]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[12], nodes[13]);

    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[1], nodes[3]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[3], nodes[6]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[6], nodes[9]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[9], nodes[11]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[11], nodes[14]);

    graph.CreateVerticalEdge(nodes[2], nodes[3]);
    graph.CreateVerticalEdge(nodes[6], nodes[5]);
    graph.CreateVerticalEdge(nodes[11], nodes[10]);

    CriticalPath path;
    ComputeCriticalPath(graph, 1, 20, 1, &path);
    CriticalPath expectedPath = {
        CriticalPathSegment(1, 6, 1, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(6, 9, 2, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(9, 11, 1, CriticalEdgeType::kWaitBlocked),
        CriticalPathSegment(11, 14, 1, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(14, 16, 2, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(16, 18, 1, CriticalEdgeType::kWaitBlocked),
        CriticalPathSegment(18, 20, 1, CriticalEdgeType::kRunUsermode),
    };
    EXPECT_EQ(expectedPath, path);

    CriticalPath otherPath;
    ComputeCriticalPath(graph, 2, 12, 1, &otherPath);
    CriticalPath otherExpectedPath = {
        CriticalPathSegment(2, 6, 1, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(6, 9, 2, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(9, 11, 1, CriticalEdgeType::kWaitBlocked),
        CriticalPathSegment(11, 12, 1, CriticalEdgeType::kRunUsermode),
    };
    EXPECT_EQ(otherExpectedPath, otherPath);
}

TEST(ComputeCriticalPath, MultipleLevels)
{
    // 123456789012345678901
    // ====--------------===
    //   |             |
    // --====---------======
    //     |        |
    // ----====----=========
    //       |   |
    // ------===============

    // Create the graph.
    CriticalGraph graph;

    std::vector<CriticalNode*> nodes;

    graph.SetTimestamp(1);
    nodes.push_back(graph.CreateNode(1));  // 0
    nodes.push_back(graph.CreateNode(2));  // 1
    nodes.push_back(graph.CreateNode(3));  // 2
    nodes.push_back(graph.CreateNode(4));  // 3

    graph.SetTimestamp(3);
    nodes.push_back(graph.CreateNode(1));  // 4
    nodes.push_back(graph.CreateNode(2));  // 5

    graph.SetTimestamp(5);
    nodes.push_back(graph.CreateNode(1));  // 6
    nodes.push_back(graph.CreateNode(2));  // 7
    nodes.push_back(graph.CreateNode(3));  // 8

    graph.SetTimestamp(7);
    nodes.push_back(graph.CreateNode(2));  // 9
    nodes.push_back(graph.CreateNode(3));  // 10
    nodes.push_back(graph.CreateNode(4));  // 11

    graph.SetTimestamp(9);
    nodes.push_back(graph.CreateNode(3));  // 12

    graph.SetTimestamp(11);
    nodes.push_back(graph.CreateNode(3));  // 13
    nodes.push_back(graph.CreateNode(4));  // 14

    graph.SetTimestamp(13);
    nodes.push_back(graph.CreateNode(3));  // 15

    graph.SetTimestamp(14);
    nodes.push_back(graph.CreateNode(2));  // 16
    nodes.push_back(graph.CreateNode(3));  // 17

    graph.SetTimestamp(16);
    nodes.push_back(graph.CreateNode(2));  // 18

    graph.SetTimestamp(17);
    nodes.push_back(graph.CreateNode(1));  // 19
    nodes.push_back(graph.CreateNode(2));  // 20

    graph.SetTimestamp(19);
    nodes.push_back(graph.CreateNode(1));  // 21

    graph.SetTimestamp(22);
    nodes.push_back(graph.CreateNode(1));  // 22
    nodes.push_back(graph.CreateNode(2));  // 23
    nodes.push_back(graph.CreateNode(3));  // 24
    nodes.push_back(graph.CreateNode(4));  // 25

    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[0], nodes[4]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[4], nodes[6]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[6], nodes[19]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[19], nodes[21]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[21], nodes[22]);

    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[1], nodes[5]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[5], nodes[7]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[7], nodes[9]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[9], nodes[16]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[16], nodes[18]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[18], nodes[20]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[20], nodes[23]);

    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[2], nodes[8]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[8], nodes[10]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[10], nodes[12]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[12], nodes[13]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[13], nodes[15]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[15], nodes[17]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[17], nodes[24]);

    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[3], nodes[11]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[11], nodes[14]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[14], nodes[25]);

    graph.CreateVerticalEdge(nodes[4], nodes[5]);
    graph.CreateVerticalEdge(nodes[7], nodes[8]);
    graph.CreateVerticalEdge(nodes[10], nodes[11]);
    graph.CreateVerticalEdge(nodes[14], nodes[13]);
    graph.CreateVerticalEdge(nodes[17], nodes[16]);
    graph.CreateVerticalEdge(nodes[20], nodes[19]);

    CriticalPath path;
    ComputeCriticalPath(graph, 1, 22, 1, &path);

    CriticalPath expectedPath = {
        CriticalPathSegment(1, 5, 1, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(5, 7, 2, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(7, 9, 3, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(9, 11, 4, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(11, 13, 3, CriticalEdgeType::kWaitBlocked),
        CriticalPathSegment(13, 14, 3, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(14, 16, 2, CriticalEdgeType::kWaitBlocked),
        CriticalPathSegment(16, 17, 2, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(17, 19, 1, CriticalEdgeType::kWaitBlocked),
        CriticalPathSegment(19, 22, 1, CriticalEdgeType::kRunUsermode),
    };

    EXPECT_EQ(expectedPath, path);
}

TEST(ComputeCriticalPath, UnresolvedBlock)
{
    // 123456789012
    // =====-----==
    //    |    |
    // ---===-=====

    // Create the graph.
    CriticalGraph graph;

    std::vector<CriticalNode*> nodes;

    graph.SetTimestamp(1);
    nodes.push_back(graph.CreateNode(1));  // 0
    nodes.push_back(graph.CreateNode(2));  // 1

    graph.SetTimestamp(4);
    nodes.push_back(graph.CreateNode(1));  // 2
    nodes.push_back(graph.CreateNode(2));  // 3

    graph.SetTimestamp(6);
    nodes.push_back(graph.CreateNode(1));  // 4

    graph.SetTimestamp(7);
    nodes.push_back(graph.CreateNode(2));  // 5

    graph.SetTimestamp(8);
    nodes.push_back(graph.CreateNode(2));  // 6

    graph.SetTimestamp(9);
    nodes.push_back(graph.CreateNode(1));  // 7
    nodes.push_back(graph.CreateNode(2));  // 8

    graph.SetTimestamp(11);
    nodes.push_back(graph.CreateNode(1));  // 9

    graph.SetTimestamp(13);
    nodes.push_back(graph.CreateNode(1));  // 10
    nodes.push_back(graph.CreateNode(2));  // 11

    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[0], nodes[2]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[2], nodes[4]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[4], nodes[7]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[7], nodes[9]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[9], nodes[10]);

    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[1], nodes[3]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[3], nodes[5]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[5], nodes[6]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[6], nodes[8]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[8], nodes[11]);

    graph.CreateVerticalEdge(nodes[2], nodes[3]);
    graph.CreateVerticalEdge(nodes[8], nodes[7]);

    CriticalPath path;
    ComputeCriticalPath(graph, 1, 12, 1, &path);

    CriticalPath expectedPath = {
        CriticalPathSegment(1, 6, 1, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(6, 7, 2, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(7, 8, 2, CriticalEdgeType::kWaitBlocked),
        CriticalPathSegment(8, 9, 2, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(9, 11, 1, CriticalEdgeType::kWaitBlocked),
        CriticalPathSegment(11, 12, 1, CriticalEdgeType::kRunUsermode),
    };

    EXPECT_EQ(expectedPath, path);
}

TEST(ComputeCriticalPath, Stair)
{
    // 12345678901234567
    // ==------------===
    //             |
    //         =====
    //         |
    // =================

    // Create the graph.
    CriticalGraph graph;

    std::vector<CriticalNode*> nodes;

    graph.SetTimestamp(1);
    nodes.push_back(graph.CreateNode(1));  // 0
    nodes.push_back(graph.CreateNode(3));  // 1

    graph.SetTimestamp(3);
    nodes.push_back(graph.CreateNode(1));  // 2

    graph.SetTimestamp(9);
    nodes.push_back(graph.CreateNode(2));  // 3
    nodes.push_back(graph.CreateNode(3));  // 4

    graph.SetTimestamp(13);
    nodes.push_back(graph.CreateNode(1));  // 5
    nodes.push_back(graph.CreateNode(2));  // 6

    graph.SetTimestamp(15);
    nodes.push_back(graph.CreateNode(1));  // 7

    graph.SetTimestamp(17);
    nodes.push_back(graph.CreateNode(1));  // 8
    nodes.push_back(graph.CreateNode(3));  // 9

    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[0], nodes[2]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[2], nodes[5]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kWaitBlocked, nodes[5], nodes[7]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[7], nodes[8]);

    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[3], nodes[6]);

    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[1], nodes[4]);
    graph.CreateHorizontalEdge(CriticalEdgeType::kRunUsermode, nodes[4], nodes[9]);

    graph.CreateVerticalEdge(nodes[4], nodes[3]);
    graph.CreateVerticalEdge(nodes[6], nodes[5]);

    CriticalPath path;
    ComputeCriticalPath(graph, 1, 17, 1, &path);

    CriticalPath expectedPath = {
        CriticalPathSegment(1, 3, 1, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(3, 9, 3, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(9, 13, 2, CriticalEdgeType::kRunUsermode),
        CriticalPathSegment(13, 15, 1, CriticalEdgeType::kWaitBlocked),
        CriticalPathSegment(15, 17, 1, CriticalEdgeType::kRunUsermode),
    };

    EXPECT_EQ(expectedPath, path);
}

}    // namespace critical
}    // namespace tibee
