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
#ifndef _TIBEE_IO_WRITESTREAM_HPP
#define _TIBEE_IO_WRITESTREAM_HPP

#include <iostream>

#include <fstream>
#include <string>

namespace tibee
{
namespace io
{

template <typename T>
void WriteStream(std::ofstream& out, const T& value)
{
    out.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

inline void WriteStringToStream(std::ofstream& out, const std::string& value)
{
    // Write string size.
    WriteStream(out, value.size());

    // Write string content.
    if (!value.empty())
        out.write(value.data(), value.size());
}

}
}

#endif // _TIBEE_IO_WRITESTREAM_HPP
