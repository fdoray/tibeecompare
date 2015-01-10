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
#include "execution/StacksReader.hpp"

#include <assert.h>
#include <fstream>

#include "io/ReadStream.hpp"

namespace tibee
{
namespace execution
{

namespace
{

void ReadStackItem(std::ifstream& in,
                   thread_t thread,
                   StacksFromDisk* stacks)
{
    StackItem item;

    // Read info.
    std::string name;
    io::ReadStringFromStream(in, &name);
    item.set_name(name);

    size_t depth = 0;
    io::ReadStream(in, &depth);
    item.set_depth(depth);

    timestamp_t start = 0;
    io::ReadStream(in, &start);
    item.set_start(start);

    timestamp_t end = 0;
    io::ReadStream(in, &end);
    item.set_end(end);

    // Read numeric properties.
    size_t numNumericProperties = 0;
    io::ReadStream(in, &numNumericProperties);

    for (size_t i = 0; i < numNumericProperties; ++i)
    {
        quark::Quark propertyKey;
        io::ReadStream(in, &propertyKey);

        uint64_t propertyValue;
        io::ReadStream(in, &propertyValue);

        item.SetNumericProperty(propertyKey, propertyValue);
    }

    // Read string properties.
    size_t numStringProperties = 0;
    io::ReadStream(in, &numStringProperties);

    for (size_t i = 0; i < numStringProperties; ++i)
    {
        quark::Quark propertyKey;
        io::ReadStream(in, &propertyKey);

        std::string propertyValue;
        io::ReadStringFromStream(in, &propertyValue);

        item.SetStringProperty(propertyKey, propertyValue);
    }

    stacks->AddStackItem(thread, item);
}

}  // namespace

bool ReadStacks(const std::string& filename,
                StacksFromDisk* stacks)
{
    assert(stacks != nullptr);

    std::ifstream in;
    in.open(filename, std::ios::in | std::ios::binary);

    // Read number of threads.
    size_t numThreads = 0;
    io::ReadStream(in, &numThreads);

    // Read threads with their stacks.
    for (size_t i = 0; i < numThreads; ++i)
    {
        // Read thread info.
        thread_t thread = -1;
        io::ReadStream(in, &thread);

        std::string name;
        io::ReadStringFromStream(in, &name);

        Thread threadDesc(thread);
        threadDesc.set_name(name);

        stacks->AddThread(threadDesc);

        // Read number of stack items for this thread.
        size_t numStackItems = 0;
        io::ReadStream(in, &numStackItems);

        // Read the stack items.
        for (size_t j = 0; j < numStackItems; ++j)
            ReadStackItem(in, thread, stacks);
    }

    // Read number of links.
    size_t numLinks = 0;
    io::ReadStream(in, &numLinks);

    // Read links.
    for (size_t i = 0; i < numLinks; ++i)
    {
        Link link;
        io::ReadStream(in, &link);
        stacks->AddLink(link);
    }

    return true;
}

}  // namespace execution
}  // namespace tibee
