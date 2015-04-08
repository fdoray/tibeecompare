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

#include <vector>

#include "base/BasicTypes.hpp"
#include "disk/DiskRequest.hpp"

namespace tibee {
namespace disk {

/**
 * @author Francois Doray
 */
class DiskRequests
{
public:
  DiskRequests();
  ~DiskRequests();

  void SetTimestamp(timestamp_t ts) { _ts = ts; }

  // Removes everything that is before the specified timestamp.
  void Cleanup(timestamp_t ts);

  void AddDiskRequest(const DiskRequest& rq);

  // Returns disk requests completed during the specified time range.
  std::vector<DiskRequest> GetRequests(timestamp_t begin, timestamp_t end) const;

private:
  timestamp_t _ts;

  std::vector<DiskRequest> _requests;
};

}  // namespace disk
}  // namespace tibee

#endif // _TIBEE_DISK_DISKREQUESTS_HPP
