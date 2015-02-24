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

    graph.CreateHorizontalEdge(kRun, nodes[0], nodes[2]);
    graph.CreateHorizontalEdge(kRun, nodes[2], nodes[4]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[4], nodes[5]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[5], nodes[7]);
    graph.CreateHorizontalEdge(kRun, nodes[7], nodes[8]);

    graph.CreateHorizontalEdge(kWaitBlocked, nodes[1], nodes[3]);
    graph.CreateHorizontalEdge(kRun, nodes[3], nodes[6]);
    graph.CreateHorizontalEdge(kRun, nodes[6], nodes[9]);

    graph.CreateVerticalEdge(nodes[2], nodes[3]);
    graph.CreateVerticalEdge(nodes[6], nodes[5]);

    CriticalPath path;
    ComputeCriticalPath(graph, 1, 12, 1, &path);

    CriticalPath expectedPath = {
        CriticalPathSegment(1, 6, 1, kRun),
        CriticalPathSegment(6, 9, 2, kRun),
        CriticalPathSegment(9, 11, 1, kWaitBlocked),
        CriticalPathSegment(11, 12, 1, kRun),
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

    graph.CreateHorizontalEdge(kRun, nodes[0], nodes[2]);
    graph.CreateHorizontalEdge(kRun, nodes[2], nodes[4]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[4], nodes[5]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[5], nodes[7]);
    graph.CreateHorizontalEdge(kRun, nodes[7], nodes[8]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[8], nodes[10]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[10], nodes[12]);
    graph.CreateHorizontalEdge(kRun, nodes[12], nodes[13]);

    graph.CreateHorizontalEdge(kWaitBlocked, nodes[1], nodes[3]);
    graph.CreateHorizontalEdge(kRun, nodes[3], nodes[6]);
    graph.CreateHorizontalEdge(kRun, nodes[6], nodes[9]);
    graph.CreateHorizontalEdge(kRun, nodes[9], nodes[11]);
    graph.CreateHorizontalEdge(kRun, nodes[11], nodes[14]);

    graph.CreateVerticalEdge(nodes[2], nodes[3]);
    graph.CreateVerticalEdge(nodes[6], nodes[5]);
    graph.CreateVerticalEdge(nodes[11], nodes[10]);

    CriticalPath path;
    ComputeCriticalPath(graph, 1, 20, 1, &path);
    CriticalPath expectedPath = {
        CriticalPathSegment(1, 6, 1, kRun),
        CriticalPathSegment(6, 9, 2, kRun),
        CriticalPathSegment(9, 11, 1, kWaitBlocked),
        CriticalPathSegment(11, 14, 1, kRun),
        CriticalPathSegment(14, 16, 2, kRun),
        CriticalPathSegment(16, 18, 1, kWaitBlocked),
        CriticalPathSegment(18, 20, 1, kRun),
    };
    EXPECT_EQ(expectedPath, path);

    CriticalPath otherPath;
    ComputeCriticalPath(graph, 2, 12, 1, &otherPath);
    CriticalPath otherExpectedPath = {
        CriticalPathSegment(2, 6, 1, kRun),
        CriticalPathSegment(6, 9, 2, kRun),
        CriticalPathSegment(9, 11, 1, kWaitBlocked),
        CriticalPathSegment(11, 12, 1, kRun),
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

    graph.CreateHorizontalEdge(kRun, nodes[0], nodes[4]);
    graph.CreateHorizontalEdge(kRun, nodes[4], nodes[6]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[6], nodes[19]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[19], nodes[21]);
    graph.CreateHorizontalEdge(kRun, nodes[21], nodes[22]);

    graph.CreateHorizontalEdge(kWaitBlocked, nodes[1], nodes[5]);
    graph.CreateHorizontalEdge(kRun, nodes[5], nodes[7]);
    graph.CreateHorizontalEdge(kRun, nodes[7], nodes[9]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[9], nodes[16]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[16], nodes[18]);
    graph.CreateHorizontalEdge(kRun, nodes[18], nodes[20]);
    graph.CreateHorizontalEdge(kRun, nodes[20], nodes[23]);

    graph.CreateHorizontalEdge(kWaitBlocked, nodes[2], nodes[8]);
    graph.CreateHorizontalEdge(kRun, nodes[8], nodes[10]);
    graph.CreateHorizontalEdge(kRun, nodes[10], nodes[12]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[12], nodes[13]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[13], nodes[15]);
    graph.CreateHorizontalEdge(kRun, nodes[15], nodes[17]);
    graph.CreateHorizontalEdge(kRun, nodes[17], nodes[24]);

    graph.CreateHorizontalEdge(kWaitBlocked, nodes[3], nodes[11]);
    graph.CreateHorizontalEdge(kRun, nodes[11], nodes[14]);
    graph.CreateHorizontalEdge(kRun, nodes[14], nodes[25]);

    graph.CreateVerticalEdge(nodes[4], nodes[5]);
    graph.CreateVerticalEdge(nodes[7], nodes[8]);
    graph.CreateVerticalEdge(nodes[10], nodes[11]);
    graph.CreateVerticalEdge(nodes[14], nodes[13]);
    graph.CreateVerticalEdge(nodes[17], nodes[16]);
    graph.CreateVerticalEdge(nodes[20], nodes[19]);

    CriticalPath path;
    ComputeCriticalPath(graph, 1, 22, 1, &path);

    CriticalPath expectedPath = {
        CriticalPathSegment(1, 5, 1, kRun),
        CriticalPathSegment(5, 7, 2, kRun),
        CriticalPathSegment(7, 9, 3, kRun),
        CriticalPathSegment(9, 11, 4, kRun),
        CriticalPathSegment(11, 13, 3, kWaitBlocked),
        CriticalPathSegment(13, 14, 3, kRun),
        CriticalPathSegment(14, 16, 2, kWaitBlocked),
        CriticalPathSegment(16, 17, 2, kRun),
        CriticalPathSegment(17, 19, 1, kWaitBlocked),
        CriticalPathSegment(19, 22, 1, kRun),
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

    graph.CreateHorizontalEdge(kRun, nodes[0], nodes[2]);
    graph.CreateHorizontalEdge(kRun, nodes[2], nodes[4]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[4], nodes[7]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[7], nodes[9]);
    graph.CreateHorizontalEdge(kRun, nodes[9], nodes[10]);

    graph.CreateHorizontalEdge(kWaitBlocked, nodes[1], nodes[3]);
    graph.CreateHorizontalEdge(kRun, nodes[3], nodes[5]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[5], nodes[6]);
    graph.CreateHorizontalEdge(kRun, nodes[6], nodes[8]);
    graph.CreateHorizontalEdge(kRun, nodes[8], nodes[11]);

    graph.CreateVerticalEdge(nodes[2], nodes[3]);
    graph.CreateVerticalEdge(nodes[8], nodes[7]);

    CriticalPath path;
    ComputeCriticalPath(graph, 1, 12, 1, &path);

    CriticalPath expectedPath = {
        CriticalPathSegment(1, 6, 1, kRun),
        CriticalPathSegment(6, 7, 2, kRun),
        CriticalPathSegment(7, 8, 2, kWaitBlocked),
        CriticalPathSegment(8, 9, 2, kRun),
        CriticalPathSegment(9, 11, 1, kWaitBlocked),
        CriticalPathSegment(11, 12, 1, kRun),
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

    graph.CreateHorizontalEdge(kRun, nodes[0], nodes[2]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[2], nodes[5]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[5], nodes[7]);
    graph.CreateHorizontalEdge(kRun, nodes[7], nodes[8]);

    graph.CreateHorizontalEdge(kRun, nodes[3], nodes[6]);

    graph.CreateHorizontalEdge(kRun, nodes[1], nodes[4]);
    graph.CreateHorizontalEdge(kRun, nodes[4], nodes[9]);

    graph.CreateVerticalEdge(nodes[4], nodes[3]);
    graph.CreateVerticalEdge(nodes[6], nodes[5]);

    CriticalPath path;
    ComputeCriticalPath(graph, 1, 17, 1, &path);

    CriticalPath expectedPath = {
        CriticalPathSegment(1, 3, 1, kRun),
        CriticalPathSegment(3, 9, 3, kRun),
        CriticalPathSegment(9, 13, 2, kRun),
        CriticalPathSegment(13, 15, 1, kWaitBlocked),
        CriticalPathSegment(15, 17, 1, kRun),
    };

    EXPECT_EQ(expectedPath, path);
}

TEST(ComputeCriticalPath, Network)
{
    // 123456789012
    // ==-------===
    //  |     |
    //  ==   ==  (network thread)
    //   |   |
    //   =====

    // Create the graph.
    CriticalGraph graph;

    std::vector<CriticalNode*> nodes;

    graph.SetTimestamp(1);
    nodes.push_back(graph.CreateNode(1));  // 0

    graph.SetTimestamp(2);
    nodes.push_back(graph.CreateNode(1));  // 1
    nodes.push_back(graph.CreateNode(CriticalGraph::kNetworkThread));  // 2

    graph.SetTimestamp(3);
    nodes.push_back(graph.CreateNode(CriticalGraph::kNetworkThread));  // 3
    nodes.push_back(graph.CreateNode(3));  // 4

    graph.SetTimestamp(7);
    nodes.push_back(graph.CreateNode(CriticalGraph::kNetworkThread));  // 5
    nodes.push_back(graph.CreateNode(3));  // 6

    graph.SetTimestamp(8);
    nodes.push_back(graph.CreateNode(1));  // 7
    nodes.push_back(graph.CreateNode(CriticalGraph::kNetworkThread));  // 8

    graph.SetTimestamp(10);
    nodes.push_back(graph.CreateNode(1));  // 9

    graph.SetTimestamp(12);
    nodes.push_back(graph.CreateNode(1));  // 10

    graph.CreateHorizontalEdge(kRun, nodes[0], nodes[1]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[1], nodes[7]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[7], nodes[9]);
    graph.CreateHorizontalEdge(kRun, nodes[9], nodes[10]);

    graph.CreateHorizontalEdge(kRun, nodes[2], nodes[3]);
    graph.CreateHorizontalEdge(kRun, nodes[5], nodes[8]);

    graph.CreateHorizontalEdge(kRun, nodes[4], nodes[6]);

    graph.CreateVerticalEdge(nodes[1], nodes[2]);
    graph.CreateVerticalEdge(nodes[3], nodes[4]);

    graph.CreateVerticalEdge(nodes[6], nodes[5]);
    graph.CreateVerticalEdge(nodes[8], nodes[7]);

    CriticalPath path;
    ComputeCriticalPath(graph, 1, 12, 1, &path);

    CriticalPath expectedPath = {
        CriticalPathSegment(1, 2, 1, kRun),
        CriticalPathSegment(2, 3, CriticalGraph::kNetworkThread, kRun),
        CriticalPathSegment(3, 7, 3, kRun),
        CriticalPathSegment(7, 8, CriticalGraph::kNetworkThread, kRun),
        CriticalPathSegment(8, 10, 1, kWaitBlocked),
        CriticalPathSegment(10, 12, 1, kRun),
    };

    EXPECT_EQ(expectedPath, path);
}

TEST(ComputeCriticalPath, NetworkAsync)
{
    // 123456789012
    // ====-----===
    //  |     |
    //  ==   ==  (network thread)
    //   |   |
    //   =====

    // Create the graph.
    CriticalGraph graph;

    std::vector<CriticalNode*> nodes;

    graph.SetTimestamp(1);
    nodes.push_back(graph.CreateNode(1));  // 0

    graph.SetTimestamp(2);
    nodes.push_back(graph.CreateNode(1));  // 1
    nodes.push_back(graph.CreateNode(CriticalGraph::kNetworkThread));  // 2

    graph.SetTimestamp(3);
    nodes.push_back(graph.CreateNode(CriticalGraph::kNetworkThread));  // 3
    nodes.push_back(graph.CreateNode(3));  // 4

    graph.SetTimestamp(5);
    CriticalNode* asyncNode = graph.CreateNode(1);

    graph.SetTimestamp(7);
    nodes.push_back(graph.CreateNode(CriticalGraph::kNetworkThread));  // 5
    nodes.push_back(graph.CreateNode(3));  // 6

    graph.SetTimestamp(8);
    nodes.push_back(graph.CreateNode(1));  // 7
    nodes.push_back(graph.CreateNode(CriticalGraph::kNetworkThread));  // 8

    graph.SetTimestamp(10);
    nodes.push_back(graph.CreateNode(1));  // 9

    graph.SetTimestamp(12);
    nodes.push_back(graph.CreateNode(1));  // 10

    graph.CreateHorizontalEdge(kRun, nodes[0], nodes[1]);
    graph.CreateHorizontalEdge(kRun, nodes[1], asyncNode);
    graph.CreateHorizontalEdge(kWaitBlocked, asyncNode, nodes[7]);
    graph.CreateHorizontalEdge(kWaitBlocked, nodes[7], nodes[9]);
    graph.CreateHorizontalEdge(kRun, nodes[9], nodes[10]);

    graph.CreateHorizontalEdge(kRun, nodes[2], nodes[3]);
    graph.CreateHorizontalEdge(kRun, nodes[5], nodes[8]);

    graph.CreateHorizontalEdge(kRun, nodes[4], nodes[6]);

    graph.CreateVerticalEdge(nodes[1], nodes[2]);
    graph.CreateVerticalEdge(nodes[3], nodes[4]);

    graph.CreateVerticalEdge(nodes[6], nodes[5]);
    graph.CreateVerticalEdge(nodes[8], nodes[7]);

    CriticalPath path;
    ComputeCriticalPath(graph, 1, 12, 1, &path);

    CriticalPath expectedPath = {
        CriticalPathSegment(1, 5, 1, kRun),
        CriticalPathSegment(5, 7, 3, kRun),
        CriticalPathSegment(7, 8, CriticalGraph::kNetworkThread, kRun),
        CriticalPathSegment(8, 10, 1, kWaitBlocked),
        CriticalPathSegment(10, 12, 1, kRun),
    };

    EXPECT_EQ(expectedPath, path);
}

}    // namespace critical
}    // namespace tibee
