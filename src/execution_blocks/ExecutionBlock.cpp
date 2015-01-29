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

const char kDurationMetricName[] = "duration";

}  // namespace

ExecutionBlock::ExecutionBlock()
    : _quarks(nullptr)
{
    _traceId = boost::lexical_cast<std::string>(
        boost::uuids::uuid(boost::uuids::random_generator()()));
}

ExecutionBlock::~ExecutionBlock()
{
}

void ExecutionBlock::RegisterServices(block::ServiceList* serviceList)
{
    serviceList->AddService(kLinksBuilderServiceName, &_linksBuilder);
    serviceList->AddService(kExecutionsBuilderServiceName, &_executionsBuilder);
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
}

void ExecutionBlock::onEnd(const notification::Path& path, const value::Value* value)
{  
    /*
    // Notify the executions builder that we reached
    // the end of the trace.
    _executionsBuilder.Terminate();

    // Write executions to database.
    quark::Quark Q_DURATION = _quarks->StrQuark(kDurationMetricName);
    for (const auto& execution : _executionsBuilder)
    {
        // Collect all threads involved in the execution.
        std::unordered_set<thread_t> threads;
        for (const auto& segment : execution->segments())
            threads.insert(segment.thread());

        // Try to find more threads involved in the execution.
        execution::Segments extraSegments;
        for (const auto& segment : execution->segments())
        {
            GetSegments(segment, _linksBuilder, threads, &extraSegments);
        }
        for (const auto& segment : extraSegments)
        {
            execution->AddSegment(segment);
            threads.insert(segment.thread());
        }

        // Compute the critical path.
        critical::CriticalPath criticalPath;
        auto startCriticalNode = _criticalGraph->GetNodeIntersecting(
            execution->startTs(),
            execution->startThread());
        auto endCriticalNode = _criticalGraph->GetNodeIntersecting(
            execution->endTs(),
            execution->endThread());
        if (!_criticalGraph->ComputeCriticalPath(
                startCriticalNode, endCriticalNode,
                threads, &criticalPath))
        {
            tberror() << "Unable to compute critical path." << tbendl();
        }

        // Add metrics.
        timestamp_t duration = execution->endTs() - execution->startTs();
        execution->SetMetric(Q_DURATION, duration);
        metrics::ExtractCriticalMetrics(
            criticalPath,
            execution->endTs(),
            _quarks,
            execution.get());

        // Set trace id.
        execution->set_trace(_traceId);

        // Insert the execution in a database!
    }
    */
}

}  // namespace execution_blocks
}  // namespace tibee
