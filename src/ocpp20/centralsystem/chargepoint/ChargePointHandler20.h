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

#ifndef OPENOCPP_OCPP20_CS_CHARGEPOINTHANDLER20_H
#define OPENOCPP_OCPP20_CS_CHARGEPOINTHANDLER20_H

#include "Authorize20.h"
#include "BootNotification20.h"
#include "ClearedChargingLimit20.h"
#include "CostUpdated20.h"
#include "DataTransfer20.h"
#include "FirmwareStatusNotification20.h"
#include "GenericMessageHandler.h"
#include "Heartbeat20.h"
#include "LogStatusNotification20.h"
#include "Logger.h"
#include "MessagesConverter20.h"
#include "MeterValues20.h"
#include "NotifyChargingLimit20.h"
#include "NotifyCustomerInformation20.h"
#include "NotifyDisplayMessages20.h"
#include "NotifyEVChargingNeeds20.h"
#include "NotifyEVChargingSchedule20.h"
#include "NotifyEvent20.h"
#include "NotifyMonitoringReport20.h"
#include "NotifyReport20.h"
#include "PublishFirmwareStatusNotification20.h"
#include "ReportChargingProfiles20.h"
#include "ReservationStatusUpdate20.h"
#include "SecurityEventNotification20.h"
#include "SignCertificate20.h"
#include "StatusNotification20.h"
#include "TransactionEvent20.h"

namespace ocpp
{
namespace config
{
class ICentralSystemConfig20;
} // namespace config
namespace messages
{
class MessageDispatcher;

namespace ocpp20
{
class MessagesConverter20;
} // namespace ocpp20
} // namespace messages

namespace centralsystem
{
namespace ocpp20
{

class IChargePointRequestHandler20;

/** @brief Handler for charge point requests */
class ChargePointHandler20
    : public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::BootNotificationReq,
                                                   ocpp::messages::ocpp20::BootNotificationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::AuthorizeReq, ocpp::messages::ocpp20::AuthorizeConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::ClearedChargingLimitReq,
                                                   ocpp::messages::ocpp20::ClearedChargingLimitConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::CostUpdatedReq, ocpp::messages::ocpp20::CostUpdatedConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::DataTransferReq, ocpp::messages::ocpp20::DataTransferConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::FirmwareStatusNotificationReq,
                                                   ocpp::messages::ocpp20::FirmwareStatusNotificationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::HeartbeatReq, ocpp::messages::ocpp20::HeartbeatConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::LogStatusNotificationReq,
                                                   ocpp::messages::ocpp20::LogStatusNotificationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::MeterValuesReq, ocpp::messages::ocpp20::MeterValuesConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::NotifyChargingLimitReq,
                                                   ocpp::messages::ocpp20::NotifyChargingLimitConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::NotifyCustomerInformationReq,
                                                   ocpp::messages::ocpp20::NotifyCustomerInformationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::NotifyDisplayMessagesReq,
                                                   ocpp::messages::ocpp20::NotifyDisplayMessagesConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::NotifyEVChargingNeedsReq,
                                                   ocpp::messages::ocpp20::NotifyEVChargingNeedsConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::NotifyEVChargingScheduleReq,
                                                   ocpp::messages::ocpp20::NotifyEVChargingScheduleConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::NotifyEventReq, ocpp::messages::ocpp20::NotifyEventConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::NotifyMonitoringReportReq,
                                                   ocpp::messages::ocpp20::NotifyMonitoringReportConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::NotifyReportReq, ocpp::messages::ocpp20::NotifyReportConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::PublishFirmwareStatusNotificationReq,
                                                   ocpp::messages::ocpp20::PublishFirmwareStatusNotificationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::ReportChargingProfilesReq,
                                                   ocpp::messages::ocpp20::ReportChargingProfilesConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::ReservationStatusUpdateReq,
                                                   ocpp::messages::ocpp20::ReservationStatusUpdateConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::SecurityEventNotificationReq,
                                                   ocpp::messages::ocpp20::SecurityEventNotificationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::SignCertificateReq, ocpp::messages::ocpp20::SignCertificateConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::StatusNotificationReq,
                                                   ocpp::messages::ocpp20::StatusNotificationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::TransactionEventReq,
                                                   ocpp::messages::ocpp20::TransactionEventConf>
{
  public:
    /**
     * @brief Constructor
     * @param identifier Charge point's identifier
     * @param messages_converter Converter from/to OCPP to/from JSON messages
     * @param msg_dispatcher Message dispatcher
     */
    ChargePointHandler20(const std::string&                                 identifier,
                         const ocpp::messages::ocpp20::MessagesConverter20& messages_converter,
                         ocpp::messages::MessageDispatcher&                 msg_dispatcher);
    /** @brief Destructor */
    virtual ~ChargePointHandler20();

    /** @brief Register the event handler */
    void registerHandler(IChargePointRequestHandler20& handler) { m_handler = &handler; }

    // OCPP handlers
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::BootNotificationReq& request,
                       ocpp::messages::ocpp20::BootNotificationConf&      response,
                       std::string&                                       error_code,
                       std::string&                                       error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::AuthorizeReq& request,
                       ocpp::messages::ocpp20::AuthorizeConf&      response,
                       std::string&                                error_code,
                       std::string&                                error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::ClearedChargingLimitReq& request,
                       ocpp::messages::ocpp20::ClearedChargingLimitConf&      response,
                       std::string&                                           error_code,
                       std::string&                                           error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::CostUpdatedReq& request,
                       ocpp::messages::ocpp20::CostUpdatedConf&      response,
                       std::string&                                  error_code,
                       std::string&                                  error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::DataTransferReq& request,
                       ocpp::messages::ocpp20::DataTransferConf&      response,
                       std::string&                                   error_code,
                       std::string&                                   error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::FirmwareStatusNotificationReq& request,
                       ocpp::messages::ocpp20::FirmwareStatusNotificationConf&      response,
                       std::string&                                                 error_code,
                       std::string&                                                 error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::HeartbeatReq& request,
                       ocpp::messages::ocpp20::HeartbeatConf&      response,
                       std::string&                                error_code,
                       std::string&                                error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::LogStatusNotificationReq& request,
                       ocpp::messages::ocpp20::LogStatusNotificationConf&      response,
                       std::string&                                            error_code,
                       std::string&                                            error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::MeterValuesReq& request,
                       ocpp::messages::ocpp20::MeterValuesConf&      response,
                       std::string&                                  error_code,
                       std::string&                                  error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::NotifyChargingLimitReq& request,
                       ocpp::messages::ocpp20::NotifyChargingLimitConf&      response,
                       std::string&                                          error_code,
                       std::string&                                          error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::NotifyCustomerInformationReq& request,
                       ocpp::messages::ocpp20::NotifyCustomerInformationConf&      response,
                       std::string&                                                error_code,
                       std::string&                                                error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::NotifyDisplayMessagesReq& request,
                       ocpp::messages::ocpp20::NotifyDisplayMessagesConf&      response,
                       std::string&                                            error_code,
                       std::string&                                            error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::NotifyEVChargingNeedsReq& request,
                       ocpp::messages::ocpp20::NotifyEVChargingNeedsConf&      response,
                       std::string&                                            error_code,
                       std::string&                                            error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::NotifyEVChargingScheduleReq& request,
                       ocpp::messages::ocpp20::NotifyEVChargingScheduleConf&      response,
                       std::string&                                               error_code,
                       std::string&                                               error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::NotifyEventReq& request,
                       ocpp::messages::ocpp20::NotifyEventConf&      response,
                       std::string&                                  error_code,
                       std::string&                                  error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::NotifyMonitoringReportReq& request,
                       ocpp::messages::ocpp20::NotifyMonitoringReportConf&      response,
                       std::string&                                             error_code,
                       std::string&                                             error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::NotifyReportReq& request,
                       ocpp::messages::ocpp20::NotifyReportConf&      response,
                       std::string&                                   error_code,
                       std::string&                                   error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::PublishFirmwareStatusNotificationReq& request,
                       ocpp::messages::ocpp20::PublishFirmwareStatusNotificationConf&      response,
                       std::string&                                                        error_code,
                       std::string&                                                        error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::ReportChargingProfilesReq& request,
                       ocpp::messages::ocpp20::ReportChargingProfilesConf&      response,
                       std::string&                                             error_code,
                       std::string&                                             error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::ReservationStatusUpdateReq& request,
                       ocpp::messages::ocpp20::ReservationStatusUpdateConf&      response,
                       std::string&                                              error_code,
                       std::string&                                              error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::SecurityEventNotificationReq& request,
                       ocpp::messages::ocpp20::SecurityEventNotificationConf&      response,
                       std::string&                                                error_code,
                       std::string&                                                error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::SignCertificateReq& request,
                       ocpp::messages::ocpp20::SignCertificateConf&      response,
                       std::string&                                      error_code,
                       std::string&                                      error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::StatusNotificationReq& request,
                       ocpp::messages::ocpp20::StatusNotificationConf&      response,
                       std::string&                                         error_code,
                       std::string&                                         error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::TransactionEventReq& request,
                       ocpp::messages::ocpp20::TransactionEventConf&      response,
                       std::string&                                       error_code,
                       std::string&                                       error_message) override;

  private:
    /** @brief Charge point's identifier */
    const std::string m_identifier;
    /** @brief Request handler */
    IChargePointRequestHandler20* m_handler;
};

} // namespace ocpp20
} // namespace centralsystem
} // namespace ocpp

#endif // OPENOCPP_OCPP20_CS_CHARGEPOINTHANDLER20_H