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

#ifndef OPENOCPP_ICHARGEPOINTREQUESTHANDLER_H
#define OPENOCPP_ICHARGEPOINTREQUESTHANDLER_H

#include "Certificate.h"
#include "CertificateRequest.h"
#include "Enums.h"
#include "IdTagInfo.h"
#include "IdTokenInfoType.h"
#include "MeterValue.h"
#include "OcspRequestDataType.h"

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
     * @brief Called when an heartbeat has been received
     */
    virtual void heartbeat() { }

    /**
     * @brief Called to get authorization informations for an id tag
     * @param id_tag Id tag to check
     * @return Authorization informations for the id tag (see IdTagInfo documentation)
     */
    virtual ocpp::types::ocpp16::IdTagInfo authorize(const std::string& id_tag) = 0;

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
    virtual ocpp::types::ocpp16::RegistrationStatus bootNotification(const std::string& model,
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
    virtual ocpp::types::ocpp16::DataTransferStatus dataTransfer(const std::string& vendor_id,
                                                                 const std::string& message_id,
                                                                 const std::string& request_data,
                                                                 std::string&       response_data) = 0;

    /**
     * @brief Called when a diagnostic status notification has been received
     * @param status Diagnostic status
     */
    virtual void diagnosticStatusNotification(ocpp::types::ocpp16::DiagnosticsStatus status) = 0;

    /**
     * @brief Called when a firmware status notification has been received
     * @param status Firmware status
     */
    virtual void firmwareStatusNotification(ocpp::types::ocpp16::FirmwareStatus status) = 0;

    /**
     * @brief Called when meter values have been received
     * @param connector_id Id of the connector concerned by the meter values
     * @param transaction_id Id of the transaction concerned by the meter values
     * @param meter_values Meter values
     */
    virtual void meterValues(unsigned int                                        connector_id,
                             const ocpp::types::Optional<int>&                   transaction_id,
                             const std::vector<ocpp::types::ocpp16::MeterValue>& meter_values) = 0;

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
    virtual ocpp::types::ocpp16::IdTagInfo startTransaction(unsigned int                      connector_id,
                                                            const std::string&                id_tag,
                                                            int                               meter_start,
                                                            const ocpp::types::Optional<int>& reservation_id,
                                                            const ocpp::types::DateTime&      timestamp,
                                                            int&                              transaction_id) = 0;

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
    virtual void statusNotification(unsigned int                              connector_id,
                                    ocpp::types::ocpp16::ChargePointErrorCode error_code,
                                    const std::string&                        info,
                                    ocpp::types::ocpp16::ChargePointStatus    status,
                                    const ocpp::types::DateTime&              timestamp,
                                    const std::string&                        vendor_id,
                                    const std::string&                        vendor_error) = 0;

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
    virtual ocpp::types::Optional<ocpp::types::ocpp16::IdTagInfo> stopTransaction(
        const std::string&                                  id_tag,
        int                                                 meter_stop,
        const ocpp::types::DateTime&                        timestamp,
        int                                                 transaction_id,
        ocpp::types::ocpp16::Reason                         reason,
        const std::vector<ocpp::types::ocpp16::MeterValue>& transaction_data) = 0;

    // Security extensions

    /**
     * @brief Called when a log status notification has been received
     * @param status Log status
     * @param request_id Request id of the correspondin GetLog request
     */
    virtual void logStatusNotification(ocpp::types::ocpp16::UploadLogStatusEnumType status,
                                       const ocpp::types::Optional<int>&            request_id) = 0;

    /**
     * @brief Called when a security event notification has been received
     * @param type Type of the security event
     * @param timestamp Timestamp when the security event has been generated
     * @param message Additional information about the occurred security event
     */
    virtual void securityEventNotification(const std::string& type, const ocpp::types::DateTime& timestamp, const std::string& message) = 0;

    /**
     * @brief Called when a request to sign a new client certificat has been received
     * @param certificate_request Certificate request
     * @return true if the certificate request can be processed, false otherwise
     */
    virtual bool signCertificate(const ocpp::x509::CertificateRequest& certificate_request) = 0;

    /**
     * @brief Called when a signed firmware update status notification has been received
     * @param status Firmware update status
     * @param request_id Request id of the correspondin GetLog request
     */
    virtual void signedFirmwareUpdateStatusNotification(ocpp::types::ocpp16::FirmwareStatusEnumType status,
                                                        const ocpp::types::Optional<int>&           request_id) = 0;

    // ISO 15118 PnC extensions

    /**
     * @brief Called to authorize an ISO15118 transaction
     * @param certificate The X.509 certificated presented by EV
     * @param id_token This contains the identifier that needs to be authorized
     * @param cert_hash_data Contains the information needed to verify the EV Contract Certificate via OCSP
     * @param cert_status Certificate status information. - if all certificates are
     *                    valid: return 'Accepted'. - if one of the certificates was revoked,
     *                    return 'CertificateRevoked
     * @return Authorization status (see AuthorizationStatus type)
    */
    virtual ocpp::types::ocpp16::IdTokenInfoType iso15118Authorize(
        const ocpp::x509::Certificate&                                                  certificate,
        const std::string&                                                              id_token,
        const std::vector<ocpp::types::ocpp16::OcspRequestDataType>&                    cert_hash_data,
        ocpp::types::Optional<ocpp::types::ocpp16::AuthorizeCertificateStatusEnumType>& cert_status) = 0;

    /**
     * @brief Called when the Charge Point wants to get or update an ISO15118 EV certificate
     * @param iso15118_schema_version Schema version currently used for the 15118 session between EV and Charge Point
     * @param action Defines whether certificate needs to be installed or updated
     * @param exi_request Raw CertificateInstallationReq request from EV, Base64 encoded
     * @param exi_response Raw CertificateInstallationRes response for the EV, Base64 encoded
     * @return Operation status (see Iso15118EVCertificateStatusEnumType enum)
     */
    virtual ocpp::types::ocpp16::Iso15118EVCertificateStatusEnumType iso15118GetEVCertificate(
        const std::string&                             iso15118_schema_version,
        ocpp::types::ocpp16::CertificateActionEnumType action,
        const std::string&                             exi_request,
        std::string&                                   exi_response) = 0;

    /**
     * @brief Called when the CHarge Point wants to get the validity status of an ISO15118 certificate
     * @param ocsp_request Indicates the certificate of which the status is requested
     * @param ocsp_result OCSPResponse class as defined in IETF RFC 6960. DER encoded (as defined in IETF RFC 6960), and then base64 encoded
     * @return Operation status (see GetCertificateStatusEnumType enum)
     */
    virtual ocpp::types::ocpp16::GetCertificateStatusEnumType iso15118GetCertificateStatus(
        const ocpp::types::ocpp16::OcspRequestDataType& ocsp_request, std::string& ocsp_result) = 0;

    /**
     * @brief Called when a request to sign a new ISO15118 client certificat has been received
     * @param certificate_request Certificate request
     * @return true if the certificate request can be processed, false otherwise
     */
    virtual bool iso15118SignCertificate(const ocpp::x509::CertificateRequest& certificate_request) = 0;
};

} // namespace centralsystem
} // namespace ocpp

#endif // OPENOCPP_ICHARGEPOINTREQUESTHANDLER_H
