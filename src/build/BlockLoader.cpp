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
#include "critical_blocks/CriticalBlock.hpp"
#include "execution_blocks/ExecutionBlock.hpp"
#include "execution_blocks/ProcessBlock.hpp"
#include "execution_blocks/PunchBlock.hpp"
#include "execution_blocks/SchedWakeupBlock.hpp"
#include "execution_blocks/SyscallOnlyBlock.hpp"
#include "execution_blocks/ThreadNameBlock.hpp"
#include "state_blocks/CurrentStateBlock.hpp"
#include "state_blocks/LinuxSchedStateBlock.hpp"
#include "symbols_blocks/FinstrumentSymbolsBlock.hpp"
#include "trace_blocks/TraceBlock.hpp"

namespace tibee
{
namespace build
{

block::BlockInterface::UP LoadBlock(const std::string& name)
{
    block::BlockInterface::UP block;

    if (name == kCriticalBlockName) {
        block.reset(new critical_blocks::CriticalBlock);
    } else if (name == kExecutionBlockName) {
        block.reset(new execution_blocks::ExecutionBlock);
    } else if (name == kProcessBlockName) {
        block.reset(new execution_blocks::ProcessBlock);
    } else if (name == kPunchBlockName) {
        block.reset(new execution_blocks::PunchBlock);
    } else if (name == kSchedWakeupBlockName) {
        block.reset(new execution_blocks::SchedWakeupBlock);
    } else if (name == kSyscallOnlyBlockName) {
        block.reset(new execution_blocks::SyscallOnlyBlock);
    } else if (name == kThreadNameBlockName) {
        block.reset(new execution_blocks::ThreadNameBlock);
    } else if (name == kTraceBlockName) {
        block.reset(new trace_blocks::TraceBlock);
    } else if (name == kCurrentStateBlockName) {
        block.reset(new state_blocks::CurrentStateBlock);
    } else if (name == kLinuxSchedStateBlockName) {
        block.reset(new state_blocks::LinuxSchedStateBlock);
    } else if (name == kFinstrumentSymbolsBlockName) {
        block.reset(new symbols_blocks::FinstrumentSymbolsBlock);
    }

    return block;
}

}  // namespace build
}  // namespace tibee
