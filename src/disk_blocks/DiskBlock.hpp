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
#ifndef _TIBEE_DISKBLOCKS_DISKBLOCK_HPP
#define _TIBEE_DISKBLOCKS_DISKBLOCK_HPP

#include <unordered_map>

#include "base/BasicTypes.hpp"
#include "build_blocks/AbstractBuildBlock.hpp"
#include "disk/DiskRequest.hpp"
#include "symbols/SymbolLookup.hpp"
#include "trace/value/EventValue.hpp"

namespace tibee {
namespace disk_blocks {

/**
 * Block that keeps track of disk requests.
 *
 * @author Francois Doray
 */
class DiskBlock : public build_blocks::AbstractBuildBlock
{
public:
    DiskBlock();
    ~DiskBlock();

    virtual void AddObservers(notification::NotificationCenter* notificationCenter) override;

private:
    void OnBlockBioRemap(const trace::EventValue& event);
    void OnBlockBioBackMerge(const trace::EventValue& event);
    void OnBlockRqIssue(const trace::EventValue& event);
    void OnBlockRqComplete(const trace::EventValue& event);

    typedef std::unordered_map<uint64_t, disk::DiskRequest> SectorToDiskRequest;
    typedef std::unordered_map<uint64_t, SectorToDiskRequest> Disks;
    Disks _disks;

    SectorToDiskRequest* GetDiskUsingRemaps(uint64_t dev, uint64_t sector);
    SectorToDiskRequest* GetDisk(uint64_t dev);

    struct RemapRequest {
      uint64_t origDev;
      uint64_t dev;
      uint64_t sector;
    };
    std::list<RemapRequest> _remapRequests;
};

}  // namespace disk_blocks
}  // namespace tibee

#endif // _TIBEE_DISKBLOCKS_DISKBLOCK_HPP
