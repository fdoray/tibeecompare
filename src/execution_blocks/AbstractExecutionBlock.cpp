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
#include "execution_blocks/AbstractExecutionBlock.hpp"

#include "base/CompareConstants.hpp"
#include "base/Constants.hpp"
#include "block/ServiceList.hpp"

namespace tibee {
namespace execution_blocks {

AbstractExecutionBlock::AbstractExecutionBlock()
    : _currentState(nullptr),
      _quarks(nullptr),
      _executionsBuilder(nullptr),
      _linksBuilder(nullptr)
{
}

void AbstractExecutionBlock::LoadServices(const block::ServiceList& serviceList)
{
    serviceList.QueryService(kExecutionsBuilderServiceName,
                             reinterpret_cast<void**>(&_executionsBuilder));

    serviceList.QueryService(kLinksBuilderServiceName,
                             reinterpret_cast<void**>(&_linksBuilder));

    serviceList.QueryService(kCurrentStateServiceName,
                             reinterpret_cast<void**>(&_currentState));

    serviceList.QueryService(kQuarksServiceName,
                             reinterpret_cast<void**>(&_quarks));
}

uint32_t AbstractExecutionBlock::CpuForEvent(const trace::EventValue& event) const
{
    return event.getStreamPacketContext()->GetField("cpu_id")->AsUInteger();
}

thread_t AbstractExecutionBlock::ThreadForEvent(const trace::EventValue& event) const
{
    auto thread_context_value = event.getStreamEventContext()->GetField("vtid");
    if (thread_context_value != nullptr)
        return thread_context_value->AsUInteger();
    auto cpu = CpuForEvent(event);
    auto thread = State()->CurrentThreadForCpu(cpu);
    return thread;
}

thread_t AbstractExecutionBlock::ProcessForEvent(const trace::EventValue& event) const
{
    auto process_context_value = event.getStreamEventContext()->GetField("vpid");
    if (process_context_value != nullptr)
        return process_context_value->AsUInteger();
    // TODO: Get the process from the current state.
    return -1;
}

}  // namespace execution_blocks
}  // namespace tibee