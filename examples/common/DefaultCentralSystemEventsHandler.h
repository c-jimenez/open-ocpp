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

#include <map>

/** @brief Default central system event handlers implementation for the examples */
class DefaultCentralSystemEventsHandler : public ocpp::centralsystem::ICentralSystemEventsHandler
{
  public:
    /** @brief Constructor */
    DefaultCentralSystemEventsHandler();

    /** @brief Destructor */
    virtual ~DefaultCentralSystemEventsHandler();

    // ICentralSystemEventsHandler interface

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

        // IChargePointRequestHandler interface

        /** @copydoc void IChargePointRequestHandler::disconnected() */
        void disconnected() override;

        /** @copydoc ocpp::types::IdTagInfo IChargePointRequestHandler::authorize(const std::string&) */
        ocpp::types::IdTagInfo authorize(const std::string& id_tag) override;

        /** @copydoc ocpp::types::RegistrationStatus IChargePointRequestHandler::bootNotification(const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&) */
        ocpp::types::RegistrationStatus bootNotification(const std::string& model,
                                                         const std::string& serial_number,
                                                         const std::string& vendor,
                                                         const std::string& firmware_version,
                                                         const std::string& iccid,
                                                         const std::string& imsi,
                                                         const std::string& meter_serial_number,
                                                         const std::string& meter_type) override;

        /** @copydoc ocpp::types::DataTransferStatus IChargePointRequestHandler::dataTransfer(const std::string&,
                                                                                              const std::string&,
                                                                                              const std::string&,
                                                                                              std::string&) */
        ocpp::types::DataTransferStatus dataTransfer(const std::string& vendor_id,
                                                     const std::string& message_id,
                                                     const std::string& request_data,
                                                     std::string&       response_data) override;

        /** @copydoc void IChargePointRequestHandler::diagnosticStatusNotification(ocpp::types::DiagnosticsStatus) */
        void diagnosticStatusNotification(ocpp::types::DiagnosticsStatus status) override;

        /** @copydoc void IChargePointRequestHandler::firmwareStatusNotification(ocpp::types::FirmwareStatus) */
        void firmwareStatusNotification(ocpp::types::FirmwareStatus status) override;

        /** @copydoc void IChargePointRequestHandler::meterValues(unsigned int,
                                                                  const ocpp::types::Optional<int>&,
                                                                  const std::vector<ocpp::types::MeterValue>&) */
        void meterValues(unsigned int                                connector_id,
                         const ocpp::types::Optional<int>&           transaction_id,
                         const std::vector<ocpp::types::MeterValue>& meter_values) override;

        /** @copydoc ocpp::types::IdTagInfo IChargePointRequestHandler::startTransaction(unsigned int,
                                                                                         const std::string&,
                                                                                         int,
                                                                                         const ocpp::types::Optional<int>&,
                                                                                         const ocpp::types::DateTime&,
                                                                                         int&) */
        ocpp::types::IdTagInfo startTransaction(unsigned int                      connector_id,
                                                const std::string&                id_tag,
                                                int                               meter_start,
                                                const ocpp::types::Optional<int>& reservation_id,
                                                const ocpp::types::DateTime&      timestamp,
                                                int&                              transaction_id) override;

        /** @copydoc void IChargePointRequestHandler::statusNotification(unsigned int,
                                                                         ocpp::types::ChargePointErrorCode,
                                                                         const std::string&,
                                                                         ocpp::types::ChargePointStatus,
                                                                         const ocpp::types::DateTime&,
                                                                         const std::string&,
                                                                         const std::string&) */
        void statusNotification(unsigned int                      connector_id,
                                ocpp::types::ChargePointErrorCode error_code,
                                const std::string&                info,
                                ocpp::types::ChargePointStatus    status,
                                const ocpp::types::DateTime&      timestamp,
                                const std::string&                vendor_id,
                                const std::string&                vendor_error) override;

        /** @copydoc ocpp::types::Optional<ocpp::types::IdTagInfo> IChargePointRequestHandler::stopTransaction(
                                                                                const std::string&,
                                                                                int,
                                                                                const ocpp::types::DateTime&,
                                                                                int,
                                                                                ocpp::types::Reason,
                                                                                const std::vector<ocpp::types::MeterValue>&) */
        ocpp::types::Optional<ocpp::types::IdTagInfo> stopTransaction(
            const std::string&                          id_tag,
            int                                         meter_stop,
            const ocpp::types::DateTime&                timestamp,
            int                                         transaction_id,
            ocpp::types::Reason                         reason,
            const std::vector<ocpp::types::MeterValue>& transaction_data) override;

      private:
        /** @brief Event handler */
        DefaultCentralSystemEventsHandler& m_event_handler;

        /** @brief Charge point proxy */
        std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint> m_chargepoint;
    };

    /** @brief Get the list of the connected charge points */
    std::map<std::string, std::shared_ptr<ChargePointRequestHandler>>& chargePoints() { return m_chargepoints; }

    /** @brief Remove a charge point from the connected charge points */
    void removeChargePoint(const std::string& identifier);

  private:
    /** @brief Connected charge points */
    std::map<std::string, std::shared_ptr<ChargePointRequestHandler>> m_chargepoints;
};

#endif // DEFAULTCENTRALSYSTEMEVENTSHANDLER_H
