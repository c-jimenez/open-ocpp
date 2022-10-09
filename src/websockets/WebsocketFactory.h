/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPENOCPP_WEBSOCKETFACTORY_H
#define OPENOCPP_WEBSOCKETFACTORY_H

#include "IWebsocketClient.h"
#include "IWebsocketServer.h"

namespace ocpp
{
namespace websockets
{

/** @brief Factory to instanciate websockets */
class WebsocketFactory
{
  public:
    /** @brief Instanciate a client websocket */
    static IWebsocketClient* newClient();
    /** @brief Instanciate a server websocket */
    static IWebsocketServer* newServer();
};

} // namespace websockets
} // namespace ocpp

#endif // OPENOCPP_WEBSOCKETFACTORY_H
