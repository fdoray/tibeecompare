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
#include "base/BasicTypes.hpp"

#include <memory>

namespace tibee
{
namespace execution
{

const timestamp_t kInvalidTs = -1;
const uint32_t kInvalidLevel = -1;

struct Vertex
{
    typedef std::unique_ptr<Vertex> UP;

    Vertex(thread_t thread, timestamp_t ts)
        : thread(thread), ts(ts),
          hout(nullptr), hin(nullptr),
          vout(nullptr), vin(nullptr),
          level(kInvalidLevel) {}

    thread_t thread;
    timestamp_t ts;

    Vertex* hout;
    Vertex* hin;
    Vertex* vout;
    Vertex* vin;

    uint32_t level;
};

typedef std::vector<Vertex::UP> Vertices;
typedef std::unordered_map<thread_t, Vertices> VerticesPerThread;

}  // namespace execution
}  // namespace tibee
