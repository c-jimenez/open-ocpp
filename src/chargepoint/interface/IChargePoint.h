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

#ifndef ICHARGEPOINT_H
#define ICHARGEPOINT_H

#include "IChargePointConfig.h"
#include "IChargePointEventsHandler.h"
#include "IOcppConfig.h"

#include <memory>

namespace ocpp
{
namespace helpers
{
class TimerPool;
} // namespace helpers

namespace chargepoint
{

/** @brief Interface for charge point implementations */
class IChargePoint
{
  public:
    /**
     * @brief Instanciate a charge point
     * @param stack_config Stack configuration
     * @param ocpp_config Standard OCPP configuration
     * @param event_handler Stack event handler
     */
    static std::unique_ptr<IChargePoint> create(const ocpp::config::IChargePointConfig& stack_config,
                                                ocpp::config::IOcppConfig&              ocpp_config,
                                                IChargePointEventsHandler&              events_handler);

    /** @brief Destructor */
    virtual ~IChargePoint() { }

    /**
     * @brief Get the timer pool associated to the charge point
     * @return Timer pool associated to the charge point
     */
    virtual ocpp::helpers::TimerPool& getTimerPool() = 0;

    /**
     * @brief Reset the charge point's internal data (can be done only when the charge point is stopped)
     * @return true if the data has been reset, false otherwise
     */
    virtual bool resetData() = 0;

    /**
     * @brief Reset only connector related persistency data (can be done only when the charge point is stopped)
     * @return true if the data has been reset, false otherwise
     */
    virtual bool resetConnectorData() = 0;

    /**
     * @brief Start the charge point
     * @return true if the charge point has been started, false otherwise
     */
    virtual bool start() = 0;

    /**
     * @brief Stop the charge point
     * @return true if the charge point has been stopped, false otherwise
     */
    virtual bool stop() = 0;

    /**
     * @brief Get the registration status of the charge point
     * @return Registration of the charge point
     */
    virtual ocpp::types::RegistrationStatus getRegistrationStatus() = 0;

    /**
     * @brief Get the status of a connector
     * @param connector_id Id of the connector
     * @return Status of the connector
     */
    virtual ocpp::types::ChargePointStatus getConnectorStatus(unsigned int connector_id) = 0;

    /**
     * @brief Notify a new status for a connector
     * @param connector_id Id of the connector
     * @param status Status of the connector
     * @param error_code Error code if in Faulted state
     * @param info Information about current status
     * @param vendor_id Identifies vendor specific implementation
     * @param vendor_error Vendor specific error code
     * @return true if the status has been notified, false otherwise
     */
    virtual bool statusNotification(unsigned int                      connector_id,
                                    ocpp::types::ChargePointStatus    status,
                                    ocpp::types::ChargePointErrorCode error_code   = ocpp::types::ChargePointErrorCode::NoError,
                                    const std::string&                info         = "",
                                    const std::string&                vendor_id    = "",
                                    const std::string&                vendor_error = "") = 0;

    /**
     * @brief Ask for authorization of an operation on a connector
     * @param connector_id Id of the connector
     * @param id_tag Id of the user
     * @param parent_id If of the user's parent tag
     * @return Authorization status (see AuthorizationStatus enum)
     */
    virtual ocpp::types::AuthorizationStatus authorize(unsigned int connector_id, const std::string& id_tag, std::string& parent_id) = 0;

    /**
     * @brief Start a transaction
     * @param connector_id Id of the connector
     * @param id_tag Id of the user
     * @return ocpp::types::AuthorizationStatus (see AuthorizationStatus enum)
     */
    virtual ocpp::types::AuthorizationStatus startTransaction(unsigned int connector_id, const std::string& id_tag) = 0;

    /**
     * @brief Stop a transaction
     * @param connector_id Id of the connector
     * @param id_tag Id of the user (leave empty if no id tag)
     * @param reason Stop reason
     * @return true if a corresponding transaction exist and has been stopped, false otherwise
     */
    virtual bool stopTransaction(unsigned int connector_id, const std::string& id_tag, ocpp::types::Reason reason) = 0;

    /**
     * @brief Send a data transfer request
     * @param vendor_id Identifies the vendor specific implementation
     * @param message_id Identifies the message
     * @param request_data Data associated to the request
     * @param status Response status
     * @param response_data Data associated with the response
     * @return true if the data transfer has been done, false otherwise
     */
    virtual bool dataTransfer(const std::string&               vendor_id,
                              const std::string&               message_id,
                              const std::string&               request_data,
                              ocpp::types::DataTransferStatus& status,
                              std::string&                     response_data) = 0;

    /**
     * @brief Send meter values to Central System for a given connector
     * @param connector_id Id of the connector
     * @param values Meter values to send
     * @return true if the meter values have been sent, false otherwise
     */
    virtual bool sendMeterValues(unsigned int connector_id, const std::vector<ocpp::types::MeterValue>& values) = 0;

    /**
     * @brief Get the smart charging setpoints for a connector and the whole charge point
     * @param connector_id Id of the connector
     * @param charge_point_setpoint Setpoint of the whole charge point in A (not set if no active profile)
     * @param connector_setpoint Setpoint of the given connector in A (not set if no active profile)
     * @return true if the setpoints have been computed, false otherwise
     */
    virtual bool getSetpoint(unsigned int                  connector_id,
                             ocpp::types::Optional<float>& charge_point_setpoint,
                             ocpp::types::Optional<float>& connector_setpoint) = 0;

    /**
     * @brief Notify the end of a firmware update operation
     * @param success Set to true if the firmware has been installed,
     *                otherwise set to false if the installation failed
     * @return true if the notification has been sent, false otherwise
     */
    virtual bool notifyFirmwareUpdateStatus(bool success) = 0;
};

} // namespace chargepoint
} // namespace ocpp

#endif // ICHARGEPOINT_H
