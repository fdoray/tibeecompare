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
#ifndef _TIBEE_IO_READSTREAM_HPP
#define _TIBEE_IO_READSTREAM_HPP

#include <fstream>
#include <string>

namespace tibee
{
namespace io
{

template <typename T>
void ReadStream(std::ifstream& in, T* value)
{
    in.read(reinterpret_cast<char*>(value), sizeof(*value));
}

inline void ReadStringFromStream(std::ifstream& in, std::string* value)
{
    // Read string size.
    size_t size;
    ReadStream(in, &size);

    if (size == 0)
    {
        *value = std::string();
        return;
    }

    // Read string content.
    std::vector<char> buffer(size);
    in.read(buffer.data(), buffer.size());

    *value = std::string(buffer.data(), buffer.size());
}

}
}

#endif // _TIBEE_IO_READSTREAM_HPP
