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

#include "CentralSystemDemoConfig.h"
#include "ICentralSystemEventsHandler20.h"
#include "IChargePointRequestHandler20.h"

#include <filesystem>
#include <map>
#include <mutex>

/** @brief Default central system event handlers implementation for the examples */
class DefaultCentralSystemEventsHandler : public ocpp::centralsystem::ocpp20::ICentralSystemEventsHandler20
{
  public:
    /** @brief Constructor */
    DefaultCentralSystemEventsHandler(CentralSystemDemoConfig& config,
                                      std::filesystem::path    iso_v2g_root_ca    = "",
                                      std::filesystem::path    iso_mo_root_ca     = "",
                                      bool                     set_pending_status = false);

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
    void chargePointConnected(std::shared_ptr<ocpp::centralsystem::ocpp20::ICentralSystem20::IChargePoint20> chargepoint) override;

    // API

    /** @brief Handle requests/events from charge points */
    class ChargePointRequestHandler : public ocpp::centralsystem::ocpp20::IChargePointRequestHandler20
    {
      public:
        /** @brief Constructor */
        ChargePointRequestHandler(DefaultCentralSystemEventsHandler&                                              event_handler,
                                  std::shared_ptr<ocpp::centralsystem::ocpp20::ICentralSystem20::IChargePoint20>& chargepoint);

        /** @brief Destructor */
        virtual ~ChargePointRequestHandler();

        /** @brief Get the charge point proxy */
        std::shared_ptr<ocpp::centralsystem::ocpp20::ICentralSystem20::IChargePoint20> proxy() { return m_chargepoint; }

        /** @brief Get the path to the genrated certificate */
        const std::string& generatedCertificate() { return m_generated_certificate; }

        // IChargePointRequestHandler20 interface

        /** @brief Called to notify the disconnection of the charge point */
        void disconnected() override;

        /** @brief Called on reception of a BootNotification request from the charge point */
        bool onBootNotification(const ocpp::messages::ocpp20::BootNotificationReq& request,
                                ocpp::messages::ocpp20::BootNotificationConf&      response,
                                std::string&                                       error,
                                std::string&                                       message) override;

        /** @brief Called on reception of a Authorize request from the charge point */
        bool onAuthorize(const ocpp::messages::ocpp20::AuthorizeReq& request,
                         ocpp::messages::ocpp20::AuthorizeConf&      response,
                         std::string&                                error,
                         std::string&                                message) override;

        /** @brief Called on reception of a ClearedChargingLimit request from the charge point */
        bool onClearedChargingLimit(const ocpp::messages::ocpp20::ClearedChargingLimitReq& request,
                                    ocpp::messages::ocpp20::ClearedChargingLimitConf&      response,
                                    std::string&                                           error,
                                    std::string&                                           message) override;

        /** @brief Called on reception of a CostUpdated request from the charge point */
        bool onCostUpdated(const ocpp::messages::ocpp20::CostUpdatedReq& request,
                           ocpp::messages::ocpp20::CostUpdatedConf&      response,
                           std::string&                                  error,
                           std::string&                                  message) override;

        /** @brief Called on reception of a DataTransfer request from the charge point */
        bool onDataTransfer(const ocpp::messages::ocpp20::DataTransferReq& request,
                            ocpp::messages::ocpp20::DataTransferConf&      response,
                            std::string&                                   error,
                            std::string&                                   message) override;

        /** @brief Called on reception of a FirmwareStatusNotification request from the charge point */
        bool onFirmwareStatusNotification(const ocpp::messages::ocpp20::FirmwareStatusNotificationReq& request,
                                          ocpp::messages::ocpp20::FirmwareStatusNotificationConf&      response,
                                          std::string&                                                 error,
                                          std::string&                                                 message) override;

        /** @brief Called on reception of a Heartbeat request from the charge point */
        bool onHeartbeat(const ocpp::messages::ocpp20::HeartbeatReq& request,
                         ocpp::messages::ocpp20::HeartbeatConf&      response,
                         std::string&                                error,
                         std::string&                                message) override;

        /** @brief Called on reception of a LogStatusNotification request from the charge point */
        bool onLogStatusNotification(const ocpp::messages::ocpp20::LogStatusNotificationReq& request,
                                     ocpp::messages::ocpp20::LogStatusNotificationConf&      response,
                                     std::string&                                            error,
                                     std::string&                                            message) override;

        /** @brief Called on reception of a MeterValues request from the charge point */
        bool onMeterValues(const ocpp::messages::ocpp20::MeterValuesReq& request,
                           ocpp::messages::ocpp20::MeterValuesConf&      response,
                           std::string&                                  error,
                           std::string&                                  message) override;

        /** @brief Called on reception of a NotifyChargingLimit request from the charge point */
        bool onNotifyChargingLimit(const ocpp::messages::ocpp20::NotifyChargingLimitReq& request,
                                   ocpp::messages::ocpp20::NotifyChargingLimitConf&      response,
                                   std::string&                                          error,
                                   std::string&                                          message) override;

        /** @brief Called on reception of a NotifyCustomerInformation request from the charge point */
        bool onNotifyCustomerInformation(const ocpp::messages::ocpp20::NotifyCustomerInformationReq& request,
                                         ocpp::messages::ocpp20::NotifyCustomerInformationConf&      response,
                                         std::string&                                                error,
                                         std::string&                                                message) override;

        /** @brief Called on reception of a NotifyDisplayMessages request from the charge point */
        bool onNotifyDisplayMessages(const ocpp::messages::ocpp20::NotifyDisplayMessagesReq& request,
                                     ocpp::messages::ocpp20::NotifyDisplayMessagesConf&      response,
                                     std::string&                                            error,
                                     std::string&                                            message) override;

        /** @brief Called on reception of a NotifyEVChargingNeeds request from the charge point */
        bool onNotifyEVChargingNeeds(const ocpp::messages::ocpp20::NotifyEVChargingNeedsReq& request,
                                     ocpp::messages::ocpp20::NotifyEVChargingNeedsConf&      response,
                                     std::string&                                            error,
                                     std::string&                                            message) override;

        /** @brief Called on reception of a NotifyEVChargingSchedule request from the charge point */
        bool onNotifyEVChargingSchedule(const ocpp::messages::ocpp20::NotifyEVChargingScheduleReq& request,
                                        ocpp::messages::ocpp20::NotifyEVChargingScheduleConf&      response,
                                        std::string&                                               error,
                                        std::string&                                               message) override;

        /** @brief Called on reception of a NotifyEvent request from the charge point */
        bool onNotifyEvent(const ocpp::messages::ocpp20::NotifyEventReq& request,
                           ocpp::messages::ocpp20::NotifyEventConf&      response,
                           std::string&                                  error,
                           std::string&                                  message) override;

        /** @brief Called on reception of a NotifyMonitoringReport request from the charge point */
        bool onNotifyMonitoringReport(const ocpp::messages::ocpp20::NotifyMonitoringReportReq& request,
                                      ocpp::messages::ocpp20::NotifyMonitoringReportConf&      response,
                                      std::string&                                             error,
                                      std::string&                                             message) override;

        /** @brief Called on reception of a NotifyReport request from the charge point */
        bool onNotifyReport(const ocpp::messages::ocpp20::NotifyReportReq& request,
                            ocpp::messages::ocpp20::NotifyReportConf&      response,
                            std::string&                                   error,
                            std::string&                                   message) override;

        /** @brief Called on reception of a PublishFirmwareStatusNotification request from the charge point */
        bool onPublishFirmwareStatusNotification(const ocpp::messages::ocpp20::PublishFirmwareStatusNotificationReq& request,
                                                 ocpp::messages::ocpp20::PublishFirmwareStatusNotificationConf&      response,
                                                 std::string&                                                        error,
                                                 std::string&                                                        message) override;

        /** @brief Called on reception of a ReportChargingProfiles request from the charge point */
        bool onReportChargingProfiles(const ocpp::messages::ocpp20::ReportChargingProfilesReq& request,
                                      ocpp::messages::ocpp20::ReportChargingProfilesConf&      response,
                                      std::string&                                             error,
                                      std::string&                                             message) override;

        /** @brief Called on reception of a ReservationStatusUpdate request from the charge point */
        bool onReservationStatusUpdate(const ocpp::messages::ocpp20::ReservationStatusUpdateReq& request,
                                       ocpp::messages::ocpp20::ReservationStatusUpdateConf&      response,
                                       std::string&                                              error,
                                       std::string&                                              message) override;

        /** @brief Called on reception of a SecurityEventNotification request from the charge point */
        bool onSecurityEventNotification(const ocpp::messages::ocpp20::SecurityEventNotificationReq& request,
                                         ocpp::messages::ocpp20::SecurityEventNotificationConf&      response,
                                         std::string&                                                error,
                                         std::string&                                                message) override;

        /** @brief Called on reception of a SignCertificate request from the charge point */
        bool onSignCertificate(const ocpp::messages::ocpp20::SignCertificateReq& request,
                               ocpp::messages::ocpp20::SignCertificateConf&      response,
                               std::string&                                      error,
                               std::string&                                      message) override;

        /** @brief Called on reception of a StatusNotification request from the charge point */
        bool onStatusNotification(const ocpp::messages::ocpp20::StatusNotificationReq& request,
                                  ocpp::messages::ocpp20::StatusNotificationConf&      response,
                                  std::string&                                         error,
                                  std::string&                                         message) override;

        /** @brief Called on reception of a TransactionEvent request from the charge point */
        bool onTransactionEvent(const ocpp::messages::ocpp20::TransactionEventReq& request,
                                ocpp::messages::ocpp20::TransactionEventConf&      response,
                                std::string&                                       error,
                                std::string&                                       message) override;

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
        std::shared_ptr<ocpp::centralsystem::ocpp20::ICentralSystem20::IChargePoint20> m_chargepoint;

        /** @brief Path to the generated certificate */
        std::string m_generated_certificate;
    };

    /** @brief Get the central system's configuration */
    CentralSystemDemoConfig& getConfig() { return m_config; }

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
    std::map<std::string, std::shared_ptr<ocpp::centralsystem::ocpp20::ICentralSystem20::IChargePoint20>> pendingChargePoints()
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
    void addPendingChargePoint(std::shared_ptr<ocpp::centralsystem::ocpp20::ICentralSystem20::IChargePoint20> chargepoint);

    /** @brief Add a charge point to the accepted list */
    void addAcceptedChargePoint(std::shared_ptr<ocpp::centralsystem::ocpp20::ICentralSystem20::IChargePoint20> chargepoint);

  protected:
    /** @brief Central system's configuration */
    CentralSystemDemoConfig& m_config;
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
    std::map<std::string, std::shared_ptr<ocpp::centralsystem::ocpp20::ICentralSystem20::IChargePoint20>> m_pending_chargepoints;
    /** @brief Accepted charge points */
    std::map<std::string, std::shared_ptr<ocpp::centralsystem::ocpp20::ICentralSystem20::IChargePoint20>> m_accepted_chargepoints;
};

#endif // DEFAULTCENTRALSYSTEMEVENTSHANDLER_H
