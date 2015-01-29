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
#include "base/Constants.hpp"
#include "notification/Token.hpp"

namespace tibee {
namespace execution_blocks {

using notification::Token;

SchedWakeupBlock::SchedWakeupBlock()
    : _withIO(true), _withFutex(true)
{
}

SchedWakeupBlock::~SchedWakeupBlock()
{
}

void SchedWakeupBlock::Start(const value::Value* params)
{
    auto ioValue = params->GetField("io");
    if (ioValue != nullptr && ioValue->AsString() == "no")
        _withIO = false;

    auto futexValue = params->GetField("futex");
    if (futexValue != nullptr && futexValue->AsString() == "no")
        _withFutex = false;
}

void SchedWakeupBlock::LoadServices(const block::ServiceList& serviceList)
{
    AbstractExecutionBlock::LoadServices(serviceList);

    // Get constant quarks.
    Q_LINUX = State()->Quark(kStateLinux);
    Q_THREADS = State()->Quark(kStateThreads);
    Q_SYSCALL = State()->Quark(kStateSyscall);
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
    AddThreadStateObserver(notificationCenter, notification::Token(kStateExecName),
                           base::BindObject(&SchedWakeupBlock::OnExecName, this));
}

void SchedWakeupBlock::OnTTWU(const trace::EventValue& event)
{
    auto cpu = CpuForEvent(event);

    // Wake-up from an interrupt handler.
    if (_nestedInterrupts[cpu] != 0)
        return;

    uint32_t source_tid = ThreadForEvent(event);
    uint32_t target_tid = event.getEventField("tid")->AsUInteger();

    // Ignore annoying kernel threads.
    if (_excludedThreads.find(target_tid) != _excludedThreads.end())
        return;

    // Handle excluded system calls.
    if (!_withIO || !_withFutex)
    {
        auto syscallValue = State()->GetAttributeValue(
            {Q_LINUX, Q_THREADS, State()->IntQuark(source_tid), Q_SYSCALL});
        if (syscallValue != nullptr)
        {
            auto syscall = syscallValue->AsString();
            if (!_withFutex && syscall == "futex")
                return;
            if (!_withIO && (syscall == "read" || syscall == "write"))
                return;
        }
    }

    Links()->AddLink(source_tid, target_tid);
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

void SchedWakeupBlock::OnExecName(uint32_t tid, const notification::Path& path, const value::Value* value)
{
    auto threadNameValue = value->GetField(kCurrentStateAttributeValueField);
    if (threadNameValue != nullptr)
    {
        std::string threadName(threadNameValue->AsString());
        if (threadName.substr(0, 7) == "kworker" ||
            threadName.substr(0, 5) == "rcuos")
        {
            _excludedThreads.insert(tid);
        }
    }
}

}  // namespace execution_blocks
}  // namespace tibee
