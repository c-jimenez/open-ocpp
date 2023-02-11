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

#ifndef STATUSMANAGERSTUB_H
#define STATUSMANAGERSTUB_H

#include "IStatusManager.h"

#include <map>
#include <tuple>

namespace ocpp
{
namespace chargepoint
{

/** @brief Handle charge point status (boot notification, status notification, heartbeat) */
class StatusManagerStub : public IStatusManager
{
  public:
    /** @brief Constructor */
    StatusManagerStub();

    /** @brief Destructor */
    virtual ~StatusManagerStub();

    // IStatusManager interface

    /** @copydoc ocpp::types::RegistrationStatus IStatusManager::getRegistrationStatus() */
    ocpp::types::RegistrationStatus getRegistrationStatus() override { return m_registration_status; }

    /** @copydoc void IStatusManager::forceRegistrationStatus(ocpp::types::RegistrationStatus) */
    void forceRegistrationStatus(ocpp::types::RegistrationStatus status) override { m_registration_status = status; }

    /** @copydoc void IStatusManager::updateConnectionStatus(bool) */
    void updateConnectionStatus(bool is_connected) override { m_connected = is_connected; }

    /** @copydoc bool IStatusManager::updateConnectorStatus(unsigned int,
     *                                                      ocpp::types::ChargePointStatus,
     *                                                      ocpp::types::ChargePointErrorCode,
     *                                                      const std::string&,
     *                                                      const std::string&,
     *                                                      const std::string&)
    */
    bool updateConnectorStatus(unsigned int                      connector_id,
                               ocpp::types::ChargePointStatus    status,
                               ocpp::types::ChargePointErrorCode error_code   = ocpp::types::ChargePointErrorCode::NoError,
                               const std::string&                info         = "",
                               const std::string&                vendor_id    = "",
                               const std::string&                vendor_error = "") override;

    /** @copydoc void IStatusManager::resetHeartBeatTimer() */
    void resetHeartBeatTimer() override { m_hb_timer_reset = true; }

    // API

    /** @brief Get the connection status */
    bool getConnectionStatus() const { return m_connected; }
    /** @brief Indicate if the heartbeat timer has been reset */
    bool isHeartbeatTimerReset() const { return m_hb_timer_reset; }

    /** @brief Get the status of a connector */
    ocpp::types::ChargePointStatus getConnectorStatus(unsigned int connector_id);
    /** @brief Get the error code of a connector */
    ocpp::types::ChargePointErrorCode getConnectorErrorCode(unsigned int connector_id);
    /** @brief Get the info of a connector */
    std::string getConnectorInfo(unsigned int connector_id);
    /** @brief Get the vendor id of a connector */
    std::string getConnectorVendor(unsigned int connector_id);
    /** @brief Get the vendor error of a connector */
    std::string getConnectorVendorError(unsigned int connector_id);

    /** @brief Reset the stub */
    void reset();

  private:
    /** @brief Registration status */
    ocpp::types::RegistrationStatus m_registration_status;
    /** @brief Connection status */
    bool m_connected;
    /** @brief Indicate if the heartbeat timer has been reset */
    bool m_hb_timer_reset;
    /** @brief Connectors status */
    std::map<unsigned int,
             std::tuple<ocpp::types::ChargePointStatus, ocpp::types::ChargePointErrorCode, std::string, std::string, std::string>>
        m_connectors;
};

} // namespace chargepoint
} // namespace ocpp

#endif // STATUSMANAGERSTUB_H
