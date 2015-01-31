/* Copyright (c) 2015 Francois Pierre Doray <francois.pierre-doray@polymtl.ca>
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
#include "base/EscapeString.hpp"

#include <boost/format.hpp>
#include <cctype>

namespace tibee
{
namespace base
{

namespace
{

void EscapeCharacter(char character, std::string* escaped)
{
    if (character == '\'')
    {
        *escaped += "\\'";
    }
    else if (character == '\\')
    {
        *escaped += "\\\\";
    }
    else if (std::isprint(static_cast<int>(character)) == 0)
    {
        *escaped +=
            str(boost::format("\\x%02x") % static_cast<int>(character));
    }
    else
    {
        *escaped += character;
    }
}

}  // namespace

std::string EscapeString(const std::string& str)
{
    std::string escaped;
    escaped.reserve(str.size());

    for (char character : str)
        EscapeCharacter(character, &escaped);

    return escaped;
}

}  // namespace base
}  // namespace tibee
