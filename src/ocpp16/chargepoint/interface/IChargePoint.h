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

#ifndef OPENOCPP_ICHARGEPOINT_H
#define OPENOCPP_ICHARGEPOINT_H

#include "CertificateRequest.h"
#include "IChargePointConfig.h"
#include "IChargePointEventsHandler.h"
#include "IOcppConfig.h"
#include "OcspRequestDataType.h"
#include "SecurityEvent.h"
#include "SmartChargingSetpoint.h"

#include <memory>

namespace ocpp
{
namespace helpers
{
class ITimerPool;
class WorkerThreadPool;
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

    /**
     * @brief Instanciate a charge point with the provided timer and worker pools
     *        To use when you have to instanciate multiple Central System / Charge Point
     *        => Allow to reduce thread and memory usage
     * @param stack_config Stack configuration
     * @param ocpp_config Standard OCPP configuration
     * @param event_handler Stack event handler
     * @param timer_pool Timer pool
     * @param worker_pool Worker thread pool
     */
    static std::unique_ptr<IChargePoint> create(const ocpp::config::IChargePointConfig&          stack_config,
                                                ocpp::config::IOcppConfig&                       ocpp_config,
                                                IChargePointEventsHandler&                       events_handler,
                                                std::shared_ptr<ocpp::helpers::ITimerPool>       timer_pool,
                                                std::shared_ptr<ocpp::helpers::WorkerThreadPool> worker_pool);

    /** @brief Destructor */
    virtual ~IChargePoint() { }

    /**
     * @brief Get the timer pool associated to the charge point
     * @return Timer pool associated to the charge point
     */
    virtual ocpp::helpers::ITimerPool& getTimerPool() = 0;

    /**
     * @brief Get the worker pool associated to the charge point
     * @return Worker pool associated to the charge point
     */
    virtual ocpp::helpers::WorkerThreadPool& getWorkerPool() = 0;

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
     * @brief Triggers a reconnexion of the charge point to the Central System
     * @return true if the reconnexion has been scheduled, false otherwise
     */
    virtual bool reconnect() = 0;

    /**
     * @brief Get the registration status of the charge point
     * @return Registration of the charge point
     */
    virtual ocpp::types::ocpp16::RegistrationStatus getRegistrationStatus() = 0;

    /**
     * @brief Get the status of a connector
     * @param connector_id Id of the connector
     * @return Status of the connector
     */
    virtual ocpp::types::ocpp16::ChargePointStatus getConnectorStatus(unsigned int connector_id) = 0;

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
    virtual bool statusNotification(
        unsigned int                              connector_id,
        ocpp::types::ocpp16::ChargePointStatus    status,
        ocpp::types::ocpp16::ChargePointErrorCode error_code   = ocpp::types::ocpp16::ChargePointErrorCode::NoError,
        const std::string&                        info         = "",
        const std::string&                        vendor_id    = "",
        const std::string&                        vendor_error = "") = 0;

    /**
     * @brief Ask for authorization of an operation on a connector
     * @param connector_id Id of the connector
     * @param id_tag Id of the user
     * @param parent_id If of the user's parent tag
     * @return Authorization status (see AuthorizationStatus enum)
     */
    virtual ocpp::types::ocpp16::AuthorizationStatus authorize(unsigned int       connector_id,
                                                               const std::string& id_tag,
                                                               std::string&       parent_id) = 0;

    /**
     * @brief Start a transaction
     * @param connector_id Id of the connector
     * @param id_tag Id of the user
     * @param transaction_id Id of the transaction
     * @return Authorization status (see AuthorizationStatus enum)
     */
    virtual ocpp::types::ocpp16::AuthorizationStatus startTransaction(unsigned int       connector_id,
                                                                      const std::string& id_tag,
                                                                      int&               transaction_id) = 0;

    /**
     * @brief Stop a transaction
     * @param connector_id Id of the connector
     * @param id_tag Id of the user (leave empty if no id tag)
     * @param reason Stop reason
     * @return true if a corresponding transaction exist and has been stopped, false otherwise
     */
    virtual bool stopTransaction(unsigned int connector_id, const std::string& id_tag, ocpp::types::ocpp16::Reason reason) = 0;

    /**
     * @brief Send a data transfer request
     * @param vendor_id Identifies the vendor specific implementation
     * @param message_id Identifies the message
     * @param request_data Data associated to the request
     * @param status Response status (see DataTransferStatus documentation)
     * @param response_data Data associated with the response
     * @return true if the data transfer has been done, false otherwise
     */
    virtual bool dataTransfer(const std::string&                       vendor_id,
                              const std::string&                       message_id,
                              const std::string&                       request_data,
                              ocpp::types::ocpp16::DataTransferStatus& status,
                              std::string&                             response_data) = 0;

    /**
     * @brief Send meter values to Central System for a given connector
     * @param connector_id Id of the connector
     * @param values Meter values to send
     * @return true if the meter values have been sent, false otherwise
     */
    virtual bool sendMeterValues(unsigned int connector_id, const std::vector<ocpp::types::ocpp16::MeterValue>& values) = 0;

    /**
     * @brief Get the smart charging setpoints for a connector and the whole charge point
     * @param connector_id Id of the connector
     * @param charge_point_setpoint Setpoint of the whole charge point (not set if no active profile)
     * @param connector_setpoint Setpoint of the given connector (not set if no active profile)
     * @param unit Setpoint unit (A or W)
     * @return true if the setpoints have been computed, false otherwise
     */
    virtual bool getSetpoint(unsigned int                                                       connector_id,
                             ocpp::types::Optional<ocpp::types::ocpp16::SmartChargingSetpoint>& charge_point_setpoint,
                             ocpp::types::Optional<ocpp::types::ocpp16::SmartChargingSetpoint>& connector_setpoint,
                             ocpp::types::ocpp16::ChargingRateUnitType unit = ocpp::types::ocpp16::ChargingRateUnitType::A) = 0;

    /**
     * @brief Notify the end of a firmware update operation
     * @param success Set to true if the firmware has been installed,
     *                otherwise set to false if the installation failed
     * @return true if the notification has been sent, false otherwise
     */
    virtual bool notifyFirmwareUpdateStatus(bool success) = 0;

    // Security extensions

    /**
     * @brief Log a security event
     * @param type Type of the security event
     * @param message Additional information about the occurred security event
     * @param critical If non-standard security event, indicates its criticity
     *                 (only critival events are forward to central system)
     * @return true if the security evenst has been logged, false otherwise
     */
    virtual bool logSecurityEvent(const std::string& type, const std::string& message, bool critical = false) = 0;

    /**
     * @brief Clear all the security events
     * @return true if the security evenst have been cleared, false otherwise
     */
    virtual bool clearSecurityEvents() = 0;

    /**
     * @brief Send a CSR request to sign a certificate
     *        (Can be used only if InternalCertificateManagementEnabled = false)
     * @param csr CSR request
     * @return true if the request has been sent and accepted, false otherwise
     */
    virtual bool signCertificate(const ocpp::x509::CertificateRequest& csr) = 0;

    /**
     * @brief Send a CSR request to sign a certificate
     *        (Can be used only if InternalCertificateManagementEnabled = true)
     * @return true if the request has been sent and accepted, false otherwise
     */
    virtual bool signCertificate() = 0;

    /**
     * @brief Notify the end of a signed firmware update operation
     * @param status Installation status (see FirmwareStatusEnumType documentation)
     * @return true if the notification has been sent, false otherwise
     */
    virtual bool notifySignedUpdateFirmwareStatus(ocpp::types::ocpp16::FirmwareStatusEnumType status) = 0;

    // ISO 15118 PnC extensions

    /**
     * @brief Authorize an ISO15118 transaction
     * @param certificate The X.509 certificated presented by EV
     * @param id_token This contains the identifier that needs to be authorized
     * @param cert_hash_data Contains the information needed to verify the EV Contract Certificate via OCSP
     * @param cert_status Certificate status information. - if all certificates are
     *                    valid: return 'Accepted'. - if one of the certificates was revoked,
     *                    return 'CertificateRevoked
     * @return Authorization status (see AuthorizationStatus type)
    */
    virtual ocpp::types::ocpp16::AuthorizationStatus iso15118Authorize(
        const ocpp::x509::Certificate&                                                  certificate,
        const std::string&                                                              id_token,
        const std::vector<ocpp::types::ocpp16::OcspRequestDataType>&                    cert_hash_data,
        ocpp::types::Optional<ocpp::types::ocpp16::AuthorizeCertificateStatusEnumType>& cert_status) = 0;

    /**
     * @brief Get or update an ISO15118 EV certificate
     * @param iso15118_schema_version Schema version currently used for the 15118 session between EV and Charge Point
     * @param action Defines whether certificate needs to be installed or updated
     * @param exi_request Raw CertificateInstallationReq request from EV, Base64 encoded
     * @param exi_response Raw CertificateInstallationRes response for the EV, Base64 encoded
     * @return true if the processing of the message has been successful and an EXI response has been included, false otherwise
     */
    virtual bool iso15118GetEVCertificate(const std::string&                             iso15118_schema_version,
                                          ocpp::types::ocpp16::CertificateActionEnumType action,
                                          const std::string&                             exi_request,
                                          std::string&                                   exi_response) = 0;

    /**
     * @brief Get the status of an ISO15118 certificate
     * @param ocsp_request Indicates the certificate of which the status is requested
     * @param ocsp_result OCSPResponse class as defined in IETF RFC 6960. DER encoded (as defined in IETF RFC 6960), and then base64 encoded
     * @return true if the status of the certificate has been successfully retrieved, false otherwise
     */
    virtual bool iso15118GetCertificateStatus(const ocpp::types::ocpp16::OcspRequestDataType& ocsp_request, std::string& ocsp_result) = 0;

    /**
     * @brief Send a CSR request to sign an ISO15118 certificate
     * @param csr CSR request
     * @return true if the request has been sent and accepted, false otherwise
     */
    virtual bool iso15118SignCertificate(const ocpp::x509::CertificateRequest& csr) = 0;
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_ICHARGEPOINT_H
