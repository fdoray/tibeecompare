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
#include "build_blocks/AbstractBuildBlock.hpp"

#include "base/CompareConstants.hpp"
#include "base/Constants.hpp"
#include "block/ServiceList.hpp"

namespace tibee {
namespace build_blocks {

AbstractBuildBlock::AbstractBuildBlock()
    : _currentState(nullptr),
      _quarks(nullptr),
      _executionsBuilder(nullptr),
      _stacksBuilder(nullptr),
      _criticalGraph(nullptr)
{
}

void AbstractBuildBlock::LoadServices(const block::ServiceList& serviceList)
{
    serviceList.QueryService(kCurrentStateServiceName,
                             reinterpret_cast<void**>(&_currentState));

    serviceList.QueryService(kQuarksServiceName,
                             reinterpret_cast<void**>(&_quarks));

    serviceList.QueryService(kExecutionsBuilderServiceName,
                             reinterpret_cast<void**>(&_executionsBuilder));

    serviceList.QueryService(kStacksBuilderServiceName,
                             reinterpret_cast<void**>(&_stacksBuilder));

    serviceList.QueryService(kCriticalGraphServiceName,
                             reinterpret_cast<void**>(&_criticalGraph));
}

uint32_t AbstractBuildBlock::CpuForEvent(const trace::EventValue& event) const
{
    return event.getStreamPacketContext()->GetField("cpu_id")->AsUInteger();
}

thread_t AbstractBuildBlock::ThreadForEvent(const trace::EventValue& event) const
{
    auto thread_context_value = event.getStreamEventContext()->GetField("vtid");
    if (thread_context_value != nullptr)
        return thread_context_value->AsUInteger();
    auto cpu = CpuForEvent(event);
    auto thread = State()->CurrentThreadForCpu(cpu);
    return thread;
}

thread_t AbstractBuildBlock::ProcessForEvent(const trace::EventValue& event) const
{
    auto process_context_value = event.getStreamEventContext()->GetField("vpid");
    if (process_context_value != nullptr)
        return process_context_value->AsUInteger();
    // TODO: Get the process from the current state.
    return -1;
}

}  // namespace build_blocks
}  // namespace tibee
