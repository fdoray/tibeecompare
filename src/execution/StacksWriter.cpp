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
#include "execution/StacksWriter.hpp"

#include <fstream>
#include <vector>

#include "base/Inserter.hpp"
#include "io/WriteStream.hpp"

namespace tibee
{
namespace execution
{

namespace
{

void WriteStackItem(std::ofstream& out,
                    const StackItem& item)
{
    // Write info.
    io::WriteStream(out, item.name());
    io::WriteStream(out, item.depth());
    io::WriteStream(out, item.start());
    io::WriteStream(out, item.end());
}

void WriteLink(std::ofstream& out,
                const Link& link)
{
    io::WriteStream(out, link);
}

}  // namespace

bool WriteStacks(const std::string& filename,
                 const Stacks& stacks)
{
    namespace pl = std::placeholders;

    std::ofstream out;
    out.open(filename, std::ios::out | std::ios::binary);

    std::vector<Thread> threads;
    stacks.EnumerateThreads(base::BackInserter(&threads));

    // Write number of threads.
    io::WriteStream(out, threads.size());

    // Write threads with their stacks.
    for (const auto& thread : threads)
    {
        // Write thread info.
        io::WriteStream(out, thread.thread());
        io::WriteStringToStream(out, thread.name());

        // Write number of stack items for this thread.
        io::WriteStream(out, stacks.StackItemsCount(thread.thread()));

        // Write the stack items.
        stacks.EnumerateStacks(
            thread.thread(), containers::Interval(0, -1),
            std::bind(&WriteStackItem, std::ref(out), pl::_1));
    }

    // Write number of links.
    io::WriteStream(out, stacks.LinksCount());

    // Write links.
    stacks.EnumerateLinks(
        containers::Interval(0, -1),
        std::bind(&WriteLink, std::ref(out), pl::_1));

    return true;
}

}  // namespace execution
}  // namespace tibee
