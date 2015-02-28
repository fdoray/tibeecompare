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
#include "build_blocks/BuildBlock.hpp"

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
#include "critical/ComputeCriticalPath.hpp"
#include "execution/ExtractMetrics.hpp"
#include "execution/ExtractStacks.hpp"
#include "notification/NotificationCenter.hpp"
#include "notification/Token.hpp"

namespace tibee {
namespace build_blocks {

namespace
{

namespace bfs = boost::filesystem;

using base::tbinfo;
using base::tberror;
using base::tbendl;
using notification::Token;

// Intervals at which executions are saved (ns).
const timestamp_t kSaveInterval = 10000000000;  // 10 seconds

}  // namespace

BuildBlock::BuildBlock(bool stats)
    : _quarks(nullptr), _currentState(nullptr), _stats(stats),
	  _saveTs(0), _lastCleanupTs(0), _numExecutions(0)
{
    _traceId = boost::lexical_cast<std::string>(
        boost::uuids::uuid(boost::uuids::random_generator()()));

    _stacksBuilder.SetDatabase(&_db);
}

BuildBlock::~BuildBlock()
{
}

void BuildBlock::RegisterServices(block::ServiceList* serviceList)
{
    serviceList->AddService(kExecutionsBuilderServiceName, &_executionsBuilder);
    serviceList->AddService(kStacksBuilderServiceName, &_stacksBuilder);
    serviceList->AddService(kCriticalGraphServiceName, &_criticalGraph);
    serviceList->AddService(kStateHistoryServiceName, &_stateHistory);
}

void BuildBlock::LoadServices(const block::ServiceList& serviceList)
{
    serviceList.QueryService(kQuarksServiceName,
                             reinterpret_cast<void**>(&_quarks));
    serviceList.QueryService(kCurrentStateServiceName,
                             reinterpret_cast<void**>(&_currentState));
}

void BuildBlock::AddObservers(notification::NotificationCenter* notificationCenter)
{
    notificationCenter->AddObserver(
        {Token(kTraceNotificationPrefix), Token(kTimestampNotificationName)},
        base::BindObject(&BuildBlock::onTimestamp, this));
    notificationCenter->AddObserver(
        {Token(kTraceNotificationPrefix), Token(kEndNotificationName)},
        base::BindObject(&BuildBlock::onEnd, this));
}

void BuildBlock::onTimestamp(const notification::Path& path, const value::Value* value)
{
    if (_stats)
        return;

    auto ts = value->AsULong();
    _executionsBuilder.SetTimestamp(ts);
    _stacksBuilder.SetTimestamp(ts);
    _criticalGraph.SetTimestamp(ts);
    _stateHistory.SetTimestamp(ts);

    if (_saveTs == 0)
    {
        _saveTs = ts;
    }
    else if (ts > _saveTs + kSaveInterval)
    {
        SaveExecutions();

        // Clean everything that is |kSaveInterval| old.
        tbinfo() << "Cleaning the history." << tbendl();
        _stacksBuilder.Cleanup(_saveTs);
        _criticalGraph.Cleanup(_saveTs);
        _stateHistory.Cleanup(_saveTs);

        tbinfo() << "Continuing to read the trace." << tbendl();

        _lastCleanupTs = _saveTs;
        _saveTs = ts;
    }
}

void BuildBlock::onEnd(const notification::Path& path, const value::Value* value)
{
    if (_stats)
        return;

	tbinfo() << "Completed reading the trace." << tbendl();
	SaveExecutions();
	tbinfo() << "A total of " << _numExecutions << " executions were added to the database." << tbendl();
}

void BuildBlock::SaveExecutions()
{
    tbinfo() << "Saving current executions to the database." << tbendl();

    // Notify the executions and stacks builder that we reached
    // the end of the trace.
    _executionsBuilder.Terminate();
    _stacksBuilder.Terminate();

    // Save all executions in the database.
    for (auto& execution : _executionsBuilder)
    {
        if (execution->startTs() < _lastCleanupTs) {
            tberror() << "Skipping an execution because it starts before the last cleanup ts." << tbendl();
            tberror() << "  Execution start ts: " << execution->startTs() << tbendl();
            continue;
        }

        // Compute the critical path of the execution.
        critical::CriticalPath criticalPath;
        critical::ComputeCriticalPath(
            _criticalGraph, execution->startTs(), execution->endTs(),
            execution->startThread(), &criticalPath);

        // Extract the stacks that belong to the execution.
        execution::ExtractStacks(
            criticalPath, _stacksBuilder, _criticalGraph, _stateHistory,
            _currentState, &_db, execution.get());

        // Extract execution metrics.
        execution::ExtractMetrics(criticalPath, execution.get());

        // Add the execution to the database.
        _db.AddExecution(*execution);

        ++_numExecutions;
    }

    // Flush saved executions.
    _executionsBuilder.Flush();
}

}  // namespace build_blocks
}  // namespace tibee
