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

#include "base/Inserter.hpp"
#include "db/Database.hpp"

namespace tibee
{
namespace db
{

TEST(Database, FunctionName)
{
    Database::DestroyTestDb();

    std::unique_ptr<Database> db(new Database(true));

    auto keyA = db->AddFunctionName("a");
    auto keyB = db->AddFunctionName("b");
    auto keyC = db->AddFunctionName("c");
    auto keyD = db->AddFunctionName("d");

    EXPECT_EQ("a", db->GetFunctionName(keyA));
    EXPECT_EQ("b", db->GetFunctionName(keyB));
    EXPECT_EQ("c", db->GetFunctionName(keyC));
    EXPECT_EQ("d", db->GetFunctionName(keyD));

    db.reset(nullptr);
    db.reset(new Database(true));

    EXPECT_EQ("a", db->GetFunctionName(keyA));
    EXPECT_EQ("b", db->GetFunctionName(keyB));
    EXPECT_EQ("c", db->GetFunctionName(keyC));
    EXPECT_EQ("d", db->GetFunctionName(keyD));

    auto keyE = db->AddFunctionName("e");
    auto keyHello = db->AddFunctionName("hello");
    auto keyHelloWorld = db->AddFunctionName("hello world!");

    EXPECT_EQ("e", db->GetFunctionName(keyE));
    EXPECT_EQ("hello", db->GetFunctionName(keyHello));
    EXPECT_EQ("hello world!", db->GetFunctionName(keyHelloWorld));

    auto otherKeyA = db->AddFunctionName("a");
    auto otherKeyB = db->AddFunctionName("b");
    auto otherKeyC = db->AddFunctionName("c");
    auto otherKeyD = db->AddFunctionName("d");

    EXPECT_EQ(keyA, otherKeyA);
    EXPECT_EQ(keyB, otherKeyB);
    EXPECT_EQ(keyC, otherKeyC);
    EXPECT_EQ(keyD, otherKeyD);
}

TEST(Database, Stack)
{
    Database::DestroyTestDb();

    std::unique_ptr<Database> db(new Database(true));

    auto keyA = db->AddStack(stacks::Stack(1, 2));
    auto keyB = db->AddStack(stacks::Stack(3, 4));
    auto keyC = db->AddStack(stacks::Stack(5, 6));
    auto keyD = db->AddStack(stacks::Stack(7, 8));

    EXPECT_EQ(stacks::Stack(1, 2), db->GetStack(keyA));
    EXPECT_EQ(stacks::Stack(3, 4), db->GetStack(keyB));
    EXPECT_EQ(stacks::Stack(5, 6), db->GetStack(keyC));
    EXPECT_EQ(stacks::Stack(7, 8), db->GetStack(keyD));

    db.reset(nullptr);
    db.reset(new Database(true));

    EXPECT_EQ(stacks::Stack(1, 2), db->GetStack(keyA));
    EXPECT_EQ(stacks::Stack(3, 4), db->GetStack(keyB));
    EXPECT_EQ(stacks::Stack(5, 6), db->GetStack(keyC));
    EXPECT_EQ(stacks::Stack(7, 8), db->GetStack(keyD));

    auto keyE = db->AddStack(stacks::Stack(9, 9));
    auto keyF = db->AddStack(stacks::Stack(9, 10));
    auto keyG = db->AddStack(stacks::Stack(10, 9));

    EXPECT_EQ(stacks::Stack(9, 9), db->GetStack(keyE));
    EXPECT_EQ(stacks::Stack(9, 10), db->GetStack(keyF));
    EXPECT_EQ(stacks::Stack(10, 9), db->GetStack(keyG));

    auto otherKeyA = db->AddStack(stacks::Stack(1, 2));
    auto otherKeyB = db->AddStack(stacks::Stack(3, 4));
    auto otherKeyC = db->AddStack(stacks::Stack(5, 6));
    auto otherKeyD = db->AddStack(stacks::Stack(7, 8));

    EXPECT_EQ(keyA, otherKeyA);
    EXPECT_EQ(keyB, otherKeyB);
    EXPECT_EQ(keyC, otherKeyC);
    EXPECT_EQ(keyD, otherKeyD);
}

TEST(Database, Execution)
{
    Database::DestroyTestDb();

    std::unique_ptr<Database> db(new Database(true));

    std::vector<execution::Execution> executions;

    db->EnumerateExecutions("myname", base::BackInserter(&executions));
    EXPECT_TRUE(executions.empty());
    executions.clear();

    execution::Execution a;
    a.set_name("myname");
    a.set_trace("mytrace");
    a.set_startTs(42);
    a.set_startThread(142);
    a.set_endTs(1337);
    a.set_endThread(11337);
    db->AddExecution(a);

    db->EnumerateExecutions("myname", base::BackInserter(&executions));
    EXPECT_EQ(std::vector<execution::Execution>({a}),
              executions);
    executions.clear();

    execution::Execution b;
    b.set_name("myname");
    b.set_trace("myothertrace");
    b.set_startTs(222);
    b.set_startThread(142);
    b.set_endTs(223);
    b.set_endThread(11337);
    db->AddExecution(b);

    execution::Execution c;
    c.set_name("myname");
    c.set_trace("mytrace");
    c.set_startTs(1);
    c.set_startThread(123);
    c.set_endTs(10);
    c.set_endThread(456);
    db->AddExecution(c);

    execution::Execution d;
    d.set_name("differentname");
    d.set_trace("mytrace");
    d.set_startTs(1000);
    d.set_startThread(2000);
    d.set_endTs(3000);
    d.set_endThread(4000);
    d.SetMetric(kDurationMetricId, 4);
    d.SetMetric(kInterruptedMetricId, 8);
    d.IncrementSample(7, 22);
    d.IncrementSample(8, 33);

    db->AddExecution(d);

    db->EnumerateExecutions("myname", base::BackInserter(&executions));
    EXPECT_EQ(std::vector<execution::Execution>({c, a, b}),
              executions);
    executions.clear();

    db->EnumerateExecutions("differentname", base::BackInserter(&executions));
    EXPECT_EQ(std::vector<execution::Execution>({d}),
              executions);
    executions.clear();

    db.reset(nullptr);
    db.reset(new Database(true));

    db->EnumerateExecutions("myname", base::BackInserter(&executions));
    EXPECT_EQ(std::vector<execution::Execution>({c, a, b}),
              executions);
    executions.clear();

    db->EnumerateExecutions("differentname", base::BackInserter(&executions));
    EXPECT_EQ(std::vector<execution::Execution>({d}),
              executions);
    executions.clear();
}

}  // namespace db
}  // namespace tibee
