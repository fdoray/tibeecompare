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

#include "build_blocks/BuildBlock.hpp"
#include "critical_blocks/CriticalBlock.hpp"
#include "execution_blocks/PunchBlock.hpp"
#include "stacks_blocks/DumpStacksBlock.hpp"
#include "stacks_blocks/ProfilerBlock.hpp"
#include "state_blocks/CurrentStateBlock.hpp"
#include "state_blocks/LinuxSchedStateBlock.hpp"
#include "trace_blocks/TraceBlock.hpp"

#define BLOCK(blockName, blockType)  \
    if (name == blockName) {         \
	    block.reset(new blockType);  \
        return block;                \
    }

namespace tibee
{
namespace build
{

block::BlockInterface::UP LoadBlock(const std::string& name)
{
    block::BlockInterface::UP block;

    BLOCK("build", build_blocks::BuildBlock);
    BLOCK("critical", critical_blocks::CriticalBlock);
    BLOCK("punch", execution_blocks::PunchBlock);
    BLOCK("dump-stacks", stacks_blocks::DumpStacksBlock);
    BLOCK("profiler", stacks_blocks::ProfilerBlock);
    BLOCK("current-state", state_blocks::CurrentStateBlock);
    BLOCK("linux-sched-state", state_blocks::LinuxSchedStateBlock);
    BLOCK("trace", trace_blocks::TraceBlock);

    return block;
}

}  // namespace build
}  // namespace tibee
