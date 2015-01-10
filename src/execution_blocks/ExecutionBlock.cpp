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
#include "execution_blocks/ExecutionBlock.hpp"

#include "base/BindObject.hpp"
#include "base/CompareConstants.hpp"
#include "base/Constants.hpp"
#include "block/ServiceList.hpp"
#include "notification/NotificationCenter.hpp"
#include "notification/Token.hpp"
#include "trace_blocks/TraceBlock.hpp"

namespace tibee {
namespace execution_blocks {

using notification::Token;
using trace_blocks::TraceBlock;

ExecutionBlock::ExecutionBlock()
{
}

ExecutionBlock::~ExecutionBlock()
{
}

void ExecutionBlock::RegisterServices(block::ServiceList* serviceList)
{
    serviceList->AddService(kStacksBuilderServiceName, &_stacksBuilder);
}

void ExecutionBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    notificationCenter->AddObserver(
        {Token(kTraceNotificationPrefix), Token(kTimestampNotificationName)},
        base::BindObject(&ExecutionBlock::onTimestamp, this));
    notificationCenter->AddObserver(
        {Token(kTraceNotificationPrefix), Token(kEndNotificationName)},
        base::BindObject(&ExecutionBlock::onEnd, this));
}

void ExecutionBlock::onTimestamp(const notification::Path& path, const value::Value* value)
{
    _stacksBuilder.SetTimestamp(value->AsULong());
}

void ExecutionBlock::onEnd(const notification::Path& path, const value::Value* value)
{  
    _stacksBuilder.Terminate();

    // Handle the completed executions...
}


}  // namespace execution_blocks
}  // namespace tibee