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
#ifndef _TIBEE_DISK_DISKREQUEST_HPP
#define _TIBEE_DISK_DISKREQUEST_HPP

#include "base/BasicTypes.hpp"

namespace tibee {
namespace disk {

enum class DiskRequestType {
  kUnknown,
  kRead,
  kWrite
};

/**
 * @author Francois Doray
 */
struct DiskRequest
{
  DiskRequest()
    : ts(0),
      nrSector(0),
      size(0),
      tid(-1),
      type(DiskRequestType::kUnknown) {}

  timestamp_t ts;
  uint64_t nrSector;
  uint64_t size;
  uint64_t tid;
  DiskRequestType type;
};

}  // namespace disk
}  // namespace tibee

#endif // _TIBEE_DISK_DISKREQUEST_HPP
