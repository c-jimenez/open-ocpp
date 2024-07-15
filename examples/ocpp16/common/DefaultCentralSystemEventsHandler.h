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

#ifndef DEFAULTCENTRALSYSTEMEVENTSHANDLER_H
#define DEFAULTCENTRALSYSTEMEVENTSHANDLER_H

#include "ICentralSystemEventsHandler.h"
#include "IChargePointRequestHandler.h"

#include <filesystem>
#include <map>
#include <mutex>

/** @brief Default central system event handlers implementation for the examples */
class DefaultCentralSystemEventsHandler : public ocpp::centralsystem::ICentralSystemEventsHandler
{
  public:
    /** @brief Constructor */
    DefaultCentralSystemEventsHandler(std::filesystem::path iso_v2g_root_ca    = "",
                                      std::filesystem::path iso_mo_root_ca     = "",
                                      bool                  set_pending_status = false);

    /** @brief Destructor */
    virtual ~DefaultCentralSystemEventsHandler();

    // ICentralSystemEventsHandler interface

    /** @copydoc bool ICentralSystemEventsHandler::acceptConnection(const char*) */
    bool acceptConnection(const char* ip_address) override;

    /** @copydoc void ICentralSystemEventsHandler::clientFailedToConnect(const char*) */
    void clientFailedToConnect(const char* ip_address) override;

    /** @copydoc bool ICentralSystemEventsHandler::checkCredentials(const std::string&, const std::string&) */
    bool checkCredentials(const std::string& chargepoint_id, const std::string& password) override;

    /** @copydoc bool ICentralSystemEventsHandler::chargePointConnected(std::shared_ptr<ICentralSystem::IChargePoint>) */
    void chargePointConnected(std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint> chargepoint) override;

    // API

    /** @brief Handle requests/events from charge points */
    class ChargePointRequestHandler : public ocpp::centralsystem::IChargePointRequestHandler
    {
      public:
        /** @brief Constructor */
        ChargePointRequestHandler(DefaultCentralSystemEventsHandler&                                  event_handler,
                                  std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint>& chargepoint);

        /** @brief Destructor */
        virtual ~ChargePointRequestHandler();

        /** @brief Get the charge point proxy */
        std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint> proxy() { return m_chargepoint; }

        /** @brief Get the path to the genrated certificate */
        const std::string& generatedCertificate() { return m_generated_certificate; }

        // IChargePointRequestHandler interface

        /** @copydoc void IChargePointRequestHandler::disconnected() */
        void disconnected() override;

        /** @copydoc ocpp::types::ocpp16::IdTagInfo IChargePointRequestHandler::authorize(const std::string&) */
        ocpp::types::ocpp16::IdTagInfo authorize(const std::string& id_tag) override;

        /** @copydoc ocpp::types::ocpp16::RegistrationStatus IChargePointRequestHandler::bootNotification(const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&) */
        ocpp::types::ocpp16::RegistrationStatus bootNotification(const std::string& model,
                                                                 const std::string& serial_number,
                                                                 const std::string& vendor,
                                                                 const std::string& firmware_version,
                                                                 const std::string& iccid,
                                                                 const std::string& imsi,
                                                                 const std::string& meter_serial_number,
                                                                 const std::string& meter_type) override;

        /** @copydoc ocpp::types::ocpp16::DataTransferStatus IChargePointRequestHandler::dataTransfer(const std::string&,
                                                                                              const std::string&,
                                                                                              const std::string&,
                                                                                              std::string&) */
        ocpp::types::ocpp16::DataTransferStatus dataTransfer(const std::string& vendor_id,
                                                             const std::string& message_id,
                                                             const std::string& request_data,
                                                             std::string&       response_data) override;

        /** @copydoc void IChargePointRequestHandler::diagnosticStatusNotification(ocpp::types::ocpp16::DiagnosticsStatus) */
        void diagnosticStatusNotification(ocpp::types::ocpp16::DiagnosticsStatus status) override;

        /** @copydoc void IChargePointRequestHandler::firmwareStatusNotification(ocpp::types::ocpp16::FirmwareStatus) */
        void firmwareStatusNotification(ocpp::types::ocpp16::FirmwareStatus status) override;

        /** @copydoc void IChargePointRequestHandler::meterValues(unsigned int,
                                                                  const ocpp::types::Optional<int>&,
                                                                  const std::vector<ocpp::types::ocpp16::MeterValue>&) */
        void meterValues(unsigned int                                        connector_id,
                         const ocpp::types::Optional<int>&                   transaction_id,
                         const std::vector<ocpp::types::ocpp16::MeterValue>& meter_values) override;

        /** @copydoc ocpp::types::ocpp16::IdTagInfo IChargePointRequestHandler::startTransaction(unsigned int,
                                                                                         const std::string&,
                                                                                         int,
                                                                                         const ocpp::types::Optional<int>&,
                                                                                         const ocpp::types::DateTime&,
                                                                                         int&) */
        ocpp::types::ocpp16::IdTagInfo startTransaction(unsigned int                      connector_id,
                                                        const std::string&                id_tag,
                                                        int                               meter_start,
                                                        const ocpp::types::Optional<int>& reservation_id,
                                                        const ocpp::types::DateTime&      timestamp,
                                                        int&                              transaction_id) override;

        /** @copydoc void IChargePointRequestHandler::statusNotification(unsigned int,
                                                                         ocpp::types::ocpp16::ChargePointErrorCode,
                                                                         const std::string&,
                                                                         ocpp::types::ocpp16::ChargePointStatus,
                                                                         const ocpp::types::DateTime&,
                                                                         const std::string&,
                                                                         const std::string&) */
        void statusNotification(unsigned int                              connector_id,
                                ocpp::types::ocpp16::ChargePointErrorCode error_code,
                                const std::string&                        info,
                                ocpp::types::ocpp16::ChargePointStatus    status,
                                const ocpp::types::DateTime&              timestamp,
                                const std::string&                        vendor_id,
                                const std::string&                        vendor_error) override;

        /** @copydoc ocpp::types::Optional<ocpp::types::ocpp16::IdTagInfo> IChargePointRequestHandler::stopTransaction(
                                                                                const std::string&,
                                                                                int,
                                                                                const ocpp::types::DateTime&,
                                                                                int,
                                                                                ocpp::types::ocpp16::Reason,
                                                                                const std::vector<ocpp::types::ocpp16::MeterValue>&) */
        ocpp::types::Optional<ocpp::types::ocpp16::IdTagInfo> stopTransaction(
            const std::string&                                  id_tag,
            int                                                 meter_stop,
            const ocpp::types::DateTime&                        timestamp,
            int                                                 transaction_id,
            ocpp::types::ocpp16::Reason                         reason,
            const std::vector<ocpp::types::ocpp16::MeterValue>& transaction_data) override;

        // Security extensions

        /** @copydoc void IChargePointRequestHandler::logStatusNotification(ocpp::types::ocpp16::UploadLogStatusEnumType, const ocpp::types::Optional<int>&) */
        void logStatusNotification(ocpp::types::ocpp16::UploadLogStatusEnumType status,
                                   const ocpp::types::Optional<int>&            request_id) override;

        /** @copydoc void IChargePointRequestHandler::securityEventNotification(const std::string&,
                                                                                const ocpp::types::DateTime&,
                                                                                const std::string&) */
        void securityEventNotification(const std::string&           type,
                                       const ocpp::types::DateTime& timestamp,
                                       const std::string&           message) override;

        /** @copydoc bool IChargePointRequestHandler::signCertificate(const ocpp::x509::CertificateRequest&) */
        bool signCertificate(const ocpp::x509::CertificateRequest& certificate_request) override;

        /** @copydoc void IChargePointRequestHandler::signedFirmwareUpdateStatusNotification(ocpp::types::ocpp16::FirmwareStatusEnumType,
                                                                                             const ocpp::types::Optional<int>&) */
        void signedFirmwareUpdateStatusNotification(ocpp::types::ocpp16::FirmwareStatusEnumType status,
                                                    const ocpp::types::Optional<int>&           request_id) override;

        // ISO 15118 PnC extensions

        /** @copydoc ocpp::types::ocpp16::IdTokenInfoType IChargePointRequestHandler::iso15118Authorize(
                                                          const ocpp::x509::Certificate&,
                                                          const std::string&,
                                                          const std::vector<ocpp::types::ocpp16::OcspRequestDataType>&,
                                                          ocpp::types::Optional<ocpp::types::ocpp16::AuthorizeCertificateStatusEnumType>&) override; */
        ocpp::types::ocpp16::IdTokenInfoType iso15118Authorize(
            const ocpp::x509::Certificate&                                                  certificate,
            const std::string&                                                              id_token,
            const std::vector<ocpp::types::ocpp16::OcspRequestDataType>&                    cert_hash_data,
            ocpp::types::Optional<ocpp::types::ocpp16::AuthorizeCertificateStatusEnumType>& cert_status) override;

        /** @copydoc ocpp::types::ocpp16::Iso15118EVCertificateStatusEnumType IChargePointRequestHandler::iso15118GetEVCertificate(
                                                          const std::string&,
                                                          ocpp::types::ocpp16::CertificateActionEnumType,
                                                          const std::string&,
                                                          std::string&) */
        ocpp::types::ocpp16::Iso15118EVCertificateStatusEnumType iso15118GetEVCertificate(
            const std::string&                             iso15118_schema_version,
            ocpp::types::ocpp16::CertificateActionEnumType action,
            const std::string&                             exi_request,
            std::string&                                   exi_response) override;

        /** @copydoc ocpp::types::ocpp16::GetCertificateStatusEnumType IChargePointRequestHandler::iso15118GetCertificateStatus(
                                                          const ocpp::types::ocpp16::OcspRequestDataType&,
                                                          std::string&) */
        ocpp::types::ocpp16::GetCertificateStatusEnumType iso15118GetCertificateStatus(
            const ocpp::types::ocpp16::OcspRequestDataType& ocsp_request, std::string& ocsp_result) override;

        /** @copydoc bool iso15118SignCertificate(const ocpp::x509::CertificateRequest&) */
        bool iso15118SignCertificate(const ocpp::x509::CertificateRequest& certificate_request) override;

      protected:
        /** @brief Get the serial number of the charge point */
        virtual std::string getChargePointSerialNumber(const std::string& chargepoint_id)
        {
            (void)chargepoint_id;
            return "";
        }

      private:
        /** @brief Event handler */
        DefaultCentralSystemEventsHandler& m_event_handler;

        /** @brief Charge point proxy */
        std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint> m_chargepoint;

        /** @brief Path to the generated certificate */
        std::string m_generated_certificate;
    };

    /** @brief Get the number connected charge points */
    size_t chargePointsCount()
    {
        std::lock_guard<std::mutex> lock(m_chargepoints_mutex);
        return m_chargepoints.size();
    }

    /** @brief Get the list of the connected charge points */
    std::map<std::string, std::shared_ptr<ChargePointRequestHandler>> chargePoints()
    {
        std::lock_guard<std::mutex> lock(m_chargepoints_mutex);
        return m_chargepoints;
    }

    /** @brief Get the list of the pending charge points */
    std::map<std::string, std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint>> pendingChargePoints()
    {
        std::lock_guard<std::mutex> lock(m_chargepoints_mutex);
        return m_pending_chargepoints;
    }

    /** @brief Path to the V2G root CA */
    std::filesystem::path& v2gRootCA() { return m_iso_v2g_root_ca; }
    /** @brief Path to the MO root CA */
    std::filesystem::path& moRootCA() { return m_iso_mo_root_ca; }

    /** @brief Indicate if the charge point must be set on pending status upon connection */
    bool setPendingEnabled() const { return m_set_pending_status; }

    /** @brief Remove a charge point from the connected charge points */
    void removeChargePoint(const std::string& identifier);

    /** @brief Indicate if a charge point must be accepted */
    bool isAcceptedChargePoint(const std::string& identifier);

    /** @brief Add a charge point to the pending list */
    void addPendingChargePoint(std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint> chargepoint);

    /** @brief Add a charge point to the accepted list */
    void addAcceptedChargePoint(std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint> chargepoint);

  protected:
    /** @brief Mutex for charge point list */
    std::mutex m_chargepoints_mutex;
    /** @brief Path to the V2G root CA */
    std::filesystem::path m_iso_v2g_root_ca;
    /** @brief Path to the MO root CA */
    std::filesystem::path m_iso_mo_root_ca;
    /** @brief Indicate if the charge point must be set on pending status upon connection */
    bool m_set_pending_status;
    /** @brief Connected charge points */
    std::map<std::string, std::shared_ptr<ChargePointRequestHandler>> m_chargepoints;
    /** @brief Pending charge points */
    std::map<std::string, std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint>> m_pending_chargepoints;
    /** @brief Accepted charge points */
    std::map<std::string, std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint>> m_accepted_chargepoints;
};

#endif // DEFAULTCENTRALSYSTEMEVENTSHANDLER_H
