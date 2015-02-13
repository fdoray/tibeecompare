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

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "base/BindObject.hpp"
#include "base/CompareConstants.hpp"
#include "base/Constants.hpp"
#include "base/print.hpp"
#include "block/ServiceList.hpp"
#include "execution/ExtractMetrics.hpp"
#include "execution/ExtractStacks.hpp"
#include "notification/NotificationCenter.hpp"
#include "notification/Token.hpp"

namespace tibee {
namespace execution_blocks {

namespace
{

namespace bfs = boost::filesystem;

using base::tbinfo;
using base::tberror;
using base::tbendl;
using notification::Token;

}  // namespace

ExecutionBlock::ExecutionBlock()
    : _quarks(nullptr)
{
    _traceId = boost::lexical_cast<std::string>(
        boost::uuids::uuid(boost::uuids::random_generator()()));

    _stacksBuilder.SetDatabase(&_db);
}

ExecutionBlock::~ExecutionBlock()
{
}

void ExecutionBlock::RegisterServices(block::ServiceList* serviceList)
{
    serviceList->AddService(kExecutionsBuilderServiceName, &_executionsBuilder);
    serviceList->AddService(kLinksBuilderServiceName, &_linksBuilder);
    serviceList->AddService(kStacksBuilderServiceName, &_stacksBuilder);
}

void ExecutionBlock::LoadServices(const block::ServiceList& serviceList)
{
    serviceList.QueryService(kQuarksServiceName,
                             reinterpret_cast<void**>(&_quarks));
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
    auto ts = value->AsULong();
    _executionsBuilder.SetTimestamp(ts);
    _linksBuilder.SetTimestamp(ts);
    _stacksBuilder.SetTimestamp(ts);
}

void ExecutionBlock::onEnd(const notification::Path& path, const value::Value* value)
{  
    // Notify the executions and stacks builder that we reached
    // the end of the trace.
    _executionsBuilder.Terminate();
    _stacksBuilder.Terminate();

    // Save all executions.
    for (auto& execution : _executionsBuilder)
    {
        /*
        // Extract the segments.
        // TODO: For now, we only have one segment per execution.
        critical::Segment segment(
            execution->startThread(), execution->startTs(),
            execution->endTs(), true);
        critical::Segments segments({segment});

        // Extract the stacks.
        execution::ExtractStacks(
            _stacksBuilder, segments, execution.get());
        */

        // Extract metrics.
        execution::ExtractMetrics(execution.get());

        // Add the execution to the database.
        _db.AddExecution(*execution);
    }
}

}  // namespace execution_blocks
}  // namespace tibee
