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
#include "server/handlers/HandleGetNodes.hpp"

#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "base/CompareConstants.hpp"
#include "base/Constants.hpp"
#include "base/print.hpp"
#include "critical/CriticalGraphReader.hpp"
#include "execution/Execution.hpp"
#include "execution/ExecutionsDb.hpp"
#include "execution/GetExecutionSegments.hpp"
#include "execution/StacksFromDisk.hpp"
#include "execution/StacksReader.hpp"
#include "quark/QuarkDatabase.hpp"

namespace tibee
{
namespace server
{
namespace handlers
{

namespace
{

namespace bfs = boost::filesystem;

using base::tberror;
using base::tbendl;

bool ReadRequest(mq::MessageDecoder* request,
                 std::vector<std::string>* executionIds,
                 size_t* executionIndex)
{
    assert(request != nullptr);
    assert(executionIds != nullptr);
    assert(executionIndex != nullptr);

    *executionIds = std::vector<std::string>(2);
    int executionIndexInt = 0;
    if (!request->ReadString(&executionIds->at(0), kOIDLength) ||
        !request->ReadString(&executionIds->at(1), kOIDLength) ||
        !request->Read(&executionIndexInt))
    {
        return false;
    }
    *executionIndex = static_cast<size_t>(executionIndexInt);

    return true;
}

void CollectNodeTypesCallback(
    const execution::StackItem& item,
    quark::DiskQuarkDatabase* quarks,
    quark::QuarkDatabase<std::string>* types)
{
    types->Insert(quarks->String(item.name()));
}

void CollectNodeTypes(
    const execution::Stacks& stacks,
    const execution::ExecutionSegments& segments,
    quark::DiskQuarkDatabase* quarks,
    quark::QuarkDatabase<std::string>* types)
{
    types->Insert("");

    for (const auto& segment : segments)
    {
        stacks.EnumerateStacks(
            segment.thread(),
            containers::Interval(segment.startTs(), segment.endTs()),
            std::bind(&CollectNodeTypesCallback,
                      std::placeholders::_1, quarks, types));
    }
}

void WriteNodeTypes(const quark::QuarkDatabase<std::string>& nodeTypeDb,
                    mq::MessageEncoder* response)
{
    assert(response != nullptr);

    response->Write(static_cast<int>(nodeTypeDb.size()));
    for (const std::string* nodeType : nodeTypeDb)
        response->WriteString(*nodeType);
}

void WriteStackItemsCallback(
    const execution::StackItem& item,
    int32_t tid,
    timestamp_t startTs,
    timestamp_t endTs,
    quark::DiskQuarkDatabase* quarks,
    quark::QuarkDatabase<std::string>* types,
    int32_t* numNodes,
    mq::MessageEncoder* response)
{
    int64_t startTime = std::max(startTs, item.start());
    int64_t endTime = std::min(endTs, item.end());
    int64_t duration = endTime - startTime;
    int64_t extraDuration = 0;
    int32_t matchCode = 0;
    quark::Quark typeQuark = types->Insert(quarks->String(item.name()));
    int32_t depth = item.depth();
    int32_t parentNodeId = 0;
    int32_t childIndex = 0;

    response->Write(startTime);
    response->Write(duration);
    response->Write(extraDuration);
    response->Write(matchCode);
    response->Write(static_cast<int32_t>(typeQuark.get()));
    response->Write(tid);
    response->Write(depth);
    response->Write(parentNodeId);
    response->Write(childIndex);

    ++(*numNodes);
}

void WriteStackItems(
    const execution::ExecutionSegments& segments,
    const execution::Stacks& stacks,
    quark::DiskQuarkDatabase* quarks,
    quark::QuarkDatabase<std::string>* types,
    mq::MessageEncoder* response)
{
    // Placeholder for number of nodes.
    auto numNodesPos = response->pos();
    response->Write(static_cast<int32_t>(0));
    int32_t numNodes = 0;

    for (const auto& segment : segments)
    {
        stacks.EnumerateStacks(
            segment.thread(),
            containers::Interval(segment.startTs(), segment.endTs()),
            std::bind(&WriteStackItemsCallback,
                      std::placeholders::_1,
                      segment.thread(),
                      segment.startTs(),
                      segment.endTs(),
                      quarks,
                      types,
                      &numNodes,
                      response));
    }

    // Update number of nodes.
    response->WriteAtPos(numNodes, numNodesPos);
}

void WriteLinks(
    const std::vector<execution::Link>& links,
    mq::MessageEncoder* response)
{
    int32_t numLinks = links.size();
    response->Write(numLinks);

    for (const auto& link : links)
    {
        int64_t start = link.sourceTs();
        int64_t duration = link.targetTs() - start;
        int32_t sourceThread = link.sourceThread();
        int32_t targetThread = link.targetThread();

        response->Write(start);
        response->Write(duration);
        response->Write(sourceThread);
        response->Write(targetThread);
    }
}

void WriteThreadStatus(
    const execution::Execution& execution,
    const execution::ExecutionSegments& segments,
    mq::MessageEncoder* response)
{
    // Collect tids for this execution.
    std::unordered_set<thread_t> tids;
    for (const auto& segment : segments)
        tids.insert(segment.thread());

    // Read critical graph.
    bfs::path criticalFileName =
        bfs::path(kHistoryDirectoryName) / (execution.trace() + kCriticalFileName);
    critical::CriticalGraphSegments graph;
    if (!critical::ReadCriticalGraph(criticalFileName.string(), &graph))
    {
        tberror() << "Unable to read critical graph from disk." << tbendl();
        return;
    }

    // Write number of threads.
    response->Write(static_cast<int32_t>(tids.size()));

    // Write status for each thread.
    for (const auto& tid : tids)
    {
        auto& threadSegments = graph[tid];

        // Write tid.
        response->Write(static_cast<int32_t>(tid));

        // Write number of segments.
        auto numSegmentsPos = response->pos();
        response->Write(0);

        // Write segments.
        // TODO: Binary search to find the first interval.
        uint32_t numSegments = 0;
        for (size_t i = 0; i < threadSegments.size(); ++i)
        {
            timestamp_t start = threadSegments[i].ts;
            timestamp_t end = threadSegments[i].ts;
            if (i < threadSegments.size() - 1)
                end = threadSegments[i + 1].ts;

            if (end < execution.startTs())
                continue;
            if (start > execution.endTs())
                break;

            start = std::max(execution.startTs(), start);
            end = std::min(execution.endTs(), end);

            response->Write(static_cast<int64_t>(start));
            response->Write(static_cast<int64_t>(end));
            response->Write(static_cast<int32_t>(threadSegments[i].type));

            ++numSegments;
        }

        // Update number of segments.
        response->WriteAtPos(numSegments, numSegmentsPos);
    }
}

void WriteCriticalPath(mq::MessageEncoder* response)
{
    // Write critical path header to response.
    response->Write(static_cast<int32_t>(0));
    response->Write(static_cast<int64_t>(0));
}

}  // namespace

bool HandleGetNodes(mq::MessageDecoder* request,
                    mq::MessageEncoder* response)
{
    std::cout << "receive request" << std::endl;

    // Read the request.
    std::vector<std::string> executionIds;
    size_t executionIndex = 0;
    if (!ReadRequest(request, &executionIds, &executionIndex))
    {
        tberror() << "Unable to read GetNodes request." << tbendl();
        return false;
    }

    // Read the executions.
    std::vector<execution::Execution> executions(executionIds.size());
    quark::DiskQuarkDatabase quarks(kDiskQuarkDatabaseFile);
    execution::ExecutionsDb executionsDb(&quarks);
    std::unordered_map<std::string, execution::StacksFromDisk> stacks;
    std::vector<execution::ExecutionSegments> segments(executionIds.size());
    std::vector<std::vector<execution::Link>> links(executionIds.size());

    for (size_t i = 0; i < executionIds.size(); ++i)
    {
        // Read the execution from database.
        executionsDb.ReadExecution(
            execution::ExecutionId(executionIds[i]), &executions[i]);

        // Read the stacks of the trace from disk.
        auto look = stacks.find(executions[i].trace());
        if (look == stacks.end())
        {
            bfs::path stacksFileName =
                bfs::path(kHistoryDirectoryName) / (executions[i].trace() + kStacksFileName);
            ReadStacks(stacksFileName.string(), &stacks[executions[i].trace()]);
        }

        // Extract execution segments.
        GetExecutionSegments(
            executions[i],
            stacks[executions[i].trace()],
            &links[i],
            &segments[i]);
    }

    // Write the response: thread names.
    std::unordered_set<thread_t> tids;
    for (const auto& segment : segments[executionIndex])
        tids.insert(segment.thread());
    
    response->Write(static_cast<int32_t>(tids.size()));
    for (thread_t tid : tids)
    {
        response->Write(static_cast<int32_t>(tid));
        response->WriteString(
            stacks[executions[executionIndex].trace()].GetThread(tid).name());
    }

    // Collect node types.
    quark::QuarkDatabase<std::string> types;
    for (size_t i = 0; i < executionIds.size(); ++i)
    {
        CollectNodeTypes(
            stacks[executions[i].trace()],
            segments[i],
            &quarks,
            &types);
    }

    // Write the response: node types.
    WriteNodeTypes(types, response);

    // Match the graphs.
    // TODO

    // Write the response: stack items.
    WriteStackItems(
        segments[executionIndex], 
        stacks[executions[executionIndex].trace()],
        &quarks,
        &types,
        response);

    // Write the response: links.
    WriteLinks(links[executionIndex], response);

    // Write the response: thread status.
    WriteThreadStatus(
        executions[executionIndex],
        segments[executionIndex],
        response);

    // Write the response: critical path.
    WriteCriticalPath(response);

    std::cout << "sent response" << std::endl;

    return true;
}

}  // namespace handlers
}  // namespace server
}  // namespace tibee
