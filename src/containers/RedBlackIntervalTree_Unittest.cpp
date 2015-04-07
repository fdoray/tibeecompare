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
#include "containers/RedBlackIntervalTree.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace tibee {
namespace containers {

namespace {

namespace pl = std::placeholders;

class MockObserver {
 public:
  // This is a workaround for an issue with gmock and std::pair on MSVC.
  // See https://code.google.com/p/googlemock/issues/detail?id=158.
  void ObserveElement(const RedBlackIntervalTree<int>::ElementPair& pair) {
    ObserveElementMock(pair.first, pair.second);
  }

  MOCK_METHOD2(ObserveElementMock, void(const Interval&, const int&));
};

}  // namespace

TEST(RedBlackIntervalTreeTest, Size) {
  RedBlackIntervalTree<int> tree;
  EXPECT_EQ(0u, tree.size());

  tree.Insert(Interval(0, 10), 1);
  EXPECT_EQ(1u, tree.size());

  tree.Insert(Interval(0, 10), 2);
  EXPECT_EQ(2u, tree.size());

  tree.Insert(Interval(15, 25), 3);
  EXPECT_EQ(3u, tree.size());
}

TEST(RedBlackIntervalTreeTest, InsertManyElements) {
  RedBlackIntervalTree<int> tree;
  MockObserver observer;

  for (int i = 0; i < 500; i += 5) {
    Interval interval(i, i + 1);
    int value = i;
    
    tree.Insert(interval, value);
    
    EXPECT_CALL(observer, ObserveElementMock(interval, value));
    tree.EnumerateIntersection(Interval(i, i + 2),
                               std::bind(&MockObserver::ObserveElement,
                                         &observer, pl::_1));
  }
}

TEST(RedBlackIntervalTreeTest, EnumerateIntersection) {
  RedBlackIntervalTree<int> tree;
  MockObserver observer;

  tree.Insert(Interval(0, 10), 1);
  tree.Insert(Interval(10, 20), 2);
  tree.Insert(Interval(20, 30), 3);
  tree.Insert(Interval(40, 50), 4);
  tree.Insert(Interval(0, 50), 5);

  {
    ::testing::InSequence sequence;
    EXPECT_CALL(observer, ObserveElementMock(Interval(0, 50), 5));
    EXPECT_CALL(observer, ObserveElementMock(Interval(0, 10), 1));
    EXPECT_CALL(observer, ObserveElementMock(Interval(10, 20), 2));
    EXPECT_CALL(observer, ObserveElementMock(Interval(20, 30), 3));
    EXPECT_CALL(observer, ObserveElementMock(Interval(40, 50), 4));

    tree.EnumerateIntersection(Interval(0, 50),
                               std::bind(&MockObserver::ObserveElement,
                                         &observer, pl::_1));
  }

  {
    ::testing::InSequence sequence;
    EXPECT_CALL(observer, ObserveElementMock(Interval(0, 50), 5));
    EXPECT_CALL(observer, ObserveElementMock(Interval(0, 10), 1));
    EXPECT_CALL(observer, ObserveElementMock(Interval(10, 20), 2));
    EXPECT_CALL(observer, ObserveElementMock(Interval(20, 30), 3));
    EXPECT_CALL(observer, ObserveElementMock(Interval(40, 50), 4));

    tree.EnumerateIntersection(Interval(0, 49),
                               std::bind(&MockObserver::ObserveElement,
                                         &observer, pl::_1));
  }

  {
    ::testing::InSequence sequence;
    EXPECT_CALL(observer, ObserveElementMock(Interval(0, 50), 5));
    EXPECT_CALL(observer, ObserveElementMock(Interval(0, 10), 1));

    tree.EnumerateIntersection(Interval(0, 0),
                               std::bind(&MockObserver::ObserveElement,
                                         &observer, pl::_1));
  }

  {
    ::testing::InSequence sequence;
    EXPECT_CALL(observer, ObserveElementMock(Interval(0, 50), 5));
    EXPECT_CALL(observer, ObserveElementMock(Interval(0, 10), 1));
    EXPECT_CALL(observer, ObserveElementMock(Interval(10, 20), 2));

    tree.EnumerateIntersection(Interval(0, 10),
                               std::bind(&MockObserver::ObserveElement,
                                         &observer, pl::_1));
  }

  {
    ::testing::InSequence sequence;
    EXPECT_CALL(observer, ObserveElementMock(Interval(0, 50), 5));
    EXPECT_CALL(observer, ObserveElementMock(Interval(0, 10), 1));
    EXPECT_CALL(observer, ObserveElementMock(Interval(10, 20), 2));
    EXPECT_CALL(observer, ObserveElementMock(Interval(20, 30), 3));

    tree.EnumerateIntersection(Interval(0, 25),
                               std::bind(&MockObserver::ObserveElement,
                                         &observer, pl::_1));
  }
}

}  // namespace containers
}  // namespace tibee
