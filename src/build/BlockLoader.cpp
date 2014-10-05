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
#include "build/BlockLoader.hpp"

#include "base/CompareConstants.hpp"
#include "state_blocks/CurrentStateBlock.hpp"
#include "state_blocks/LinuxSchedStateBlock.hpp"
#include "state_blocks/StateHistoryBlock.hpp"
#include "trace_blocks/TraceBlock.hpp"

namespace tibee
{
namespace build
{

block::BlockInterface::UP LoadBlock(const std::string& name)
{
    block::BlockInterface::UP block;

    if (name == kTraceBlockName) {
        block.reset(new trace_blocks::TraceBlock);
    } else if (name == kCurrentStateBlockName) {
        block.reset(new state_blocks::CurrentStateBlock);
    } else if (name == kStateHistoryBlockName) {
        block.reset(new state_blocks::StateHistoryBlock);
    } else if (name == kLinuxSchedStateBlockName) {
        block.reset(new state_blocks::LinuxSchedStateBlock);
    }

    return block;
}

}  // namespace build
}  // namespace tibee
