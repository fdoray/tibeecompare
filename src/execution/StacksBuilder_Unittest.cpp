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

#include <vector>

#include "execution/StacksBuilder.hpp"

namespace tibee
{
namespace execution
{

namespace
{

namespace pl = std::placeholders;

void StacksCallback(std::vector<StackItem>* vec, const StackItem& item)
{
    vec->push_back(item);
}

void LinksCallback(std::vector<Link>* vec, const Link& link)
{
    vec->push_back(link);
}

}  // namespace

TEST(StacksBuilder, StacksBuilder)
{

//        1234567890123456789012
//     1  aaaaaaaaaaaaaaaaaaaa
//          ccc      bbbbbbbb
//            |        |  |
//            |        |  |
//     2      ddddddddddddddddd
//               eee

    // Create stacks.
    StacksBuilder builder;

    builder.SetTimestamp(1);
    builder.PushStack(1, "a");

    builder.SetTimestamp(3);
    builder.PushStack(1, "c");

    builder.SetTimestamp(5);
    builder.AddLink(1, 2);
    builder.PushStack(2, "d");

    builder.SetTimestamp(6);
    builder.PopStack(1);

    builder.SetTimestamp(8);
    builder.PushStack(2, "e");

    builder.SetTimestamp(11);
    builder.PopStack(2);

    builder.SetTimestamp(12);
    builder.PushStack(1, "b");

    builder.SetTimestamp(14);
    builder.AddLink(2, 1);

    builder.SetTimestamp(17);
    builder.AddLink(2, 1);

    builder.SetTimestamp(20);
    builder.PopStack(1);

    builder.SetTimestamp(21);
    builder.PopStack(1);

    builder.SetTimestamp(22);
    builder.PopStack(2);

    // Check the result.
    StackItem a;
    a.set_name("a");
    a.set_depth(0);
    a.set_start(1);
    a.set_end(21);

    StackItem b;
    b.set_name("b");
    b.set_depth(1);
    b.set_start(12);
    b.set_end(20);

    StackItem c;
    c.set_name("c");
    c.set_depth(1);
    c.set_start(3);
    c.set_end(6);

    StackItem d;
    d.set_name("d");
    d.set_depth(0);
    d.set_start(5);
    d.set_end(22);

    StackItem e;
    e.set_name("e");
    e.set_depth(1);
    e.set_start(8);
    e.set_end(11);

    std::vector<StackItem> expected;
    std::vector<StackItem> actual;

    expected = {a, c, b};
    builder.EnumerateStacks(
        1, containers::Interval(1, 21),
        std::bind(&StacksCallback, &actual, pl::_1));
    EXPECT_EQ(expected, actual);
    actual.clear();

    expected = {d, e};
    builder.EnumerateStacks(
        2, containers::Interval(1, 21),
        std::bind(&StacksCallback, &actual, pl::_1));
    EXPECT_EQ(expected, actual);
    actual.clear();

    expected = {a, b};
    builder.EnumerateStacks(
        1, containers::Interval(10, 17),
        std::bind(&StacksCallback, &actual, pl::_1));
    EXPECT_EQ(expected, actual);
    actual.clear();

    std::vector<Link> expectedLinks;
    std::vector<Link> actualLinks;

    Link linkOne(1, 5, 2, 5);
    Link linkTwo(2, 14, 1, 14);
    Link linkThree(2, 17, 1, 17);

    expectedLinks = {linkOne, linkTwo, linkThree};
    builder.EnumerateLinks(
        containers::Interval(1, 21),
        std::bind(&LinksCallback, &actualLinks, pl::_1));
    EXPECT_EQ(expectedLinks, actualLinks);
    actualLinks.clear();

    expectedLinks = {linkTwo, linkThree};
    builder.EnumerateLinks(
        containers::Interval(14, 17),
        std::bind(&LinksCallback, &actualLinks, pl::_1));
    EXPECT_EQ(expectedLinks, actualLinks);
    actualLinks.clear();
}

}  // namespace execution
}  // namespace tibee
