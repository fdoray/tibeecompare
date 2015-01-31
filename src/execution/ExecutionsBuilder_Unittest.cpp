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

#include "execution/ExecutionsBuilder.hpp"

namespace tibee
{
namespace execution
{

namespace
{

const thread_t kThreadA = 1;
const char kNameA[] = "a";

const thread_t kThreadB = 2;
const char kNameB[] = "b";

}  // namespace

TEST(ExecutionsBuilder, Simple)
{
    ExecutionsBuilder builder;

    builder.SetTimestamp(10);
    builder.StartExecution(kThreadA, kNameA, true);
    builder.SetTimestamp(20);
    builder.EndExecution(kThreadA);

    auto it = builder.begin();
    ASSERT_NE(it, builder.end());

    EXPECT_EQ(kNameA, (*it)->name());
    EXPECT_EQ(10u, (*it)->startTs());
    EXPECT_EQ(kThreadA, (*it)->startThread());
    EXPECT_EQ(20u, (*it)->endTs());
    EXPECT_EQ(kThreadA, (*it)->endThread());

    ++it;
    EXPECT_EQ(builder.end(), it);
}

TEST(ExecutionsBuilder, Terminate)
{
    ExecutionsBuilder builder;

    builder.SetTimestamp(10);
    builder.StartExecution(kThreadA, kNameA, true);

    builder.SetTimestamp(20);
    builder.StartExecution(kThreadB, kNameB, false);

    builder.SetTimestamp(30);
    builder.Terminate();

    auto it = builder.begin();
    ASSERT_NE(it, builder.end());

    EXPECT_EQ(kNameB, (*it)->name());
    EXPECT_EQ(20u, (*it)->startTs());
    EXPECT_EQ(kThreadB, (*it)->startThread());
    EXPECT_EQ(30u, (*it)->endTs());
    EXPECT_EQ(kThreadB, (*it)->endThread());

    ++it;
    EXPECT_EQ(builder.end(), it);
}


}  // namespace execution
}  // namespace tibee
