/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPENOCPP_OCPP20_ILOCALCONTROLLERPROXYEVENTSHANDLER20_H
#define OPENOCPP_OCPP20_ILOCALCONTROLLERPROXYEVENTSHANDLER20_H

namespace ocpp
{
namespace localcontroller
{
namespace ocpp20
{

/** @brief Interface for Local Controller proxys event handler implementations */
class ILocalControllerProxyEventsHandler20
{
  public:
    /** @brief Destructor */
    virtual ~ILocalControllerProxyEventsHandler20() { }

    /** @brief Called to notify the disconnection of the charge point */
    virtual void disconnectedFromChargePoint() = 0;

    /** @brief Called to notify the connection to the central system */
    virtual void connectedToCentralSystem() = 0;

    /** @brief Called to notify the failure of the connection to the central system */
    virtual void failedToConnectToCentralSystem() = 0;

    /** @brief Called to notify the disconnection from the central system */
    virtual void disconnectedFromCentralSystem() = 0;
};

} // namespace ocpp20
} // namespace localcontroller
} // namespace ocpp

#endif // OPENOCPP_OCPP20_ILOCALCONTROLLERPROXYEVENTSHANDLER20_H
