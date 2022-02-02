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

#ifndef ICHARGEPOINTEVENTSHANDLER_H
#define ICHARGEPOINTEVENTSHANDLER_H

#include "Certificate.h"
#include "DateTime.h"
#include "Enums.h"
#include "MeterValue.h"

namespace ocpp
{
namespace chargepoint
{

/** @brief Interface for charge point event handlers implementations */
class IChargePointEventsHandler
{
  public:
    /** @brief Destructor */
    virtual ~IChargePointEventsHandler() { }

    /**
     * @brief Called when the first attempt to connect to the central system has failed
     * @param status Previous registration status (if Accepted, some offline operations are allowed)
     */
    virtual void connectionFailed(ocpp::types::RegistrationStatus status) = 0;

    /**
     * @brief Called when the charge point connection status has changed
     * @param isConnected true if the charge point is connected to the central system, false otherwise
     */
    virtual void connectionStateChanged(bool isConnected) = 0;

    /**
     * @brief Called on boot notification response from the central system
     * @param status Registration status
     * @param datetime Date and time of the central system
     */
    virtual void bootNotification(ocpp::types::RegistrationStatus status, const ocpp::types::DateTime& datetime) = 0;

    /**
     * @brief Called when the date and time must be adjusted with the one of the central system
     */
    virtual void datetimeReceived(const ocpp::types::DateTime& datetime) = 0;

    /**
     * @brief Called when a change availability request has been received from the central system
     * @param connector_id Id of the concerned connector (0 = whole charge point)
     * @param availability Requested availability
     * @return Status of the requested availability change (see AvailabilityStatus)
     */
    virtual ocpp::types::AvailabilityStatus changeAvailabilityRequested(unsigned int                  connector_id,
                                                                        ocpp::types::AvailabilityType availability) = 0;

    /**
     * @brief Called to retrieve the meter value in Wh for a connector at the start or at the end of a transaction
     * @param connector_id Id of the concerned connector
     * @return Meter value in Wh for the connector
     */
    virtual unsigned int getTxStartStopMeterValue(unsigned int connector_id) = 0;

    /**
     * @brief Called when a reservation as started on a connector
     * @param connector_id Id of the concerned connector
     */
    virtual void reservationStarted(unsigned int connector_id) = 0;

    /**
     * @brief Called when a reservation as expired or has been canceled on a connector
     * @param connector_id Id of the concerned connector
     * @param canceled True is the reservation has been canceled, false if it has expired
     */
    virtual void reservationEnded(unsigned int connector_id, bool canceled) = 0;

    /**
     * @brief Called when a data transfer request has been received
     * @param vendor_id Identifies the vendor specific implementation
     * @param message_id Identifies the message
     * @param request_data Data associated to the request
     * @param response_data Data associated with the response
     * @return Response status (see DataTransferStatus enum)
     */
    virtual ocpp::types::DataTransferStatus dataTransferRequested(const std::string& vendor_id,
                                                                  const std::string& message_id,
                                                                  const std::string& request_data,
                                                                  std::string&       response_data) = 0;

    /**
     * @brief Get a meter value associated to a connector
     * @param connector_id Id of the concerned connector (0 = whole charge point)
     * @param measurand Mesurand of the meter value to retrieve and its phase if specified
     * @param meter_value Meter value to fill (the context and measurand fields of SampleValues doesn't need to be filled)
     * @return true if the meter value can be retrived, false otherwise
     */
    virtual bool getMeterValue(unsigned int                                                                        connector_id,
                               const std::pair<ocpp::types::Measurand, ocpp::types::Optional<ocpp::types::Phase>>& measurand,
                               ocpp::types::MeterValue&                                                            meter_value) = 0;

    /**
     * @brief Called when a remote start transaction request has been received
     * @param connector_id Id of the concerned connector
     * @param id_tag Tag to use for the transaction
     * @return true if the request is accepted, false otherwise
     */
    virtual bool remoteStartTransactionRequested(unsigned int connector_id, const std::string& id_tag) = 0;

    /**
     * @brief Called when a remote stop transaction request has been received
     * @param connector_id Id of the concerned connector
     * @return true if the request is accepted, false otherwise
     */
    virtual bool remoteStopTransactionRequested(unsigned int connector_id) = 0;

    /**
     * @brief Called when a transaction which was started offline has been deauthorized
     *        when the connection with the Central System has been established again
     * @param connector_id Id of the concerned connector
     */
    virtual void transactionDeAuthorized(unsigned int connector_id) = 0;

    /**
     * @brief Called on a reset request from the Central System
     * @param reset_type Type of reset
     * @return true if the request is accepted, false otherwise
     */
    virtual bool resetRequested(ocpp::types::ResetType reset_type) = 0;

    /**
     * @brief Called on an unlock connector request from the Central System
     * @param connector_id Id of the concerned connector
     * @return Unlock status (see UnlockStatus documentation)
     */
    virtual ocpp::types::UnlockStatus unlockConnectorRequested(unsigned int connector_id) = 0;

    /**
     * @brief Called on a diagnostic request
     * @param start_time If set, contains the date and time of the oldest logging information to
     *                   include in the diagnostics
     * @param stop_time If set, contains the date and time of the latest logging information to
     *                  include in the diagnostics
     * @return Path to the generated diagnostic file to upload to the Central System, or empty
     *         string if no diagnostics are available
     */
    virtual std::string getDiagnostics(const ocpp::types::Optional<ocpp::types::DateTime>& start_time,
                                       const ocpp::types::Optional<ocpp::types::DateTime>& stop_time) = 0;

    /**
     * @brief Called on an update firmware request
     * @return Path where to download the firmware
     */
    virtual std::string updateFirmwareRequested() = 0;

    /**
     * @brief Called when a firmware is ready to be installed
     *        (The installation process can be done asynchronously after
     *        this function returns)
     * @param firmware_file Path to the firmware file to install
     */
    virtual void installFirmware(const std::string& firmware_file) = 0;

    /**
     * @brief Called to upload a file to the a given URL
     * @param file Path to the file to upload
     * @param url URL where to upload the file
     * @return true if the file has been uploaded, false otherwise
     */
    virtual bool uploadFile(const std::string& file, const std::string& url) = 0;

    /**
     * @brief Called to download a file from the a given URL
     * @param url URL from where to download the file
     * @param file Path where to save the file to download
     * @return true if the file has been downloaded, false otherwise
     */
    virtual bool downloadFile(const std::string& url, const std::string& file) = 0;

    // Security extensions

    /**
     * @brief Called when a CA certificate has been received and must be installed
     * @param type Type of CA certificate
     * @param certificate CA certificate to install
     * @return Installation status (see CertificateStatusEnumType enum)
     */
    virtual ocpp::types::CertificateStatusEnumType caCertificateReceived(ocpp::types::CertificateUseEnumType type,
                                                                         const ocpp::x509::Certificate&      certificate) = 0;

    /**
     * @brief Called when a charge point certificate has been received and must be installed
     * @param certificate Charge point certificate to install
     * @return true is the certificate has been installed, false otherwise
     */
    virtual bool chargePointCertificateReceived(const ocpp::x509::Certificate& certificate) = 0;

    /**
     * @brief Called when the Central System request to delete an installed CA certificate
     * @param hash_algorithm Hash algorithm used for the following parameters
     * @param issuer_name_hash Hash of the certificate's issuer's name
     * @param issuer_key_hash Hash of the certificate's public key
     * @param serial_number Serial number of the certificate
     * @return Deletion status (see DeleteCertificateStatusEnumType enum)
     */
    virtual ocpp::types::DeleteCertificateStatusEnumType deleteCertificate(ocpp::types::HashAlgorithmEnumType hash_algorithm,
                                                                           const std::string&                 issuer_name_hash,
                                                                           const std::string&                 issuer_key_hash,
                                                                           const std::string&                 serial_number) = 0;

    /**
     * @brief Called to generate a CSR in PEM format which will be used by the Central System
     *        to generate and sign a certificate for the Charge Point
     * @param csr String to store the generated CSR in PEM format
     */
    virtual void generateCsr(std::string& csr) = 0;

    /**
     * @brief Called to get the list of installed CA certificates
     * @param type Type of CA certificate
     * @param certificates Installed certificates
     */
    virtual void getInstalledCertificates(ocpp::types::CertificateUseEnumType type, std::vector<ocpp::x509::Certificate>& certificates) = 0;

    /**
     * @brief Called on a log request
     * @param type Type of log to upload
     * @param start_time If set, contains the date and time of the oldest logging information to
     *                   include in the log file
     * @param stop_time If set, contains the date and time of the latest logging information to
     *                  include in the log file
     * @return Path to the generated log file to upload to the Central System, or empty
     *         string if no log are available
     */
    virtual std::string getLog(ocpp::types::LogEnumType                            type,
                               const ocpp::types::Optional<ocpp::types::DateTime>& start_time,
                               const ocpp::types::Optional<ocpp::types::DateTime>& stop_time) = 0;

    /**
     * @brief Called to check if at least 1 Central System root certificate has been installed
     * @return true if at least 1 certificate has been installed, false otherwise
     */
    virtual bool hasCentralSystemCaCertificateInstalled() = 0;

    /**
     * @brief Called to check if at least 1 Charge Point certificate has been installed
     * @return true if at least 1 certificate has been installed, false otherwise
     */
    virtual bool hasChargePointCertificateInstalled() = 0;
};

} // namespace chargepoint
} // namespace ocpp

#endif // ICHARGEPOINTEVENTSHANDLER_H
