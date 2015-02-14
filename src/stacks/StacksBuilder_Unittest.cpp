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
#include "gtest/gtest.h"

#include <utility>

#include "db/Database.hpp"
#include "stacks/StacksBuilder.hpp"

namespace tibee
{
namespace stacks
{

namespace
{

typedef std::pair<StackId, timestamp_t> Pair;

void Callback(
    StackId stackId, timestamp_t duration, std::vector<Pair>* stacks)
{
    stacks->push_back(std::make_pair(stackId, duration));
}

}  // namespace

TEST(StacksBuilder, StacksBuilder)
{
    namespace pl = std::placeholders;

    db::Database::DestroyTestDb();
    db::Database db(true);
    StacksBuilder builder;
    builder.SetDatabase(&db);

    std::vector<Pair> stacks;
    builder.EnumerateStacks(
        1, 1500, 3500,
        std::bind(&Callback, pl::_1, pl::_2, &stacks));
    EXPECT_TRUE(stacks.empty());
    stacks.clear();

    builder.SetTimestamp(1000);
    builder.SetStack(1, 1);
    builder.SetTimestamp(2000);
    builder.SetStack(1, 2);
    builder.SetTimestamp(3000);
    builder.SetStack(1, 3);
    builder.SetStack(2, 4);
    builder.SetTimestamp(4000);
    builder.Terminate();

    builder.EnumerateStacks(
        1, 1500, 3500,
        std::bind(&Callback, pl::_1, pl::_2, &stacks));
    std::vector<Pair> expectedStacks({{1, 500}, {2, 1000}, {3, 500}});
    EXPECT_EQ(expectedStacks, stacks);
    stacks.clear();

    builder.EnumerateStacks(
        1, 0, 5000,
        std::bind(&Callback, pl::_1, pl::_2, &stacks));
    expectedStacks = std::vector<Pair>({{1, 1000}, {2, 1000}, {3, 1000}});
    EXPECT_EQ(expectedStacks, stacks);
    stacks.clear();
}

}  // namespace stacks
}  // namespace tibee
