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

#ifndef ICHARGEPOINTREQUESTHANDLER_H
#define ICHARGEPOINTREQUESTHANDLER_H

#include "Enums.h"
#include "IdTagInfo.h"
#include "MeterValue.h"

#include <vector>

namespace ocpp
{
namespace centralsystem
{

/** @brief Interface for charge point requests implementations */
class IChargePointRequestHandler
{
  public:
    /** @brief Destructor */
    virtual ~IChargePointRequestHandler() { }

    /** @brief Called to notify the disconnection of the charge point */
    virtual void disconnected() = 0;

    /**
     * @brief Called to get authorization informations for an id tag
     * @param id_tag Id tag to check
     * @return Authorization informations for the id tag (see IdTagInfo documentation)
     */
    virtual ocpp::types::IdTagInfo authorize(const std::string& id_tag) = 0;

    /**
     * @brief Called to get registration status on boot notification reception
     * @param model Charge point's model
     * @param serial_number Charge point's serial number
     * @param vendor Charge point's vendor
     * @param firmware_version Charge point's firmware version
     * @param iccid ICCID of the modem’s SIM card
     * @param imsi IMSI of the modem’s SIM card
     * @param meter_serial_number Serial number of the main electrical meter of the Charge Point
     * @param meter_type Type of the main electrical meter of the Charge Point
     * @return Registration status of the charge point (see RegistrationStatus documentation)
     */
    virtual ocpp::types::RegistrationStatus bootNotification(const std::string& model,
                                                             const std::string& serial_number,
                                                             const std::string& vendor,
                                                             const std::string& firmware_version,
                                                             const std::string& iccid,
                                                             const std::string& imsi,
                                                             const std::string& meter_serial_number,
                                                             const std::string& meter_type) = 0;

    /**
     * @brief Called when a data transfer request has been received
     * @param vendor_id Identifies the vendor specific implementation
     * @param message_id Identifies the message
     * @param request_data Data associated to the request
     * @param response_data Data associated with the response
     * @return Response status (see DataTransferStatus documentation)
     */
    virtual ocpp::types::DataTransferStatus dataTransfer(const std::string& vendor_id,
                                                         const std::string& message_id,
                                                         const std::string& request_data,
                                                         std::string&       response_data) = 0;

    /**
     * @brief Called when a diagnostic status notification has been received
     * @param status Diagnostic status
     */
    virtual void diagnosticStatusNotification(ocpp::types::DiagnosticsStatus status) = 0;

    /**
     * @brief Called when a firmware status notification has been received
     * @param status Firmware status
     */
    virtual void firmwareStatusNotification(ocpp::types::FirmwareStatus status) = 0;

    /**
     * @brief Called when meter values have been received
     * @param connector_id Id of the connector concerned by the meter values
     * @param transaction_id Id of the transaction concerned by the meter values
     * @param meter_values Meter values
     */
    virtual void meterValues(unsigned int                                connector_id,
                             const ocpp::types::Optional<int>&           transaction_id,
                             const std::vector<ocpp::types::MeterValue>& meter_values) = 0;

    /**
     * @brief Called to get an authorization to start a transaction
     * @param connector_id Id of the connector used for the transaction
     * @param id_tag Id tag which wants to start the transaction
     * @param meter_start Meter value in Wh for the connector at start of the transaction
     * @param reservation_id Id of the reservation that terminates as a result of this transaction
     * @param timestamp Date and time on which the transaction is started
     * @param transaction_id Id of the transaction
     * @return Authorization informations for the id tag (see IdTagInfo documentation)
     */
    virtual ocpp::types::IdTagInfo startTransaction(unsigned int                      connector_id,
                                                    const std::string&                id_tag,
                                                    int                               meter_start,
                                                    const ocpp::types::Optional<int>& reservation_id,
                                                    const ocpp::types::DateTime&      timestamp,
                                                    int&                              transaction_id);

    /**
     * @brief Called when a status notification has been received
     * @param connector_id Id of the connector for which the status is reported
     * @param error_code Error code
     * @param info Additional information related to the error
     * @param status Connector's status
     * @param timestamp Time for which the status is reported
     * @param vendor_id Vendor specific implementation identifier
     * @param vendor_error Vendor specific error code
     */
    virtual void statusNotification(unsigned int                      connector_id,
                                    ocpp::types::ChargePointErrorCode error_code,
                                    const std::string&                info,
                                    ocpp::types::ChargePointStatus    status,
                                    const ocpp::types::DateTime&      timestamp,
                                    const std::string&                vendor_id,
                                    const std::string&                vendor_error) = 0;

    /**
     * @brief Called when an end of transaction has been received
     * @param id_tag Id tag which did end the transaction
     * @param meter_stop Meter value in Wh for the connector at stop of the transaction
     * @param timestamp Date and time on which the transaction has been stopped
     * @param transaction_id Id of the transaction
     * @param reason Reason why the transaction was stopped
     * @param transaction_data Transaction related meter values
     * @return Authorization informations for the id tag (see IdTagInfo documentation)
     */
    virtual ocpp::types::Optional<ocpp::types::IdTagInfo> stopTransaction(const std::string&                          id_tag,
                                                                          int                                         meter_stop,
                                                                          const ocpp::types::DateTime&                timestamp,
                                                                          int                                         transaction_id,
                                                                          ocpp::types::Reason                         reason,
                                                                          const std::vector<ocpp::types::MeterValue>& transaction_data) = 0;
};

} // namespace centralsystem
} // namespace ocpp

#endif // ICHARGEPOINTREQUESTHANDLER_H
