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

#ifndef OPENOCPP_ISTATUSMANAGER_H
#define OPENOCPP_ISTATUSMANAGER_H

#include "Enums.h"

namespace ocpp
{
namespace chargepoint
{
class IStatusManager
{
  public:
    /** @brief Destructor */
    virtual ~IStatusManager() { }

    /**
     * @brief Get the registration status with the central system
     * @return Registration status (see RegistrationStatus enum)
     */
    virtual ocpp::types::ocpp16::RegistrationStatus getRegistrationStatus() = 0;

    /**
     * @brief Force the registration status with the central system
     * @param status New registration status
     */
    virtual void forceRegistrationStatus(ocpp::types::ocpp16::RegistrationStatus status) = 0;

    /**
     * @brief Update the charge point connection status
     * @param is_connected true if the charge point is connected to the central system, false otherwise
     */
    virtual void updateConnectionStatus(bool is_connected) = 0;

    /**
     * @brief Update the status of a connector
     * @param connector_id Id of the connector
     * @param status Status of the connector
     * @param error_code Error code if in Faulted state
     * @param info Information about current status
     * @param vendor_id Identifies vendor specific implementation
     * @param vendor_error Vendor specific error code
     * @return true if the status has been notified, false otherwise
     */
    virtual bool updateConnectorStatus(
        unsigned int                              connector_id,
        ocpp::types::ocpp16::ChargePointStatus    status,
        ocpp::types::ocpp16::ChargePointErrorCode error_code   = ocpp::types::ocpp16::ChargePointErrorCode::NoError,
        const std::string&                        info         = "",
        const std::string&                        vendor_id    = "",
        const std::string&                        vendor_error = "") = 0;

    /** @brief Reset the hearbeat timer */
    virtual void resetHeartBeatTimer() = 0;
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_ISTATUSMANAGER_H
