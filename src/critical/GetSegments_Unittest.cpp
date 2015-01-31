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
#include "gtest/gtest.h"

#include "base/Constants.hpp"
#include "critical/GetSegments.hpp"
#include "execution/LinksBuilder.hpp"
#include "quark/StringQuarkDatabase.hpp"

namespace tibee
{
namespace critical
{

TEST(Execution, GetSegments)
{
//        0         1         2       
//        01234567890123456789012
//     1  **********************
//            |    | |   |    |
//     2      ********   ******

    // Create stacks.
    quark::StringQuarkDatabase quarks;
    execution::LinksBuilder builder;

    builder.SetTimestamp(4);
    builder.AddLink(1, 2);

    builder.SetTimestamp(9);
    builder.AddLink(2, 1);

    builder.SetTimestamp(11);
    builder.AddLink(2, 1);

    builder.SetTimestamp(15);
    builder.AddLink(1, 2);

    builder.SetTimestamp(20);
    builder.AddLink(2, 1);

    // Get execution segments.
    execution::Execution execution;
    execution.set_startThread(1);
    execution.set_endThread(1);
    execution.set_startTs(0);
    execution.set_endTs(21);

    Segments segments;
    GetSegments(execution, builder, &segments);

    std::set<Segment> segmentsSet(
        segments.begin(), segments.end());

    Segment a;
    a.set_thread(1);
    a.set_startTs(0);
    a.set_endTs(21);

    Segment b;
    b.set_thread(2);
    b.set_startTs(4);
    b.set_endTs(11);

    Segment c;
    c.set_thread(2);
    c.set_startTs(15);
    c.set_endTs(20);

    std::set<Segment> expectedSegmentsSet = {a, b, c};

    EXPECT_EQ(expectedSegmentsSet, segmentsSet);
}

TEST(Execution, GetSegmentsWithUnrelated1)
{
//        0         1         2       
//        01234567890123456789012
//     1  **********************
//            |    | |   |    |
//     2      ********   ******
//                        | |
//     3       *****      ***
//             |   |
//     4   **************

    // Create stacks.
    quark::StringQuarkDatabase quarks;
    execution::LinksBuilder builder;

    builder.SetTimestamp(4);
    builder.AddLink(1, 2);

    builder.SetTimestamp(5);
    builder.AddLink(4, 3);

    builder.SetTimestamp(9);
    builder.AddLink(2, 1);
    builder.AddLink(3, 4);

    builder.SetTimestamp(11);
    builder.AddLink(2, 1);

    builder.SetTimestamp(15);
    builder.AddLink(1, 2);

    builder.SetTimestamp(16);
    builder.AddLink(2, 3);

    builder.SetTimestamp(18);
    builder.AddLink(3, 2);

    builder.SetTimestamp(20);
    builder.AddLink(2, 1);

    // Get execution segments.
    execution::Execution execution;
    execution.set_startThread(1);
    execution.set_endThread(1);
    execution.set_startTs(0);
    execution.set_endTs(21);

    Segments segments;
    GetSegments(execution, builder, &segments);

    std::set<Segment> segmentsSet(
        segments.begin(), segments.end());

    Segment a;
    a.set_thread(1);
    a.set_startTs(0);
    a.set_endTs(21);

    Segment b;
    b.set_thread(2);
    b.set_startTs(4);
    b.set_endTs(11);

    Segment c;
    c.set_thread(2);
    c.set_startTs(15);
    c.set_endTs(20);

    Segment d;
    d.set_thread(3);
    d.set_startTs(16);
    d.set_endTs(18);

    std::set<Segment> expectedSegmentsSet = {a, b, c, d};
    EXPECT_EQ(expectedSegmentsSet, segmentsSet);
}

TEST(Execution, GetSegmentsWithUnrelated2)
{
//        0         1         2         3
//        012345678901234567890123456789012
//     1  *********************************
//            |    | |   |    |
//     2      ********   ******
//                        | |
//     3       *****      ***     ******
//             |   |              |    |
//     4   ********************************

    // Create stacks.
    quark::StringQuarkDatabase quarks;
    execution::LinksBuilder builder;

    builder.SetTimestamp(4);
    builder.AddLink(1, 2);

    builder.SetTimestamp(5);
    builder.AddLink(4, 3);

    builder.SetTimestamp(9);
    builder.AddLink(2, 1);
    builder.AddLink(3, 4);

    builder.SetTimestamp(11);
    builder.AddLink(2, 1);

    builder.SetTimestamp(15);
    builder.AddLink(1, 2);

    builder.SetTimestamp(16);
    builder.AddLink(2, 3);

    builder.SetTimestamp(18);
    builder.AddLink(3, 2);

    builder.SetTimestamp(20);
    builder.AddLink(2, 1);

    builder.SetTimestamp(24);
    builder.AddLink(4, 3);

    builder.SetTimestamp(29);
    builder.AddLink(3, 4);

    // Get execution segments.
    execution::Execution execution;
    execution.set_startThread(1);
    execution.set_endThread(1);
    execution.set_startTs(0);
    execution.set_endTs(32);

    Segments segments;
    GetSegments(execution, builder, &segments);

    std::set<Segment> segmentsSet(
        segments.begin(), segments.end());

    Segment a;
    a.set_thread(1);
    a.set_startTs(0);
    a.set_endTs(32);

    Segment b;
    b.set_thread(2);
    b.set_startTs(4);
    b.set_endTs(11);

    Segment c;
    c.set_thread(2);
    c.set_startTs(15);
    c.set_endTs(20);

    Segment d;
    d.set_thread(3);
    d.set_startTs(16);
    d.set_endTs(18);

    std::set<Segment> expectedSegmentsSet = {a, b, c, d};
    EXPECT_EQ(expectedSegmentsSet, segmentsSet);
}

TEST(Execution, GetSegmentsWithSharedResource)
{
//        0         1         2
//        012345678901234567890123
//     1  ************************
//            |    | |   |    |
//     2      ********   ******
//               |        | |
//     3  ********        ***

    // Create stacks.
    quark::StringQuarkDatabase quarks;
    execution::LinksBuilder builder;

    builder.SetTimestamp(4);
    builder.AddLink(1, 2);

    builder.SetTimestamp(7);
    builder.AddLink(3, 2);

    builder.SetTimestamp(9);
    builder.AddLink(2, 1);
    builder.AddLink(3, 4);

    builder.SetTimestamp(11);
    builder.AddLink(2, 1);

    builder.SetTimestamp(15);
    builder.AddLink(1, 2);

    builder.SetTimestamp(16);
    builder.AddLink(2, 3);

    builder.SetTimestamp(18);
    builder.AddLink(3, 2);

    builder.SetTimestamp(20);
    builder.AddLink(2, 1);

    // Get execution segments.
    execution::Execution execution;
    execution.set_startThread(1);
    execution.set_endThread(1);
    execution.set_startTs(0);
    execution.set_endTs(23);

    Segments segments;
    GetSegments(execution, builder, &segments);

    std::set<Segment> segmentsSet(
        segments.begin(), segments.end());

    Segment a;
    a.set_thread(1);
    a.set_startTs(0);
    a.set_endTs(23);

    Segment b;
    b.set_thread(2);
    b.set_startTs(4);
    b.set_endTs(11);

    Segment c;
    c.set_thread(2);
    c.set_startTs(15);
    c.set_endTs(20);

    Segment d;
    d.set_thread(3);
    d.set_startTs(16);
    d.set_endTs(18);

    std::set<Segment> expectedSegmentsSet = {a, b, c, d};
    EXPECT_EQ(expectedSegmentsSet, segmentsSet);
}

}  // namespace critical
}  // namespace tibee
