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

#ifndef OPENOCPP_OCPP20_ILOCALCONTROLLERCONFIG_H
#define OPENOCPP_OCPP20_ILOCALCONTROLLERCONFIG_H

#include "ICentralSystemConfig20.h"

namespace ocpp
{
namespace config
{

/** @brief Interface to retrieve stack internal configuration for the Local Controller role */
class ILocalControllerConfig20 : public ICentralSystemConfig20
{
  public:
    /** @brief Destructor */
    virtual ~ILocalControllerConfig20() { }

    // Behavior

    /** @brief Size of the thread pool to handle incoming requests from the Central System */
    virtual unsigned int incomingRequestsFromCsThreadPoolSize() const = 0;
    /** @brief Disconnect from Charge Point on Central System disconnection */
    virtual bool disconnectFromCpWhenCsDisconnected() const = 0;
};

} // namespace config
} // namespace ocpp

#endif // OPENOCPP_OCPP20_ILOCALCONTROLLERCONFIG_H
