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

#ifndef OPENOCPP_ILOCALCONTROLLERCONFIG_H
#define OPENOCPP_ILOCALCONTROLLERCONFIG_H

#include "ICentralSystemConfig.h"

namespace ocpp
{
namespace config
{

/** @brief Interface to retrieve stack internal configuration for the Local Controller role */
class ILocalControllerConfig : public ICentralSystemConfig
{
  public:
    /** @brief Destructor */
    virtual ~ILocalControllerConfig() { }

    // Behavior

    /** @brief Size of the thread pool to handle incoming requests from the Central System */
    virtual unsigned int incomingRequestsFromCsThreadPoolSize() const = 0;
    /** @brief Disconnect from Charge Point on Central System disconnection */
    virtual bool disconnectFromCpWhenCsDisconnected() const = 0;

    // Unused from Central System configuration interface

    /** @brief Boot notification retry interval */
    std::chrono::seconds bootNotificationRetryInterval() const override { return std::chrono::seconds(0); }
    /** @brief Heartbeat interval */
    std::chrono::seconds heartbeatInterval() const override { return std::chrono::seconds(0); }
    /** @brief If this variable set to true, then the Central System supports ISO 15118 plug and charge messages via the DataTransfer mechanism as
               described in this application note. */
    bool iso15118PnCEnabled() const override { return false; }
};

} // namespace config
} // namespace ocpp

#endif // OPENOCPP_ILOCALCONTROLLERCONFIG_H
