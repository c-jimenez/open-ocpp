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

        /** @brief Called on reception of a BootNotification20 request from the charge point */
        bool onBootNotification20(const ocpp::messages::ocpp20::BootNotification20Req& request,
                                  ocpp::messages::ocpp20::BootNotification20Conf&      response,
                                  std::string&                                         error,
                                  std::string&                                         message) override;

        /** @brief Called on reception of a Authorize20 request from the charge point */
        bool onAuthorize20(const ocpp::messages::ocpp20::Authorize20Req& request,
                           ocpp::messages::ocpp20::Authorize20Conf&      response,
                           std::string&                                  error,
                           std::string&                                  message) override;

        /** @brief Called on reception of a ClearedChargingLimit20 request from the charge point */
        bool onClearedChargingLimit20(const ocpp::messages::ocpp20::ClearedChargingLimit20Req& request,
                                      ocpp::messages::ocpp20::ClearedChargingLimit20Conf&      response,
                                      std::string&                                             error,
                                      std::string&                                             message) override;

        /** @brief Called on reception of a CostUpdated20 request from the charge point */
        bool onCostUpdated20(const ocpp::messages::ocpp20::CostUpdated20Req& request,
                             ocpp::messages::ocpp20::CostUpdated20Conf&      response,
                             std::string&                                    error,
                             std::string&                                    message) override;

        /** @brief Called on reception of a DataTransfer20 request from the charge point */
        bool onDataTransfer20(const ocpp::messages::ocpp20::DataTransfer20Req& request,
                              ocpp::messages::ocpp20::DataTransfer20Conf&      response,
                              std::string&                                     error,
                              std::string&                                     message) override;

        /** @brief Called on reception of a FirmwareStatusNotification20 request from the charge point */
        bool onFirmwareStatusNotification20(const ocpp::messages::ocpp20::FirmwareStatusNotification20Req& request,
                                            ocpp::messages::ocpp20::FirmwareStatusNotification20Conf&      response,
                                            std::string&                                                   error,
                                            std::string&                                                   message) override;

        /** @brief Called on reception of a Heartbeat20 request from the charge point */
        bool onHeartbeat20(const ocpp::messages::ocpp20::Heartbeat20Req& request,
                           ocpp::messages::ocpp20::Heartbeat20Conf&      response,
                           std::string&                                  error,
                           std::string&                                  message) override;

        /** @brief Called on reception of a LogStatusNotification20 request from the charge point */
        bool onLogStatusNotification20(const ocpp::messages::ocpp20::LogStatusNotification20Req& request,
                                       ocpp::messages::ocpp20::LogStatusNotification20Conf&      response,
                                       std::string&                                              error,
                                       std::string&                                              message) override;

        /** @brief Called on reception of a MeterValues20 request from the charge point */
        bool onMeterValues20(const ocpp::messages::ocpp20::MeterValues20Req& request,
                             ocpp::messages::ocpp20::MeterValues20Conf&      response,
                             std::string&                                    error,
                             std::string&                                    message) override;

        /** @brief Called on reception of a NotifyChargingLimit20 request from the charge point */
        bool onNotifyChargingLimit20(const ocpp::messages::ocpp20::NotifyChargingLimit20Req& request,
                                     ocpp::messages::ocpp20::NotifyChargingLimit20Conf&      response,
                                     std::string&                                            error,
                                     std::string&                                            message) override;

        /** @brief Called on reception of a NotifyCustomerInformation20 request from the charge point */
        bool onNotifyCustomerInformation20(const ocpp::messages::ocpp20::NotifyCustomerInformation20Req& request,
                                           ocpp::messages::ocpp20::NotifyCustomerInformation20Conf&      response,
                                           std::string&                                                  error,
                                           std::string&                                                  message) override;

        /** @brief Called on reception of a NotifyDisplayMessages20 request from the charge point */
        bool onNotifyDisplayMessages20(const ocpp::messages::ocpp20::NotifyDisplayMessages20Req& request,
                                       ocpp::messages::ocpp20::NotifyDisplayMessages20Conf&      response,
                                       std::string&                                              error,
                                       std::string&                                              message) override;

        /** @brief Called on reception of a NotifyEVChargingNeeds20 request from the charge point */
        bool onNotifyEVChargingNeeds20(const ocpp::messages::ocpp20::NotifyEVChargingNeeds20Req& request,
                                       ocpp::messages::ocpp20::NotifyEVChargingNeeds20Conf&      response,
                                       std::string&                                              error,
                                       std::string&                                              message) override;

        /** @brief Called on reception of a NotifyEVChargingSchedule20 request from the charge point */
        bool onNotifyEVChargingSchedule20(const ocpp::messages::ocpp20::NotifyEVChargingSchedule20Req& request,
                                          ocpp::messages::ocpp20::NotifyEVChargingSchedule20Conf&      response,
                                          std::string&                                                 error,
                                          std::string&                                                 message) override;

        /** @brief Called on reception of a NotifyEvent20 request from the charge point */
        bool onNotifyEvent20(const ocpp::messages::ocpp20::NotifyEvent20Req& request,
                             ocpp::messages::ocpp20::NotifyEvent20Conf&      response,
                             std::string&                                    error,
                             std::string&                                    message) override;

        /** @brief Called on reception of a NotifyMonitoringReport20 request from the charge point */
        bool onNotifyMonitoringReport20(const ocpp::messages::ocpp20::NotifyMonitoringReport20Req& request,
                                        ocpp::messages::ocpp20::NotifyMonitoringReport20Conf&      response,
                                        std::string&                                               error,
                                        std::string&                                               message) override;

        /** @brief Called on reception of a NotifyReport20 request from the charge point */
        bool onNotifyReport20(const ocpp::messages::ocpp20::NotifyReport20Req& request,
                              ocpp::messages::ocpp20::NotifyReport20Conf&      response,
                              std::string&                                     error,
                              std::string&                                     message) override;

        /** @brief Called on reception of a PublishFirmwareStatusNotification20 request from the charge point */
        bool onPublishFirmwareStatusNotification20(const ocpp::messages::ocpp20::PublishFirmwareStatusNotification20Req& request,
                                                   ocpp::messages::ocpp20::PublishFirmwareStatusNotification20Conf&      response,
                                                   std::string&                                                          error,
                                                   std::string&                                                          message) override;

        /** @brief Called on reception of a ReportChargingProfiles20 request from the charge point */
        bool onReportChargingProfiles20(const ocpp::messages::ocpp20::ReportChargingProfiles20Req& request,
                                        ocpp::messages::ocpp20::ReportChargingProfiles20Conf&      response,
                                        std::string&                                               error,
                                        std::string&                                               message) override;

        /** @brief Called on reception of a ReservationStatusUpdate20 request from the charge point */
        bool onReservationStatusUpdate20(const ocpp::messages::ocpp20::ReservationStatusUpdate20Req& request,
                                         ocpp::messages::ocpp20::ReservationStatusUpdate20Conf&      response,
                                         std::string&                                                error,
                                         std::string&                                                message) override;

        /** @brief Called on reception of a SecurityEventNotification20 request from the charge point */
        bool onSecurityEventNotification20(const ocpp::messages::ocpp20::SecurityEventNotification20Req& request,
                                           ocpp::messages::ocpp20::SecurityEventNotification20Conf&      response,
                                           std::string&                                                  error,
                                           std::string&                                                  message) override;

        /** @brief Called on reception of a SignCertificate20 request from the charge point */
        bool onSignCertificate20(const ocpp::messages::ocpp20::SignCertificate20Req& request,
                                 ocpp::messages::ocpp20::SignCertificate20Conf&      response,
                                 std::string&                                        error,
                                 std::string&                                        message) override;

        /** @brief Called on reception of a StatusNotification20 request from the charge point */
        bool onStatusNotification20(const ocpp::messages::ocpp20::StatusNotification20Req& request,
                                    ocpp::messages::ocpp20::StatusNotification20Conf&      response,
                                    std::string&                                           error,
                                    std::string&                                           message) override;

        /** @brief Called on reception of a TransactionEvent20 request from the charge point */
        bool onTransactionEvent20(const ocpp::messages::ocpp20::TransactionEvent20Req& request,
                                  ocpp::messages::ocpp20::TransactionEvent20Conf&      response,
                                  std::string&                                         error,
                                  std::string&                                         message) override;

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
