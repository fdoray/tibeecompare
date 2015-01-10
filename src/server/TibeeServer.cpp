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
#include "server/TibeeServer.hpp"

#include <iostream>
#include <unistd.h>
#include <zmq.hpp>

#include "base/print.hpp"
#include "mq/MessageDecoder.hpp"
#include "mq/MessageEncoder.hpp"
#include "server/handlers/HandleGetNodes.hpp"
#include "server/handlers/HandleGetProperties.hpp"

namespace tibee
{
namespace server
{

namespace
{

using base::tberror;
using base::tbendl;

enum RequestType
{
    kGetNodesRequestType = 1,
    kGetPropertiesRequestType = 2,
};

const uint32_t kErrorResponse = -1;

void SendError(zmq::socket_t* socket)
{
    mq::MessageEncoder responseEncoder;
    responseEncoder.Write(kErrorResponse);
    zmq::message_t reply(responseEncoder.size());
    memcpy((void*)reply.data(), responseEncoder.data(), responseEncoder.size());
    socket->send(reply);
}

}  // namespace

TibeeServer::TibeeServer(const Arguments& args)
{
    validateSaveArguments(args);
}

bool TibeeServer::run()
{
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");

    // Handle requests.
    for (;;)
    {
        // Receive the request.
        zmq::message_t request;
        if (!socket.recv(&request))
        {
            switch (zmq_errno())
            {
                case EAGAIN:
                    tberror() << "ZMQ receive error EAGAIN." << tbendl();
                    break;
                case ENOTSUP:
                    tberror() << "ZMQ receive error ENOTSUP." << tbendl();
                    break;
                case EFSM:
                    tberror() << "ZMQ receive error EFSM." << tbendl();
                    break;
                case ETERM:
                    tberror() << "ZMQ receive error ETERM." << tbendl();
                    break;
                case ENOTSOCK:
                    tberror() << "ZMQ receive error ENOTSOCK." << tbendl();
                    break;
                case EINTR:
                    tberror() << "ZMQ receive error EINTR." << tbendl();
                    break;
                case EFAULT:
                    tberror() << "ZMQ receive error EFAULT." << tbendl();
                    break;
                default:
                    tberror() << "ZMQ receive error unknown." << tbendl();
                    break;
            }
            return false;
        }

        mq::MessageDecoder requestDecoder(&request);

        // Handle the request.
        uint32_t requestType;
        if (!requestDecoder.Read(&requestType))
        {
            tberror() << "Received a request without a type." << tbendl();
            SendError(&socket);
            continue;
        }
        mq::MessageEncoder responseEncoder;

        switch (requestType)
        {
            case kGetNodesRequestType:
            {
                handlers::HandleGetNodes(&requestDecoder, &responseEncoder);
                break;
            }
            case kGetPropertiesRequestType:
            {
                handlers::HandleGetProperties(&requestDecoder, &responseEncoder);
                break;
            }
            default:
            {
                tberror() << "Received a request with an invalid type (" << requestType << ")" << tbendl();
                SendError(&socket);
                continue;
            }
        }

        // Send the response.
        zmq::message_t reply(responseEncoder.size());
        memcpy((void*)reply.data(), responseEncoder.data(), responseEncoder.size());
        socket.send(reply);

        // TODO(fdoray): Use a ZMQ_ROUTER socket.
    }

    return true;
}

void TibeeServer::validateSaveArguments(const Arguments& args)
{
    _verbose = args.verbose;
}

}  // namespace server
}  // namespace tibee
