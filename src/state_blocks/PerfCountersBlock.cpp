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
#include "state_blocks/PerfCountersBlock.hpp"

#include "base/BindObject.hpp"
#include "base/CompareConstants.hpp"
#include "base/Constants.hpp"
#include "notification/NotificationCenter.hpp"
#include "notification/Token.hpp"

namespace tibee
{
namespace state_blocks
{

namespace
{

std::string GetPerfFieldName(const std::string& type, const std::string& str)
{
    std::string cleaned(str);
    for (size_t i = 0; i < cleaned.size(); ++i)
    {
        if (cleaned[i] == '-')
            cleaned[i] = '_';
    }
    return std::string("perf_") + type + "_" + cleaned;
}

}  // namespace

PerfCountersBlock::PerfCountersBlock()
    : _ustInitialized(false), _kernelInitialized(false)
{
}

PerfCountersBlock::~PerfCountersBlock()
{
}

void PerfCountersBlock::LoadServices(const block::ServiceList& serviceList)
{
    using notification::Token;

    AbstractBuildBlock::LoadServices(serviceList);

    // Get path for thread states.
    state::AttributePathStr threadStatePath {kStateLinux, kStateThreads};
    _threadStateKey = State()->GetAttributeKeyStr(threadStatePath);
}

void PerfCountersBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    using notification::AnyToken;
    using notification::Token;

    AddKernelObserver(notificationCenter,
                      Token("sched_switch"),
                      base::BindObject(&PerfCountersBlock::OnSchedSwitchEvent, this));
    AddKernelObserver(notificationCenter,
                      Token("sched_ttwu"),
                      base::BindObject(&PerfCountersBlock::OnTTWUEvent, this));
    AddUstObserver(notificationCenter,
                   AnyToken(),
                   base::BindObject(&PerfCountersBlock::OnUstEvent, this));
}

void PerfCountersBlock::InitializeKernel(const trace::EventValue& event)
{
    if (_kernelInitialized)
        return;

    for (size_t i = 0; i < kNumPerformanceCounters; ++i) {
        PerfCounter counter;
        counter.name = GetPerfFieldName("cpu", kPerformanceCounters[i]);
        auto* value = event.getStreamEventContext()->GetField(counter.name);
        if (value != nullptr) {
            counter.quark = Quarks()->StrQuark(kPerformanceCounters[i]);
            _kernelCounters.push_back(counter);
        }
    }

    _kernelInitialized = true;
}

void PerfCountersBlock::InitializeUst(const trace::EventValue& event)
{
    if (_ustInitialized)
        return;

    for (size_t i = 0; i < kNumPerformanceCounters; ++i) {
        PerfCounter counter;
        counter.name = GetPerfFieldName("thread", kPerformanceCounters[i]);
        auto* value = event.getStreamEventContext()->GetField(counter.name);

        if (value != nullptr) {
            counter.quark = Quarks()->StrQuark(kPerformanceCounters[i]);
            _ustCounters.push_back(counter);
        }
    }

    _ustInitialized = true;
}

void PerfCountersBlock::OnSchedSwitchEvent(const trace::EventValue& event)
{
    InitializeKernel(event);

    auto prevTid = event.getFields()->GetField("prev_tid")->AsInteger();
    auto nextTid = event.getFields()->GetField("next_tid")->AsInteger();

    for (const auto& counter : _kernelCounters)
    {
        auto* value = event.getStreamEventContext()->GetField(counter.name);
        if (value == nullptr)
            continue;
        uint64_t uLongValue = value->AsULong();

        auto prevCounterKey = State()->GetAttributeKey(
            _threadStateKey, {Quarks()->IntQuark(prevTid), counter.quark});
        StateHistory()->SetPerfCounterCpuValue(prevCounterKey, uLongValue);

        auto nextCounterKey = State()->GetAttributeKey(
            _threadStateKey, {Quarks()->IntQuark(nextTid), counter.quark});
        StateHistory()->SetPerfCounterCpuBaseValue(nextCounterKey, uLongValue);
    }
}

void PerfCountersBlock::OnTTWUEvent(const trace::EventValue& event)
{
    InitializeKernel(event);

    auto thread = ThreadForEvent(event);

    for (const auto& counter : _kernelCounters)
    {
        auto* value = event.getStreamEventContext()->GetField(counter.name);
        if (value == nullptr)
            continue;
        uint64_t uLongValue = value->AsULong();

        auto counterKey = State()->GetAttributeKey(
            _threadStateKey, {Quarks()->IntQuark(thread), counter.quark});
        StateHistory()->SetPerfCounterCpuValue(counterKey, uLongValue);
    }
}

void PerfCountersBlock::OnUstEvent(const trace::EventValue& event)
{
    InitializeUst(event);

    auto thread = ThreadForEvent(event);

    for (const auto& counter : _ustCounters)
    {
        auto* value = event.getStreamEventContext()->GetField(counter.name);
        if (value == nullptr)
            continue;
        uint64_t uLongValue = value->AsULong();

        auto counterKey = State()->GetAttributeKey(_threadStateKey,
                { Quarks()->IntQuark(thread), counter.quark });
        StateHistory()->SetPerfCounterThreadValue(counterKey, uLongValue);
    }
}

}  // namespace state_blocks
}  // namespace tibee
