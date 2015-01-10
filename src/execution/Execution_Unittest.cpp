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

#include "execution/Execution.hpp"

namespace tibee
{
namespace execution
{

TEST(Execution, Execution)
{
    const char kName[] = "name";
    const char kTrace[] = "trace";
    const timestamp_t kStartTs = 10;
    const thread_t kStartThread = 1;
    const timestamp_t kEndTs = 20;
    const thread_t kEndThread = 2;

    const quark::Quark kKey(5);
    const uint64_t kValue = 42;

    Execution execution;
    EXPECT_EQ("", execution.name());
    EXPECT_EQ("", execution.trace());
    EXPECT_EQ(0u, execution.startTs());
    EXPECT_EQ(static_cast<thread_t>(-1), execution.startThread());
    EXPECT_EQ(0u, execution.endTs());
    EXPECT_EQ(static_cast<thread_t>(-1), execution.endThread());
    EXPECT_EQ(0u, execution.MetricsCount());

    execution.set_name(kName);
    execution.set_trace(kTrace);
    execution.set_startTs(kStartTs);
    execution.set_startThread(kStartThread);
    execution.set_endTs(kEndTs);
    execution.set_endThread(kEndThread);
    execution.SetMetric(kKey, kValue);

    EXPECT_EQ(kName, execution.name());
    EXPECT_EQ(kTrace, execution.trace());
    EXPECT_EQ(kStartTs, execution.startTs());
    EXPECT_EQ(kStartThread, execution.startThread());
    EXPECT_EQ(kEndTs, execution.endTs());
    EXPECT_EQ(kEndThread, execution.endThread());
    EXPECT_EQ(1u, execution.MetricsCount());

    uint64_t value = 0;
    EXPECT_TRUE(execution.GetMetric(kKey, &value));
    EXPECT_EQ(kValue, value);
}

}  // namespace execution
}  // namespace tibee
