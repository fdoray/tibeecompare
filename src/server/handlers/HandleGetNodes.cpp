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
#include "server/handlers/HandleGetNodes.hpp"

#include "base/CompareConstants.hpp"
#include "base/print.hpp"

namespace tibee
{
namespace server
{
namespace handlers
{

namespace
{

using base::tberror;
using base::tbendl;

bool ReadRequest(mq::MessageDecoder* request,
                 std::vector<std::string>* graphIds,
                 size_t* graphIndex)
{
    assert(request != nullptr);
    assert(graphIds != nullptr);
    assert(graphIndex != nullptr);

    *graphIds = std::vector<std::string>(2);
    int graphIndexInt = 0;
    if (!request->ReadString(&graphIds->at(0), kOIDLength) ||
        !request->ReadString(&graphIds->at(1), kOIDLength) ||
        !request->Read(&graphIndexInt))
    {
        return false;
    }
    *graphIndex = static_cast<size_t>(graphIndexInt);

    return true;
}

}  // namespace

bool HandleGetNodes(mq::MessageDecoder* request,
                    mq::MessageEncoder* response)
{
    // Read the request.
    std::vector<std::string> graphIds;
    size_t graphIndex = 0;
    if (!ReadRequest(request, &graphIds, &graphIndex))
    {
        tberror() << "Unable to read GetNodes request." << tbendl();
        return false;
    }

    return true;
}

}  // namespace handlers
}  // namespace server
}  // namespace tibee
