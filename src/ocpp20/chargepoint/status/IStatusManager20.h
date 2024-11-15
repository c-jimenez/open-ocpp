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

#ifndef OPENOCPP_OCPP20_ISTATUSMANAGER20_H
#define OPENOCPP_OCPP20_ISTATUSMANAGER20_H

#include "BootNotification20.h"
#include "ConnectorStatusEnumType20.h"
#include "RegistrationStatusEnumType20.h"

namespace ocpp
{
namespace chargepoint
{
namespace ocpp20
{

class IStatusManager
{
  public:
    /** @brief Destructor */
    virtual ~IStatusManager() { }

    /**
     * @brief Get the registration status with the central system
     * @return Registration status (see RegistrationStatusEnumType enum)
     */
    virtual ocpp::types::ocpp20::RegistrationStatusEnumType getRegistrationStatus() = 0;

    /**
     * @brief Force the registration status with the central system
     * @param status New registration status
     */
    virtual void forceRegistrationStatus(ocpp::types::ocpp20::RegistrationStatusEnumType status) = 0;

    /**
     * @brief Update the charge point connection status
     * @param is_connected true if the charge point is connected to the central system, false otherwise
     */
    virtual void updateConnectionStatus(bool is_connected) = 0;

    /**
     * @brief Update the status of a connector
     * @param evse_id Id of the EVSE
     * @param connector_id Id of the connector
     * @param status Status of the connector
     * @return true if the status has been notified, false otherwise
     */
    virtual bool updateConnectorStatus(unsigned int                                 evse_id,
                                       unsigned int                                 connector_id,
                                       ocpp::types::ocpp20::ConnectorStatusEnumType status) = 0;

    /** @brief Reset the hearbeat timer */
    virtual void resetHeartBeatTimer() = 0;
};

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_OCPP20_ISTATUSMANAGER20_H
