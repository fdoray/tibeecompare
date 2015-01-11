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
#include "execution/ExecutionsDb.hpp"
#include "execution/StacksWriter.hpp"
#include "notification/NotificationCenter.hpp"
#include "notification/Token.hpp"

namespace tibee {
namespace execution_blocks {

namespace bfs = boost::filesystem;

using base::tbinfo;
using base::tberror;
using base::tbendl;
using notification::Token;

ExecutionBlock::ExecutionBlock()
{
    _traceId = boost::lexical_cast<std::string>(
        boost::uuids::uuid(boost::uuids::random_generator()()));
}

ExecutionBlock::~ExecutionBlock()
{
}

void ExecutionBlock::RegisterServices(block::ServiceList* serviceList)
{
    serviceList->AddService(kExecutionsBuilderServiceName, &_executionsBuilder);
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
    _stacksBuilder.SetTimestamp(value->AsULong());
}

void ExecutionBlock::onEnd(const notification::Path& path, const value::Value* value)
{  
    _stacksBuilder.Terminate();

    bfs::path stacksFileName =
        bfs::path(kHistoryDirectoryName) / (_traceId + kStacksFileName);

    // Write stacks to disk.
    WriteStacks(stacksFileName.string(), _stacksBuilder);

    // Write executions to database.
    execution::ExecutionsDb executionsDb(_quarks);
    for (const auto& execution : _executionsBuilder)
    {
        execution::ExecutionId executionId;
        if (executionsDb.InsertExecution(*execution, &executionId))
        {
            tbinfo() << "Execution " << executionId <<
                        " with name " << execution->name() << tbendl();
        }
        else
        {
            tberror() << "Unable to insert execution with name " << execution->name() <<
                         "in database." << tbendl();
        }
    }

    // Handle the completed executions.
    // TODO.
}


}  // namespace execution_blocks
}  // namespace tibee