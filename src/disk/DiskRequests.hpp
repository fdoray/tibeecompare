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
#ifndef _TIBEE_DISK_DISKREQUESTS_HPP
#define _TIBEE_DISK_DISKREQUESTS_HPP

#include <functional>
#include <vector>

#include "base/BasicTypes.hpp"
#include "containers/RedBlackIntervalTree.hpp"

namespace tibee {
namespace disk {

/**
 * @author Francois Doray
 */
class DiskRequests
{
public:
  typedef std::function<void (const containers::Interval&, thread_t thread)> EnumerateCallback;

  DiskRequests();
  ~DiskRequests();

  void SetTimestamp(timestamp_t ts) { _ts = ts; }

  // Removes everything that is before the specified timestamp.
  void Cleanup(timestamp_t ts);

  void AddInterval(timestamp_t start, timestamp_t end, thread_t tid);
  void EnumerateIntervals(timestamp_t start, timestamp_t end, const EnumerateCallback& callback) const;
  std::vector<std::pair<containers::Interval, thread_t>> GetIntervals(timestamp_t start, timestamp_t end) const;

private:
  timestamp_t _ts;
  containers::RedBlackIntervalTree<thread_t> _intervals;
};

}  // namespace disk
}  // namespace tibee

#endif // _TIBEE_DISK_DISKREQUESTS_HPP
