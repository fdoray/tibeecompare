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
#include "disk_blocks/DiskBlock.hpp"

#include "base/BindObject.hpp"
#include "value/Value.hpp"

namespace tibee {
namespace disk_blocks {

namespace
{
using notification::Token;

uint64_t kBlockSize = 512;
}  // namespace

DiskBlock::DiskBlock()
{
}

DiskBlock::~DiskBlock()
{
}

void DiskBlock::AddObservers(
    notification::NotificationCenter* notificationCenter)
{
    AddKernelObserver(notificationCenter,
                      Token("block_bio_remap"),
                      base::BindObject(&DiskBlock::OnBlockBioRemap, this));
    AddKernelObserver(notificationCenter,
                      Token("block_bio_backmerge"),
                      base::BindObject(&DiskBlock::OnBlockBioBackMerge, this));
    AddKernelObserver(notificationCenter,
                      Token("block_rq_issue"),
                      base::BindObject(&DiskBlock::OnBlockRqIssue, this));
    AddKernelObserver(notificationCenter,
                      Token("block_rq_complete"),
                      base::BindObject(&DiskBlock::OnBlockRqComplete, this));
}

void DiskBlock::OnBlockBioRemap(const trace::EventValue& event)
{
  uint64_t dev = event.getEventField("dev")->AsULong();
  uint64_t sector = event.getEventField("sector")->AsULong();
  uint64_t oldDev = event.getEventField("old_dev")->AsULong();
  uint64_t oldSector = event.getEventField("old_sector")->AsULong();

  for (auto& req : _remapRequests)
  {
    if (req.dev == oldDev && req.sector == oldSector)
    {
      req.dev = dev;
      req.sector = sector;
      return;
    }
  }

  RemapRequest req;
  req.origDev = oldDev;
  req.dev = dev;
  req.sector = sector;
  _remapRequests.push_back(req);
}

void DiskBlock::OnBlockBioBackMerge(const trace::EventValue& event)
{
  uint64_t dev = event.getEventField("dev")->AsULong();
  uint64_t sector = event.getEventField("sector")->AsULong();

  for (auto it = _remapRequests.begin(); it != _remapRequests.end(); ++it)
  {
    if (it->dev == dev && it->sector == sector) {
      _remapRequests.erase(it);
      return;
    }
  }
}

void DiskBlock::OnBlockRqIssue(const trace::EventValue& event)
{
  uint64_t dev = event.getEventField("dev")->AsULong();
  uint64_t sector = event.getEventField("sector")->AsULong();
  uint64_t nrSector = event.getEventField("nr_sector")->AsULong();
  uint64_t tid = event.getEventField("tid")->AsULong();
  uint64_t rwbs = event.getEventField("rwbs")->AsULong();

  if (nrSector == 0)
    return;

  disk::DiskRequest rq;
  rq.nrSector = nrSector;
  rq.size = nrSector * kBlockSize;
  rq.tid = tid;
  rq.type = (rwbs % 2 == 0) ?
      disk::DiskRequestType::kRead : disk::DiskRequestType::kWrite;

  GetDiskUsingRemaps(dev, sector)->insert(SectorToDiskRequest::value_type {sector, rq});
}

void DiskBlock::OnBlockRqComplete(const trace::EventValue& event)
{
  uint64_t dev = event.getEventField("dev")->AsULong();
  uint64_t sector = event.getEventField("sector")->AsULong();
  uint64_t nrSector = event.getEventField("nr_sector")->AsULong();

  auto* disk = GetDiskUsingRemaps(dev, sector);
  auto look = disk->find(sector);
  if (look == disk->end())
    return;

  disk::DiskRequest rq = look->second;
  if (rq.nrSector != nrSector)
    return;

  DiskRequests()->AddDiskRequest(rq);
  disk->erase(look);
}

DiskBlock::SectorToDiskRequest* DiskBlock::GetDiskUsingRemaps(uint64_t dev, uint64_t sector)
{
  for (auto& remapRequest : _remapRequests)
  {
    if (remapRequest.dev == dev && remapRequest.sector == sector)
      return GetDisk(remapRequest.origDev);
  }
  return GetDisk(dev);
}

DiskBlock::SectorToDiskRequest* DiskBlock::GetDisk(uint64_t dev)
{
  return &_disks[dev];
}

}  // namespace disk_blocks
}  // namespace tibee
