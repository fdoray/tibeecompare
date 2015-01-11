/* Copyright (c) 2014 Francois Doray <francois.pierre-doray@polymtl.ca>
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
#include "execution_blocks/SchedWakeupBlock.hpp"

#include "base/BindObject.hpp"
#include "notification/Token.hpp"

namespace tibee {
namespace execution_blocks {

using notification::Token;

SchedWakeupBlock::SchedWakeupBlock()
{
}

SchedWakeupBlock::~SchedWakeupBlock()
{
}

void SchedWakeupBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    AddKernelObserver(notificationCenter, Token("sched_ttwu"), base::BindObject(&SchedWakeupBlock::OnTTWU, this));
    AddKernelObserver(notificationCenter, Token("softirq_entry"), base::BindObject(&SchedWakeupBlock::OnInterruptEntry, this));
    AddKernelObserver(notificationCenter, Token("softirq_exit"), base::BindObject(&SchedWakeupBlock::OnInterruptExit, this));
    AddKernelObserver(notificationCenter, Token("irq_handler_entry"), base::BindObject(&SchedWakeupBlock::OnInterruptEntry, this));
    AddKernelObserver(notificationCenter, Token("irq_handler_exit"), base::BindObject(&SchedWakeupBlock::OnInterruptExit, this));
    AddKernelObserver(notificationCenter, Token("hrtimer_expire_entry"), base::BindObject(&SchedWakeupBlock::OnInterruptEntry, this));
    AddKernelObserver(notificationCenter, Token("hrtimer_expire_exit"), base::BindObject(&SchedWakeupBlock::OnInterruptExit, this));
}

void SchedWakeupBlock::OnTTWU(const trace::EventValue& event)
{
    auto cpu = CpuForEvent(event);

    // Wake-up from an interrupt handler.
    if (_nestedInterrupts[cpu] != 0)
        return;

    uint32_t source_tid = ThreadForEvent(event);
    uint32_t target_tid = event.getEventField("tid")->AsUInteger();

    Stacks()->AddLink(source_tid, target_tid);
}

void SchedWakeupBlock::OnInterruptEntry(const trace::EventValue& event)
{
    auto cpu = CpuForEvent(event);
    ++_nestedInterrupts[cpu];
}

void SchedWakeupBlock::OnInterruptExit(const trace::EventValue& event)
{
    auto cpu = CpuForEvent(event);
    auto look = _nestedInterrupts.find(cpu);
    if (look != _nestedInterrupts.end() && look->second != 0);
        --look->second;
}

}  // namespace execution_blocks
}  // namespace tibee
