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
#include "execution/GetExecutionSegments.hpp"
#include "execution/StacksBuilder.hpp"
#include "quark/DiskQuarkDatabase.hpp"

namespace tibee
{
namespace execution
{

TEST(Execution, GetExecutionSegments)
{
//        0         1         2       
//        01234567890123456789012
//     1  **********************
//            |    | |   |    |
//     2      ********   ******

    // Create stacks.
    quark::DiskQuarkDatabase quarks(kDiskQuarkDatabaseTestFile);
    StacksBuilder builder;

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
    Execution execution;
    execution.set_startTs(0);
    execution.set_startThread(1);
    execution.set_endTs(21);
    execution.set_endThread(1);

    std::vector<Link> links;
    std::vector<ExecutionSegment> executionSegments;
    GetExecutionSegments(execution, builder, &links, &executionSegments);

    std::set<ExecutionSegment> executionSegmentsSet(
        executionSegments.begin(), executionSegments.end());

    ExecutionSegment a;
    a.set_thread(1);
    a.set_startTs(0);
    a.set_endTs(21);

    ExecutionSegment b;
    b.set_thread(2);
    b.set_startTs(4);
    b.set_endTs(11);

    ExecutionSegment c;
    c.set_thread(2);
    c.set_startTs(15);
    c.set_endTs(20);

    std::set<ExecutionSegment> expectedExecutionSegmentsSet = {a, b, c};

    EXPECT_EQ(expectedExecutionSegmentsSet, executionSegmentsSet);
}

}  // namespace execution
}  // namespace tibee
