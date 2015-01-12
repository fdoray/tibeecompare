/* Copyright (c) 2014 Francois Doray <francois.pierre-doray@polymtl.ca>
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
#include "execution/GetExecutionSegments.hpp"

#include <queue>
#include <unordered_map>

#include "base/print.hpp"

namespace tibee
{
namespace execution
{

namespace
{

const timestamp_t kInvalidTs = -1;
const uint32_t kInvalidLevel = -1;

struct TimePoint
{
    typedef std::unique_ptr<TimePoint> UP;

    TimePoint(thread_t thread, timestamp_t ts)
        : thread(thread), ts(ts),
          hout(nullptr), hin(nullptr),
          vout(nullptr), vin(nullptr),
          level(kInvalidLevel) {}

    thread_t thread;
    timestamp_t ts;

    TimePoint* hout;
    TimePoint* hin;
    TimePoint* vout;
    TimePoint* vin;

    uint32_t level;
};

typedef std::vector<std::unique_ptr<TimePoint>> TimePoints;
typedef std::unordered_map<thread_t, TimePoints> TimePointsPerThread;

void CreateTimePointsInternal(
    const Link& link,
    TimePointsPerThread* timePointsPerThread)
{
    if (link.sourceThread() == link.targetThread())
    {
        base::tberror() << "Cannot have a link with the same source and "
                        << "target thread." << base::tbendl();
        return;
    }

    auto& sourceThread = (*timePointsPerThread)[link.sourceThread()];
    auto& targetThread = (*timePointsPerThread)[link.targetThread()];

    if (sourceThread.empty())
        return;

    TimePoint::UP sourceTimePoint(new TimePoint(link.sourceThread(), link.sourceTs()));
    TimePoint::UP targetTimePoint(new TimePoint(link.sourceThread(), link.sourceTs()));

    auto* sourcePrevTimePoint = sourceThread.back().get();
    sourcePrevTimePoint->hout = sourceTimePoint.get();
    sourceTimePoint->hin = sourcePrevTimePoint;

    if (!targetThread.empty())
    {
        auto* targetPrevTimePoint = targetThread.back().get();
        targetPrevTimePoint->hout = targetTimePoint.get();
        targetTimePoint->hin = targetPrevTimePoint;
    }

    sourceTimePoint->vout = targetTimePoint.get();
    targetTimePoint->vin = sourceTimePoint.get();

    sourceThread.push_back(std::move(sourceTimePoint));
    targetThread.push_back(std::move(targetTimePoint));
}

bool CreateTimePoints(
    const Execution& execution,
    const Stacks& stacks,
    TimePointsPerThread* timePointsPerThread,
    TimePoint** start,
    TimePoint** end)
{
    namespace pl = std::placeholders;

    // Add initial time point.
    TimePoint::UP startUP(new TimePoint(execution.startThread(), execution.startTs()));
    *start = startUP.get();
    (*timePointsPerThread)[execution.startThread()].push_back(std::move(startUP));

    // Follow links to create time points.
    stacks.EnumerateLinks(
        containers::Interval(execution.startTs(), execution.endTs()),
        std::bind(&CreateTimePointsInternal, pl::_1, timePointsPerThread));

    // Add final time point.
    auto& prevThread = (*timePointsPerThread)[execution.endThread()];
    if (prevThread.empty())
    {
        base::tberror() << "No previous time point on thread " << execution.endThread() << "." << base::tbendl();
        return false;
    }
    auto* prevTimePoint = prevThread.back().get();
    TimePoint::UP endUP(new TimePoint(execution.endThread(), execution.endTs()));
    *end = endUP.get();
    prevTimePoint->hout = endUP.get();
    endUP->hin = prevTimePoint;
    (*timePointsPerThread)[execution.endThread()].push_back(std::move(endUP));

    return true;
}

void AssignLevels(TimePoint* start)
{
    std::queue<TimePoint*> queue;
    queue.push(start);

    while (!queue.empty())
    {
        TimePoint* point = queue.front();
        queue.pop();

        uint32_t hlevel = kInvalidLevel;
        uint32_t vlevel = kInvalidLevel;

        if (point->hin != nullptr)
            hlevel = point->hin->level;
        if (point->vin != nullptr)
            vlevel = point->vin->level;

        if (hlevel == kInvalidLevel && vlevel == kInvalidLevel)
            point->level = 0;
        else if (hlevel == kInvalidLevel || vlevel < hlevel)
            point->level = vlevel + 1;
        else
            point->level = hlevel;

        if (point->hout != nullptr &&
            (point->hout->vin == nullptr || point->hout->vin->level != kInvalidLevel))
        {
            queue.push(point->hout);
        }

        if (point->vout != nullptr &&
            (point->vout->hin == nullptr || point->vout->hin->level != kInvalidLevel))
        {
            queue.push(point->vout);
        }
    }
}

void GetExecutionSegmentsInternal(
    const TimePointsPerThread& timePointsPerThread,
    std::vector<Link>* links,
    std::vector<ExecutionSegment>* executionSegments)
{
    for (const auto& threadTimePoints : timePointsPerThread)
    {
        thread_t thread = threadTimePoints.first;
        const auto& timePoints = threadTimePoints.second;

        timestamp_t startTs = kInvalidTs;
        size_t index = 0;

        for (const auto& timePoint : timePoints)
        {
            if (startTs == kInvalidTs)
            {
                startTs = timePoint->ts;
            }
            else if ((timePoint->hout != nullptr &&
                      timePoint->hout->vin != nullptr &&
                      timePoint->hout->vin->level < timePoint->hout->level) ||
                     index == timePoints.size() - 1)
            {
                ExecutionSegment executionSegment;
                executionSegment.set_thread(thread);
                executionSegment.set_startTs(startTs);
                executionSegment.set_endTs(timePoint->ts);
                executionSegments->push_back(executionSegment);

                startTs = kInvalidTs;
            }

            ++index;
        }
    }
}

}  // namespace

void GetExecutionSegments(
    const Execution& execution,
    const Stacks& stacks,
    std::vector<Link>* links,
    std::vector<ExecutionSegment>* executionSegments)
{
    // Create graph.
    TimePointsPerThread timePointsPerThread;
    TimePoint* start = nullptr;
    TimePoint* end = nullptr;
    if (!CreateTimePoints(execution, stacks, &timePointsPerThread, &start, &end))
        return;

    // Assign levels to the nodes of the graph.
    AssignLevels(start);

    // On each thread, find continuous segments without a
    // vertical-in link from a smaller level.
    GetExecutionSegmentsInternal(timePointsPerThread, links, executionSegments);
}

}  // namespace execution
}  // namespace tibee
