/* Copyright (c) 2015 Francois Doray <francois.pierre-doray@polymtl.ca>
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
#include "critical_blocks/CriticalBlock.hpp"

#include "base/BindObject.hpp"
#include "base/CompareConstants.hpp"
#include "base/Constants.hpp"
#include "base/print.hpp"
#include "block/ServiceList.hpp"
#include "critical/CriticalTypes.hpp"
#include "notification/NotificationCenter.hpp"

namespace tibee {
namespace critical_blocks {

namespace {

using base::tbendl;
using base::tberror;
using notification::Token;

critical::CriticalEdgeType ResolveIRQ(uint32_t irq)
{
    switch (irq)
    {
        case 0:
            return critical::kInterrupted;
        case 19:
        case 23:
            return critical::kUserInput;
        default:
            return critical::kWaitBlocked;
    }
}

critical::CriticalEdgeType ResolveSoftIRQ(uint32_t vec)
{
    // const uint32_t kHi = 0;
    const uint32_t kTimer = 1;
    const uint32_t kNetTx = 2;
    const uint32_t kNetRx = 3;
    const uint32_t kBlock = 4;
    const uint32_t kBlockIoPoll = 5;
    // const uint32_t kTasklet = 6;
    const uint32_t kSched = 7;
    const uint32_t kHrTimer = 8;
    // const uint32_t kRcu = 9;

    switch (vec)
    {
        case kTimer:
        case kHrTimer:
            return critical::kTimer;
        case kBlock:
        case kBlockIoPoll:
            return critical::kBlockDevice;
        case kNetTx:
        case kNetRx:
            return critical::kNetwork;
        case kSched:
            return critical::kInterrupted;
        default:
            return critical::kWaitBlocked;
    }
}

uint32_t GetEventCPU(const trace::EventValue& event)
{
    return event.getStreamPacketContext()->GetField("cpu_id")->AsUInteger();
}

}  // namespace

CriticalBlock::CriticalBlock()
{
}

CriticalBlock::~CriticalBlock()
{
}

void CriticalBlock::LoadServices(const block::ServiceList& serviceList)
{
    AbstractBuildBlock::LoadServices(serviceList);

    Q_RUN_USERMODE = Quarks()->StrQuark(kStateRunUsermode);
    Q_RUN_SYSCALL = Quarks()->StrQuark(kStateRunSyscall);
    Q_WAIT_BLOCKED = Quarks()->StrQuark(kStateWaitBlocked);
    Q_INTERRUPTED = Quarks()->StrQuark(kStateInterrupted);
    Q_WAIT_FOR_CPU = Quarks()->StrQuark(kStateWaitForCpu);
    Q_UNKNOWN = Quarks()->StrQuark(kStateUnknown);
}

void CriticalBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    AddKernelObserver(notificationCenter, Token("sched_ttwu"), base::BindObject(&CriticalBlock::OnTTWU, this));
    AddKernelObserver(notificationCenter, Token("softirq_entry"), base::BindObject(&CriticalBlock::OnSoftIrqEntry, this));
    AddKernelObserver(notificationCenter, Token("softirq_exit"), base::BindObject(&CriticalBlock::OnSoftIrqExit, this));
    AddKernelObserver(notificationCenter, Token("irq_handler_entry"), base::BindObject(&CriticalBlock::OnIrqHandlerEntry, this));
    AddKernelObserver(notificationCenter, Token("irq_handler_exit"), base::BindObject(&CriticalBlock::OnIrqHandlerExit, this));
    AddKernelObserver(notificationCenter, Token("hrtimer_expire_entry"), base::BindObject(&CriticalBlock::OnHrtimerExpireEntry, this));
    AddKernelObserver(notificationCenter, Token("hrtimer_expire_exit"), base::BindObject(&CriticalBlock::OnHrtimerExpireExit, this));
    AddThreadStateObserver(notificationCenter, Token(kStateStatus), base::BindObject(&CriticalBlock::OnThreadStatus, this));
}

void CriticalBlock::OnTTWU(const trace::EventValue& event)
{
    uint32_t source_cpu = GetEventCPU(event);
    uint32_t source_tid = ThreadForCPU(source_cpu);
    uint32_t target_tid = event.getEventField("tid")->AsUInteger();

    auto& source_cpu_context = _context[source_cpu];

    if (!source_cpu_context.empty())
    {
        OnWakeupFromInterrupt(source_cpu_context.top(), target_tid);
    }
    else if (source_tid != kInvalidThread && target_tid != kInvalidThread)
    {
        OnTTWUBetweenThreads(source_tid, target_tid);
    }
}

void CriticalBlock::OnSoftIrqEntry(const trace::EventValue& event)
{
    uint32_t cpu = GetEventCPU(event);
    auto context = ResolveSoftIRQ(event.getEventField("vec")->AsUInteger());
    _context[cpu].push(context);
}

void CriticalBlock::OnSoftIrqExit(const trace::EventValue& event)
{
    uint32_t cpu = GetEventCPU(event);
    auto context = ResolveSoftIRQ(event.getEventField("vec")->AsUInteger());

    auto& cpu_context_stack = _context[cpu];
    if (cpu_context_stack.empty() || cpu_context_stack.top() != context)
    {
        tberror() << "Unexpected soft irq exit." << tbendl();
        return;
    }

    cpu_context_stack.pop();
}

void CriticalBlock::OnIrqHandlerEntry(const trace::EventValue& event)
{
    uint32_t cpu = GetEventCPU(event);
    auto context = ResolveIRQ(event.getEventField("irq")->AsUInteger());
    _context[cpu].push(context);
}

void CriticalBlock::OnIrqHandlerExit(const trace::EventValue& event)
{
    uint32_t cpu = GetEventCPU(event);
    auto context = ResolveIRQ(event.getEventField("irq")->AsUInteger());

    auto& cpu_context_stack = _context[cpu];
    if (cpu_context_stack.empty() || cpu_context_stack.top() != context)
    {
        tberror() << "Unexpected irq handler exit." << tbendl();
        return;
    }

    cpu_context_stack.pop();
}

void CriticalBlock::OnHrtimerExpireEntry(const trace::EventValue& event)
{
    uint32_t cpu = GetEventCPU(event);
    _context[cpu].push(critical::kTimer);
}

void CriticalBlock::OnHrtimerExpireExit(const trace::EventValue& event)
{
    uint32_t cpu = GetEventCPU(event);

    auto& cpu_context_stack = _context[cpu];
    if (cpu_context_stack.empty() || cpu_context_stack.top() != critical::kTimer)
    {
        tberror() << "Unexpected hr timer expire exit." << tbendl();
        return;
    }

    cpu_context_stack.pop();
}

void CriticalBlock::OnTTWUBetweenThreads(uint32_t source_tid, uint32_t target_tid)
{
    // Cut the source thread.
    auto prevNodeSource = CriticalGraph()->GetLastNodeForThread(source_tid);
    if (prevNodeSource == nullptr)
    {
        tberror() << "There should be a node on the source thread of a TTWU ("
            << source_tid << " > " << target_tid << ")." << tbendl();
        return;
    }
    auto prevTypeSourceIt = _lastEdgeTypePerThread.find(source_tid);

    if (prevTypeSourceIt == _lastEdgeTypePerThread.end() &&
        prevTypeSourceIt->second != critical::kRun)
    {
        tberror() << "Unexpected edge type for source of TTWU ("
            << source_tid << " > " << target_tid << ")." << tbendl();
        return;
    }

    auto nextNodeSource = CriticalGraph()->CreateNode(source_tid);
    CriticalGraph()->CreateHorizontalEdge(prevTypeSourceIt->second, prevNodeSource, nextNodeSource);

    // Cut the target thread.
    auto prevNodeTarget = CriticalGraph()->GetLastNodeForThread(target_tid);
    if (prevNodeTarget == nullptr)
    {
        // It's possible that we haven't seen the target of the TTWU
        // in the state dump yet.
        return;
    }

    auto prevTypeTargetIt = _lastEdgeTypePerThread.find(target_tid);
    if (prevTypeTargetIt == _lastEdgeTypePerThread.end())
    {
        tberror() << "There should be a prev type for target of TTWU ("
            << source_tid << " > " << target_tid << ")." << tbendl();
        return;
    }
    if (prevTypeTargetIt->second == critical::kRun)
    {
        // TODO: Determine whether this is normal.
        //tberror() << "Waking up a thread that is already running ("
        //    << source_tid << " > " << target_tid << ")." << tbendl();
    }

    auto nextNodeTarget = CriticalGraph()->CreateNode(target_tid);
    CriticalGraph()->CreateHorizontalEdge(prevTypeTargetIt->second, prevNodeTarget, nextNodeTarget);

    // Create the wake-up edge.
    CriticalGraph()->CreateVerticalEdge(nextNodeSource, nextNodeTarget);
}

void CriticalBlock::OnWakeupFromInterrupt(critical::CriticalEdgeType type, uint32_t target_tid)
{
    auto look = _lastEdgeTypePerThread.find(target_tid);
    if (look == _lastEdgeTypePerThread.end() ||
        look->second != critical::kWaitBlocked)
    {
        return;
    }

    _lastEdgeTypePerThread[target_tid] = type;
}

void CriticalBlock::OnThreadStatus(
    uint32_t tid,
    const notification::Path& path,
    const value::Value* value)
{
    // Determine the new edge type.
    auto newStatusValue = value->GetField(kCurrentStateAttributeValueField);
    critical::CriticalEdgeType newEdgeType = critical::kUnknown;

    if (newStatusValue != nullptr)
    {
        quark::Quark qNewStatus(newStatusValue->AsUInteger());

        if (qNewStatus == Q_RUN_USERMODE || qNewStatus == Q_RUN_SYSCALL)
            newEdgeType = critical::kRun;
        else if (qNewStatus == Q_INTERRUPTED)
            newEdgeType = critical::kInterrupted;
        else if (qNewStatus == Q_WAIT_FOR_CPU)
            newEdgeType = critical::kWaitCpu;
        else if (qNewStatus == Q_WAIT_BLOCKED)
            newEdgeType = critical::kWaitBlocked;
    }

    // Get the last edge type for the thread.
    auto lookLastType = _lastEdgeTypePerThread.find(tid);
    if (lookLastType != _lastEdgeTypePerThread.end() &&
        lookLastType->second == newEdgeType)
    {
        return;
    }

    // Create the new node and add a link to it from the prev node.
    auto prevNode = CriticalGraph()->GetLastNodeForThread(tid);
    auto newNode = CriticalGraph()->CreateNode(tid);

    if (prevNode != nullptr && lookLastType != _lastEdgeTypePerThread.end())
        CriticalGraph()->CreateHorizontalEdge(lookLastType->second, prevNode, newNode);

    if (newStatusValue != nullptr)
    {
        // Keep track of the type of the next edge.
        _lastEdgeTypePerThread[tid] = newEdgeType;
    }
    else
    {
        // When the status is null, it means that the thread exited.
        _lastEdgeTypePerThread.erase(tid);
    }
}

uint32_t CriticalBlock::ThreadForCPU(uint32_t cpu) const
{
    auto thread = State()->CurrentThreadForCpu(cpu);
    return thread;
}

}  // namespace critical_blocks
}  // namespace tibee
