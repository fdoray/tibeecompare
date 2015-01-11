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
#include "critical/CriticalGraphReader.hpp"

#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>

#include "base/Constants.hpp"

namespace tibee
{
namespace critical
{

namespace bfs = boost::filesystem;

bool ReadCriticalGraph(
    const std::string& filename,
    CriticalGraphSegments* graph)
{
    // Open critical graph file.
    std::ifstream in;
    in.open(filename, std::ios::in | std::ios::binary);

    // Read number of threads.
    uint32_t numThreads = 0;
    in.read(reinterpret_cast<char*>(&numThreads), sizeof(numThreads));

    // Read each thread.
    for (size_t i = 0; i < numThreads; ++i)
    {
        // Read tid.
        uint32_t tid = 0;
        in.read(reinterpret_cast<char*>(&tid), sizeof(tid));

        // Read number of segments for this thread.
        uint32_t numSegments = 0;
        in.read(reinterpret_cast<char*>(&numSegments), sizeof(numSegments));

        // Create vector to hold the segments of this thread.
        auto& segments = (*graph)[tid];

        // Read segments for this thread.
        for (size_t j = 0; j < numSegments; ++j)
        {
            CriticalGraphSegment segment;
            in.read(reinterpret_cast<char*>(&segment), sizeof(segment));

            segments.push_back(segment);
        }
    }

    // Close critical path file.
    in.close();

    return true;
}

}  // namespace critical
}  // namespace tibee
