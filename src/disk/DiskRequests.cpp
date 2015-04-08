/* Copyright (c) 2015 Francois Doray <francois.pierre-doray@polymtl.ca>
 *
 * This file is part of tigerbeetle.
 *
 * tigerbeetle is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tigerbeetle is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tigerbeetle.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "disk/DiskRequests.hpp"

#include <algorithm>

#include "base/CleanContainer.hpp"
#include "base/print.hpp"

namespace tibee {
namespace disk {

namespace {

void CallbackWrapper(
    const containers::RedBlackIntervalTree<thread_t>::ElementPair& pair,
    const DiskRequests::EnumerateCallback& callback)
{
  callback(pair.first, pair.second);
}

void CleanCallback(
    const containers::RedBlackIntervalTree<thread_t>::ElementPair& pair,
    containers::RedBlackIntervalTree<thread_t>* intervals)
{
  intervals->Insert(pair.first, pair.second);
}

void InsertCallback(
  const containers::RedBlackIntervalTree<thread_t>::ElementPair& pair,
  std::vector<std::pair<containers::Interval, thread_t>>* vec)
{
  vec->push_back(pair);
}

}  // namespace

DiskRequests::DiskRequests()
{

}

DiskRequests::~DiskRequests()
{

}

void DiskRequests::Cleanup(timestamp_t ts)
{
  namespace pl = std::placeholders;

  containers::RedBlackIntervalTree<thread_t> newIntervals;
  _intervals.EnumerateIntersection(
    containers::Interval(ts, std::numeric_limits<uint64_t>::max()),
    std::bind(CleanCallback, pl::_1, &newIntervals)
  );

  _intervals = std::move(newIntervals);
}

void DiskRequests::AddInterval(timestamp_t start, timestamp_t end, thread_t tid)
{
  _intervals.Insert(containers::Interval(start, end), tid);
}

void DiskRequests::EnumerateIntervals(timestamp_t start, timestamp_t end, const EnumerateCallback& callback) const
{
  namespace pl = std::placeholders;

  _intervals.EnumerateIntersection(
      containers::Interval(start, end),
      std::bind(CallbackWrapper, pl::_1, std::ref(callback)));
}

std::vector<std::pair<containers::Interval, thread_t>> DiskRequests::GetIntervals(timestamp_t start, timestamp_t end) const
{
  namespace pl = std::placeholders;

  std::vector<std::pair<containers::Interval, thread_t>> vec;
  _intervals.EnumerateIntersection(
      containers::Interval(start, end),
      std::bind(InsertCallback, pl::_1, &vec));
  return vec;
}

}  // namespace disk
}  // namespace tibee
