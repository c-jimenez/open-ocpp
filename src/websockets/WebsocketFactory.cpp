/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License version 2.1
as published by the Free Software Foundation.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#include "WebsocketFactory.h"
#include "LibWebsocketClient.h"
#include "LibWebsocketServer.h"

namespace ocpp
{
namespace websockets
{

/** @brief Instanciate a client websocket */
IWebsocketClient* WebsocketFactory::newClient()
{
    return new LibWebsocketClient();
}

/** @brief Instanciate a server websocket */
IWebsocketServer* WebsocketFactory::newServer()
{
    return new LibWebsocketServer();
}

} // namespace websockets
} // namespace ocpp
