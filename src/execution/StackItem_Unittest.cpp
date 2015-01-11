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

#include <string>

#include "base/BasicTypes.hpp"
#include "base/Constants.hpp"
#include "execution/StackItem.hpp"
#include "quark/DiskQuarkDatabase.hpp"

namespace tibee
{
namespace execution
{

TEST(Execution, StackItem)
{
    const quark::Quark kNameQuark(42);
    const size_t kDepth = 4;
    const timestamp_t kStart = 10000;
    const timestamp_t kEnd = 20000;

    quark::DiskQuarkDatabase quarks(kDiskQuarkDatabaseTestFile);

    StackItem item;
    EXPECT_EQ(quark::Quark(), item.name());
    item.set_name(kNameQuark);
    EXPECT_EQ(kNameQuark, item.name());

    EXPECT_EQ(0u, item.depth());
    item.set_depth(kDepth);
    EXPECT_EQ(kDepth, item.depth());

    EXPECT_EQ(0u, item.start());
    item.set_start(kStart);
    EXPECT_EQ(kStart, item.start());

    EXPECT_EQ(0u, item.end());
    item.set_end(kEnd);
    EXPECT_EQ(kEnd, item.end());
}

}  // namespace execution
}  // namespace tibee
