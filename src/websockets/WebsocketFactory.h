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

#ifndef WEBSOCKETFACTORY_H
#define WEBSOCKETFACTORY_H

#include "IWebsocketClient.h"

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
};

} // namespace websockets
} // namespace ocpp

#endif // WEBSOCKETFACTORY_H
