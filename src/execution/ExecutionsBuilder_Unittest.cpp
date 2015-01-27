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

#include <iostream>

#include "execution/ExecutionsBuilder.hpp"

namespace tibee
{
namespace execution
{

namespace
{

const char kTrace[] = "mytrace";
const char kNameA[] = "a";

}  // namespace

TEST(ExecutionsBuilder, NeedsToEnd)
{
    ExecutionsBuilder builder;

    builder.SetTrace(kTrace);

    builder.SetTimestamp(10);
    builder.StartExecution(1, kNameA, true);

    builder.SetTimestamp(20);
    builder.Terminate();

    ASSERT_EQ(1u, builder.ExecutionsCount());
    auto it = builder.begin();

    EXPECT_EQ((*it)->name(), kNameA);
    EXPECT_EQ((*it)->trace(), kTrace);
    EXPECT_EQ((*it)->startTs(), 10u);
    EXPECT_EQ((*it)->startThread(), 1u);
    EXPECT_EQ((*it)->endTs(), 10u);
    EXPECT_EQ((*it)->endThread(), 1u);

    EXPECT_EQ(0u, (*it)->segments().size());

    ++it;
    EXPECT_EQ(it, builder.end());
}

TEST(ExecutionsBuilder, AutoTerminate)
{
    ExecutionsBuilder builder;

    builder.SetTrace(kTrace);

    builder.SetTimestamp(10);
    builder.StartExecution(1, kNameA, false);

    builder.SetTimestamp(20);
    builder.Terminate();

    ASSERT_EQ(1u, builder.ExecutionsCount());
    auto it = builder.begin();

    EXPECT_EQ((*it)->name(), kNameA);
    EXPECT_EQ((*it)->trace(), kTrace);
    EXPECT_EQ((*it)->startTs(), 10u);
    EXPECT_EQ((*it)->startThread(), 1u);
    EXPECT_EQ((*it)->endTs(), 20u);
    EXPECT_EQ((*it)->endThread(), 1u);

    EXPECT_EQ(1u, (*it)->segments().size());
    Segments expectedSegments({{1, 10, 20}});
    EXPECT_EQ(expectedSegments, (*it)->segments());

    ++it;
    EXPECT_EQ(it, builder.end());
}

TEST(ExecutionsBuilder, ManualTerminate)
{
    ExecutionsBuilder builder;

    builder.SetTrace(kTrace);

    builder.SetTimestamp(10);
    builder.StartExecution(1, kNameA, true);

    builder.SetTimestamp(20);
    builder.EndSegment(1);

    ASSERT_EQ(1u, builder.ExecutionsCount());
    auto it = builder.begin();

    EXPECT_EQ((*it)->name(), kNameA);
    EXPECT_EQ((*it)->trace(), kTrace);
    EXPECT_EQ((*it)->startTs(), 10u);
    EXPECT_EQ((*it)->startThread(), 1u);
    EXPECT_EQ((*it)->endTs(), 20u);
    EXPECT_EQ((*it)->endThread(), 1u);

    EXPECT_EQ(1u, (*it)->segments().size());
    Segments expectedSegments({{1, 10, 20}});
    EXPECT_EQ(expectedSegments, (*it)->segments());

    ++it;
    EXPECT_EQ(it, builder.end());
}

TEST(ExecutionsBuilder, MultipleThreads)
{
    ExecutionsBuilder builder;

    builder.SetTrace(kTrace);

    builder.SetTimestamp(10);
    builder.StartExecution(1, kNameA, true);

    builder.SetTimestamp(20);
    builder.StartSegment(1, 2);

    builder.SetTimestamp(30);
    builder.StartSegment(2, 3);

    builder.SetTimestamp(40);
    builder.EndSegment(3);

    builder.SetTimestamp(50);
    builder.EndSegment(2);

    builder.SetTimestamp(60);
    builder.StartSegment(2, 4);

    builder.SetTimestamp(70);
    builder.EndSegment(1);

    //    10  20  30  40  50  60  70  
    // 1   ------------------------
    // 2       ------------
    // 3           ----

    ASSERT_EQ(1u, builder.ExecutionsCount());
    auto it = builder.begin();

    EXPECT_EQ((*it)->name(), kNameA);
    EXPECT_EQ((*it)->trace(), kTrace);
    EXPECT_EQ((*it)->startTs(), 10u);
    EXPECT_EQ((*it)->startThread(), 1u);
    EXPECT_EQ((*it)->endTs(), 70u);
    EXPECT_EQ((*it)->endThread(), 1u);

    EXPECT_EQ(3u, (*it)->segments().size());
    std::set<Segment> expectedSegments({
        {1, 10, 70}, {2, 20, 50}, {3, 30, 40}});
    EXPECT_EQ(expectedSegments,
              std::set<Segment>((*it)->segments().begin(),
                                (*it)->segments().end()));

    ++it;
    EXPECT_EQ(it, builder.end());
}

TEST(ExecutionsBuilder, MultipleThreadsCut)
{
    ExecutionsBuilder builder;

    builder.SetTrace(kTrace);

    builder.SetTimestamp(10);
    builder.StartExecution(1, kNameA, true);

    builder.SetTimestamp(20);
    builder.StartSegment(1, 2);

    builder.SetTimestamp(30);
    builder.StartSegment(2, 3);

    builder.SetTimestamp(40);
    builder.EndSegment(3);

    builder.SetTimestamp(50);
    builder.StartSegment(2, 2);

    builder.SetTimestamp(60);
    builder.EndSegment(2);

    builder.SetTimestamp(70);
    builder.EndSegment(1);

    //    10  20  30  40  50  60  70  
    // 1   ------------------------
    // 2       ------------|---
    // 3           ----

    ASSERT_EQ(1u, builder.ExecutionsCount());
    auto it = builder.begin();

    EXPECT_EQ((*it)->name(), kNameA);
    EXPECT_EQ((*it)->trace(), kTrace);
    EXPECT_EQ((*it)->startTs(), 10u);
    EXPECT_EQ((*it)->startThread(), 1u);
    EXPECT_EQ((*it)->endTs(), 70u);
    EXPECT_EQ((*it)->endThread(), 1u);

    EXPECT_EQ(4u, (*it)->segments().size());
    std::set<Segment> expectedSegments({
        {1, 10, 70}, {2, 20, 49}, {2, 50, 60}, {3, 30, 40}});
    EXPECT_EQ(expectedSegments,
              std::set<Segment>((*it)->segments().begin(),
                                (*it)->segments().end()));

    ++it;
    EXPECT_EQ(it, builder.end());
}

TEST(ExecutionsBuilder, MultipleExecutions)
{
    ExecutionsBuilder builder;

    builder.SetTrace(kTrace);

    builder.SetTimestamp(10);
    builder.StartExecution(1, kNameA, true);

    builder.SetTimestamp(20);
    builder.StartSegment(1, 2);

    builder.SetTimestamp(30);
    builder.EndSegment(2);

    builder.StartExecution(3, kNameA, true);

    builder.SetTimestamp(40);
    builder.EndSegment(1);

    builder.SetTimestamp(50);
    builder.EndSegment(3);

    //    10  20  30  40  50
    // 1   ------------
    // 2       ----
    //
    // 3           --------

    ASSERT_EQ(2u, builder.ExecutionsCount());
    auto it = builder.begin();

    EXPECT_EQ((*it)->name(), kNameA);
    EXPECT_EQ((*it)->trace(), kTrace);
    EXPECT_EQ((*it)->startTs(), 10u);
    EXPECT_EQ((*it)->startThread(), 1u);
    EXPECT_EQ((*it)->endTs(), 40u);
    EXPECT_EQ((*it)->endThread(), 1u);

    EXPECT_EQ(2u, (*it)->segments().size());
    std::set<Segment> expectedSegments({
        {1, 10, 40}, {2, 20, 30}});
    EXPECT_EQ(expectedSegments,
              std::set<Segment>((*it)->segments().begin(),
                                (*it)->segments().end()));
    ++it;

    EXPECT_EQ((*it)->name(), kNameA);
    EXPECT_EQ((*it)->trace(), kTrace);
    EXPECT_EQ((*it)->startTs(), 30u);
    EXPECT_EQ((*it)->startThread(), 3u);
    EXPECT_EQ((*it)->endTs(), 50u);
    EXPECT_EQ((*it)->endThread(), 3u);

    EXPECT_EQ(1u, (*it)->segments().size());
    expectedSegments = {{3, 30, 50}};
    EXPECT_EQ(expectedSegments,
              std::set<Segment>((*it)->segments().begin(),
                                (*it)->segments().end()));

    ++it;

    EXPECT_EQ(it, builder.end());
}

}  // namespace execution
}  // namespace tibee
