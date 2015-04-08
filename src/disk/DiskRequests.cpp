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

class DiskRequestComparator
{
public:
    bool operator() (const disk::DiskRequest& rq, timestamp_t ts) const
    {
      return rq.ts < ts;
    }

    bool operator() (timestamp_t ts, const disk::DiskRequest& rq) const
    {
      return ts < rq.ts;
    }
};
}  // namespace

DiskRequests::DiskRequests()
{

}

DiskRequests::~DiskRequests()
{

}

void DiskRequests::Cleanup(timestamp_t ts)
{
  if (_requests.empty())
    return;

  DiskRequestComparator comparator;
  auto it = std::lower_bound(
      _requests.begin(), _requests.end(), ts, comparator);
  if (it == _requests.begin())
    return;
  --it;
  base::CleanVector(it, _requests.end(), &_requests);
}

void DiskRequests::AddDiskRequest(const DiskRequest& rq)
{
  _requests.push_back(rq);
  _requests.back().ts = _ts;
}

std::vector<DiskRequest> DiskRequests::GetRequests(timestamp_t begin, timestamp_t end) const
{
  std::vector<DiskRequest> res;
  if (_requests.empty())
    return res;

  DiskRequestComparator comparator;
  auto it = std::lower_bound(
      _requests.begin(), _requests.end(), _ts, comparator);

  for (; it != _requests.end(); ++it)
  {
    if (it->ts > end)
      break;
    res.push_back(*it);
  }

  return res;
}

}  // namespace disk
}  // namespace tibee
