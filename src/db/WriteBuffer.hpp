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
#ifndef _TIBEE_DB_WRITEBUFFER_HPP
#define _TIBEE_DB_WRITEBUFFER_HPP

#include <vector>

namespace tibee
{
namespace db
{

template<typename T>
void WriteBuffer(const T& value, std::vector<char>* buffer)
{
    size_t pos = buffer->size();
    buffer->resize(buffer->size() + sizeof(value));
    memcpy(buffer->data() + pos, &value, sizeof(value));
}

void WriteStringToBuffer(const std::string& str, std::vector<char>* buffer)
{
    WriteBuffer(static_cast<uint32_t>(str.size()), buffer);
    size_t pos = buffer->size();
    buffer->resize(buffer->size() + str.size());
    memcpy(buffer->data() + pos, str.c_str(), str.size());
}

}  // namespace db
}  // namespace tibee

#endif // _TIBEE_DB_WRITEBUFFER_HPP
