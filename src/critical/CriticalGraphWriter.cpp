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
#include "critical/CriticalGraphWriter.hpp"

#include <boost/filesystem.hpp>
#include <fstream>

#include "base/Constants.hpp"
#include "critical/CriticalGraphSegment.hpp"

namespace tibee
{
namespace critical
{

namespace bfs = boost::filesystem;

bool WriteCriticalGraph(
    const std::string& filename,
    const critical::CriticalGraph& critical_graph)
{
    // Open critical path file.
    std::ofstream out;
    out.open(filename, std::ios::out | std::ios::binary);

    // Write critical graph file header.
    // - Number of threads.
    uint32_t numThreads = critical_graph.NumThreads();
    out.write(reinterpret_cast<const char*>(&numThreads), sizeof(numThreads));

    // Write each thread.
    auto threads_it = critical_graph.threads_begin();
    auto threads_it_end = critical_graph.threads_end();

    for (; threads_it != threads_it_end; ++threads_it)
    {
        // Write tid.
        uint32_t tid = threads_it->first;
        out.write(reinterpret_cast<const char*>(&tid), sizeof(tid));

        // Write number of segments for this thread.
        uint32_t numSegments = threads_it->second->size();
        out.write(reinterpret_cast<const char*>(&numSegments), sizeof(numSegments));

        // Write segments for this thread.
        for (size_t i = 0; i < numSegments; ++i)
        {
            const critical::CriticalNode* node = threads_it->second->at(i);

            if (i == numSegments - 1)
            {
                CriticalGraphSegment segment(node->ts(), critical::CriticalEdgeType::kUnknown);
                out.write(reinterpret_cast<const char*>(&segment), sizeof(segment));
            }
            else
            {
                critical::CriticalEdgeType type = critical::CriticalEdgeType::kUnknown;
                if (node->edge(critical::kCriticalEdgeOutHorizontal) != critical::kInvalidCriticalEdgeId)
                    type = critical_graph.GetEdge(node->edge(critical::kCriticalEdgeOutHorizontal)).type();
                CriticalGraphSegment segment(node->ts(), type);
                out.write(reinterpret_cast<const char*>(&segment), sizeof(segment));
            }
        }
    }

    // Close critical graph file.
    out.close();

    return true;
}

}  // namespace critical
}  // namespace tibee
