/*
MIT License

Copyright (c) 2020 Cedric Jimenez

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef DEFAULTCHARGEPOINTEVENTSHANDLER_H
#define DEFAULTCHARGEPOINTEVENTSHANDLER_H

#include "IChargePoint.h"
#include "IChargePointEventsHandler.h"

#include <filesystem>
#include <vector>

class ChargePointDemoConfig;

/** @brief Default charge point event handlers implementation for the examples */
class DefaultChargePointEventsHandler : public ocpp::chargepoint::IChargePointEventsHandler
{
  public:
    /** @brief Constructor */
    DefaultChargePointEventsHandler(ChargePointDemoConfig& config, const std::filesystem::path& working_dir);

    /** @brief Destructor */
    virtual ~DefaultChargePointEventsHandler();

    /** @brief Set the associated Charge Point instance */
    void setChargePoint(ocpp::chargepoint::IChargePoint& chargepoint) { m_chargepoint = &chargepoint; }

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
     *                              ocpp::types::InstallCertificateUseEnumType type,
                                    const ocpp::x509::Certificate&) */
    ocpp::types::InstallCertificateStatusEnumType iso15118CertificateReceived(ocpp::types::InstallCertificateUseEnumType type,
                                                                              const ocpp::x509::Certificate& certificate) override;

    /** @copydoc void IChargePointEventsHandler::iso15118GenerateCsr(std::string&) */
    void iso15118GenerateCsr(std::string& csr) override;

    // API

    /** @brief Indicate a pending remote start transaction */
    bool isRemoteStartPending(unsigned int connector_id) const { return m_remote_start_pending[connector_id - 1u]; }

    /** @brief Reset the pending remote start transaction flag */
    void resetRemoteStartPending(unsigned int connector_id) { m_remote_start_pending[connector_id - 1u] = false; }

    /** @brief Indicate a pending remote stop transaction */
    bool isRemoteStopPending(unsigned int connector_id) const { return m_remote_stop_pending[connector_id - 1u]; }

    /** @brief Reset the pending remote stop transaction flag */
    void resetRemoteStopPending(unsigned int connector_id) { m_remote_stop_pending[connector_id - 1u] = false; }

    /** @brief Id tag for the remote start request */
    std::string remoteStartIdTag(unsigned int connector_id) const { return m_remote_start_id_tag[connector_id - 1u]; }

  protected:
    /** @brief Get the configuration */
    ChargePointDemoConfig& config() { return m_config; }

  private:
    /** @brief Configuration */
    ChargePointDemoConfig& m_config;
    /** @brief Associated Charge Point instance */
    ocpp::chargepoint::IChargePoint* m_chargepoint;
    /** @brief Working directory */
    std::filesystem::path m_working_dir;
    /** @brief Indicate a pending remote start transaction */
    std::vector<bool> m_remote_start_pending;
    /** @brief Indicate a pending remote stop transaction */
    std::vector<bool> m_remote_stop_pending;
    /** @brief Id tag for the remote start request */
    std::vector<std::string> m_remote_start_id_tag;

    /** @brief Get the number of installed CA certificates */
    unsigned int getNumberOfCaCertificateInstalled(bool manufacturer, bool central_system, bool iso15118);
};

#endif // DEFAULTCHARGEPOINTEVENTSHANDLER_H
