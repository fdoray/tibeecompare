/* Copyright (c) 2015 Francois Doray <francois.pierre-doray@polymtl.ca>
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
#ifndef _TIBEE_DB_READBUFFER_HPP
#define _TIBEE_DB_READBUFFER_HPP

#include <leveldb/slice.h>
#include <vector>

namespace tibee
{
namespace db
{

template<typename T>
bool ReadBuffer(const leveldb::Slice& buffer, size_t* pos, T* value)
{
    if (*pos + sizeof(*value) > buffer.size())
        return false;
    memcpy(value, buffer.data() + *pos, sizeof(*value));
    *pos += sizeof(*value);
    return true;
}

}  // namespace db
}  // namespace tibee

#endif // _TIBEE_DB_READBUFFER_HPP
