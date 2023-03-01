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

#ifndef CHARGEPOINTEVENTSHANDLERSTUB_H
#define CHARGEPOINTEVENTSHANDLERSTUB_H

#include "IChargePoint.h"
#include "IChargePointEventsHandler.h"

#include <map>

/** @brief Charge point event handlers stub for unit tests */
class ChargePointEventsHandlerStub : public ocpp::chargepoint::IChargePointEventsHandler
{
  public:
    /** @brief Constructor */
    ChargePointEventsHandlerStub();
    /** @brief Destructor */
    virtual ~ChargePointEventsHandlerStub();

    // IChargePointEventsHandler interface

    /** @copydoc void IChargePointEventsHandler::connectionStateChanged(ocpp::types::RegistrationStatus) */
    void connectionFailed(ocpp::types::RegistrationStatus status) override;

    /** @copydoc void IChargePointEventsHandler::connectionStateChanged(bool) */
    void connectionStateChanged(bool isConnected) override;

    /** @copydoc void IChargePointEventsHandler::bootNotification(ocpp::types::RegistrationStatus, const ocpp::types::DateTime&) */
    void bootNotification(ocpp::types::RegistrationStatus status, const ocpp::types::DateTime& datetime) override;

    /** @copydoc void IChargePointEventsHandler::datetimeReceived(const ocpp::types::DateTime&) */
    void datetimeReceived(const ocpp::types::DateTime& datetime) override;

    /** @copydoc AvailabilityStatus IChargePointEventsHandler::changeAvailabilityRequested(unsigned int, ocpp::types::AvailabilityType) */
    ocpp::types::AvailabilityStatus changeAvailabilityRequested(unsigned int                  connector_id,
                                                                ocpp::types::AvailabilityType availability) override;

    /** @copydoc int IChargePointEventsHandler::getTxStartStopMeterValue(unsigned int) */
    int getTxStartStopMeterValue(unsigned int connector_id) override;

    /** @copydoc void IChargePointEventsHandler::reservationStarted(unsigned int) */
    void reservationStarted(unsigned int connector_id) override;

    /** @copydoc void IChargePointEventsHandler::reservationEnded(unsigned int, bool) */
    void reservationEnded(unsigned int connector_id, bool canceled) override;

    /** @copydoc ocpp::types::DataTransferStatus IChargePointEventsHandler::dataTransferRequested(const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  std::string&) */
    ocpp::types::DataTransferStatus dataTransferRequested(const std::string& vendor_id,
                                                          const std::string& message_id,
                                                          const std::string& request_data,
                                                          std::string&       response_data) override;

    /** @copydoc bool IChargePointEventsHandler::getMeterValue(unsigned int, const std::pair<ocpp::types::Measurand, ocpp::types::Optional<ocpp::types::Phase>>&, ocpp::types::MeterValue&) */
    bool getMeterValue(unsigned int                                                                        connector_id,
                       const std::pair<ocpp::types::Measurand, ocpp::types::Optional<ocpp::types::Phase>>& measurand,
                       ocpp::types::MeterValue&                                                            meter_value) override;

    /** @copydoc bool IChargePointEventsHandler::remoteStartTransactionRequested(unsigned int, const std::string&) */
    bool remoteStartTransactionRequested(unsigned int connector_id, const std::string& id_tag) override;

    /** @copydoc bool IChargePointEventsHandler::remoteStopTransactionRequested(unsigned int) */
    bool remoteStopTransactionRequested(unsigned int connector_id) override;

    /** @copydoc void IChargePointEventsHandler::transactionDeAuthorized(unsigned int) */
    void transactionDeAuthorized(unsigned int connector_id) override;

    /** @copydoc bool IChargePointEventsHandler::getLocalLimitationsSchedule(unsigned int, unsigned int, ocpp::types::ChargingSchedule&) */
    bool getLocalLimitationsSchedule(unsigned int connector_id, unsigned int duration, ocpp::types::ChargingSchedule& schedule) override;

    /** @copydoc bool IChargePointEventsHandler::resetRequested(ocpp::types::ResetType) */
    bool resetRequested(ocpp::types::ResetType reset_type) override;

    /** @copydoc ocpp::types::UnlockStatus IChargePointEventsHandler::unlockConnectorRequested(unsigned int) */
    ocpp::types::UnlockStatus unlockConnectorRequested(unsigned int connector_id) override;

    /** @copydoc std::string IChargePointEventsHandler::getDiagnostics(const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                                       const ocpp::types::Optional<ocpp::types::DateTime>&) */
    std::string getDiagnostics(const ocpp::types::Optional<ocpp::types::DateTime>& start_time,
                               const ocpp::types::Optional<ocpp::types::DateTime>& stop_time) override;

    /** @copydoc std::string IChargePointEventsHandler::updateFirmwareRequested() */
    std::string updateFirmwareRequested() override;

    /** @copydoc void IChargePointEventsHandler::installFirmware() */
    void installFirmware(const std::string& firmware_file) override;

    /** @copydoc bool IChargePointEventsHandler::uploadFile(const std::string&, const std::string&) */
    bool uploadFile(const std::string& file, const std::string& url) override;

    /** @copydoc bool IChargePointEventsHandler::downloadFile(const std::string&, const std::string&) */
    bool downloadFile(const std::string& url, const std::string& file) override;

    // Security extensions

    /** @copydoc ocpp::types::CertificateStatusEnumType IChargePointEventsHandler::caCertificateReceived(ocpp::types::CertificateUseEnumType,
                                                                                                         const ocpp::x509::Certificate&) */
    ocpp::types::CertificateStatusEnumType caCertificateReceived(ocpp::types::CertificateUseEnumType type,
                                                                 const ocpp::x509::Certificate&      certificate) override;

    /** @copydoc bool IChargePointEventsHandler::chargePointCertificateReceived(const ocpp::x509::Certificate&) */
    bool chargePointCertificateReceived(const ocpp::x509::Certificate& certificate) override;

    /** @copydoc ocpp::types::DeleteCertificateStatusEnumType IChargePointEventsHandler::deleteCertificate(ocpp::types::HashAlgorithmEnumType,
                                                                                                           const std::string&,
                                                                                                           const std::string&,
                                                                                                           const std::string&) */
    ocpp::types::DeleteCertificateStatusEnumType deleteCertificate(ocpp::types::HashAlgorithmEnumType hash_algorithm,
                                                                   const std::string&                 issuer_name_hash,
                                                                   const std::string&                 issuer_key_hash,
                                                                   const std::string&                 serial_number) override;

    /** @copydoc void IChargePointEventsHandler::generateCsr(std::string&) */
    void generateCsr(std::string& csr) override;

    /** @copydoc void IChargePointEventsHandler::getInstalledCertificates(ocpp::types::CertificateUseEnumType,
     *                                                                    std::vector<ocpp::x509::Certificate>&) */
    void getInstalledCertificates(ocpp::types::CertificateUseEnumType type, std::vector<ocpp::x509::Certificate>& certificates) override;

    /** @copydoc std::string IChargePointEventsHandler::getLog(ocpp::types::LogEnumType,
                                                               const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                               const ocpp::types::Optional<ocpp::types::DateTime>&) */
    std::string getLog(ocpp::types::LogEnumType                            type,
                       const ocpp::types::Optional<ocpp::types::DateTime>& start_time,
                       const ocpp::types::Optional<ocpp::types::DateTime>& stop_time) override;

    /** @copydoc bool IChargePointEventsHandler::hasCentralSystemCaCertificateInstalled() */
    bool hasCentralSystemCaCertificateInstalled() override;

    /** @copydoc bool IChargePointEventsHandler::hasChargePointCertificateInstalled() */
    bool hasChargePointCertificateInstalled() override;

    /** @copydoc ocpp::types::UpdateFirmwareStatusEnumType IChargePointEventsHandler::checkFirmwareSigningCertificate(
     *                                            const ocpp::x509::Certificate&) */
    ocpp::types::UpdateFirmwareStatusEnumType checkFirmwareSigningCertificate(const ocpp::x509::Certificate& signing_certificate) override;

    // ISO 15118 PnC extensions

    /** @copydoc bool IChargePointEventsHandler::iso15118CheckEvCertificate(const ocpp::x509::Certificate&) */
    bool iso15118CheckEvCertificate(const ocpp::x509::Certificate& certificate) override;

    /** @copydoc bool IChargePointEventsHandler::iso15118ChargePointCertificateReceived(const ocpp::x509::Certificate&) */
    bool iso15118ChargePointCertificateReceived(const ocpp::x509::Certificate& certificate) override;

    /** @copydoc ocpp::types::DeleteCertificateStatusEnumType IChargePointEventsHandler::iso15118DeleteCertificate(ocpp::types::HashAlgorithmEnumType,
                                                                                                                   const std::string&,
                                                                                                                   const std::string&,
                                                                                                                   const std::string&) */
    ocpp::types::DeleteCertificateStatusEnumType iso15118DeleteCertificate(ocpp::types::HashAlgorithmEnumType hash_algorithm,
                                                                           const std::string&                 issuer_name_hash,
                                                                           const std::string&                 issuer_key_hash,
                                                                           const std::string&                 serial_number) override;

    /** @copydoc void IChargePointEventsHandler::iso15118GetInstalledCertificates(
                                    bool,
                                    bool,
                                    bool,
                                    std::vector<std::tuple<GetCertificateIdUseEnumType, Certificate, std::vector<Certificate>>>&) */
    void iso15118GetInstalledCertificates(
        bool v2g_root_certificate,
        bool mo_root_certificate,
        bool v2g_certificate_chain,
        std::vector<std::tuple<ocpp::types::GetCertificateIdUseEnumType, ocpp::x509::Certificate, std::vector<ocpp::x509::Certificate>>>&
            certificates) override;

    /** @copydoc ocpp::types::InstallCertificateStatusEnumType IChargePointEventsHandler::iso15118CertificateReceived(
                                    ocpp::types::InstallCertificateUseEnumType type,
                                    const ocpp::x509::Certificate&) */
    ocpp::types::InstallCertificateStatusEnumType iso15118CertificateReceived(ocpp::types::InstallCertificateUseEnumType type,
                                                                              const ocpp::x509::Certificate& certificate) override;

    /** @copydoc void IChargePointEventsHandler::iso15118GenerateCsr(std::string&) */
    void iso15118GenerateCsr(std::string& csr) override;

    // API

    /** @brief Indicate if a method has been called and returns the parameters used for the call */
    bool methodCalled(const std::string& method_name, std::map<std::string, std::string>& params);
    /** @brief Clear the call history */
    void clearCalls();

    /** @brief Call results for boolean methods */
    void setCallResult(const std::string method_name, bool result) { m_call_results[method_name] = result; }
    /** @brief Availability status to return */
    void setAvailabilityStatus(ocpp::types::AvailabilityStatus availability_status) { m_availability_status = availability_status; }
    /** @brief Tx start stop meter value */
    void setTxStartStopMeterValue(int tx_start_stop_value) { m_tx_start_stop_value = tx_start_stop_value; }
    /** @brief Data transfer status */
    void setDataTransferStatus(ocpp::types::DataTransferStatus data_transfer_status) { m_data_transfer_status = data_transfer_status; }
    /** @brief Unlock status */
    void setUnlockStatus(ocpp::types::UnlockStatus unlock_status) { m_unlock_status = unlock_status; }
    /** @brief Diagnostic / log file */
    void setDiagFile(std::string diag_file) { m_diag_file = diag_file; }
    /** @brief Set a meter value */
    void setMeterValue(unsigned int                                                                 connector_id,
                       std::pair<ocpp::types::Measurand, ocpp::types::Optional<ocpp::types::Phase>> measurand,
                       const ocpp::types::MeterValue&                                               meter_value);

  private:
    /** @brief Calls to the event handler */
    std::map<std::string, std::map<std::string, std::string>> m_calls;

    /** @brief Call results for boolean methods */
    std::map<std::string, bool> m_call_results;
    /** @brief Availability status to return */
    ocpp::types::AvailabilityStatus m_availability_status;
    /** @brief Tx start stop meter value */
    int m_tx_start_stop_value;
    /** @brief Data transfer status */
    ocpp::types::DataTransferStatus m_data_transfer_status;
    /** @brief Unlock status */
    ocpp::types::UnlockStatus m_unlock_status;
    /** @brief Diagnostic / log file */
    std::string m_diag_file;
    /** @brief Meter values */
    std::map<unsigned int, std::map<std::string, ocpp::types::MeterValue>> m_meter_values;

    /** @brief Get the call result of a method */
    bool callResult(const std::string method_name);
};

#endif // CHARGEPOINTEVENTSHANDLERSTUB_H
