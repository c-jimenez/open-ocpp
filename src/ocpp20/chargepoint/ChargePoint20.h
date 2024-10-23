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

#ifndef OPENOCPP_OCPP20_CHARGEPOINT20_H
#define OPENOCPP_OCPP20_CHARGEPOINT20_H

#include "GenericMessageHandler.h"
#include "GenericMessageSender.h"
#include "IChargePoint20.h"
#include "InternalConfigManager.h"
#include "Logger.h"
#include "MessagesConverter20.h"
#include "MessagesValidator20.h"
#include "RpcClient.h"
#include "Timer.h"

#include <atomic>
#include <memory>

namespace ocpp
{
// Forward declarations
namespace messages
{
class MessageDispatcher;
class GenericMessageSender;
} // namespace messages
namespace websockets
{
class IWebsocketClient;
}
namespace chargepoint
{
namespace ocpp20
{

/** @brief Charge point implementation */
class ChargePoint20
    : public IChargePoint20,
      public ocpp::rpc::IRpc::IListener,
      public ocpp::rpc::IRpc::ISpy,
      public ocpp::rpc::RpcClient::IListener,

      // Central System messages
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::CancelReservationReq,
                                                   ocpp::messages::ocpp20::CancelReservationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::CertificateSignedReq,
                                                   ocpp::messages::ocpp20::CertificateSignedConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::ChangeAvailabilityReq,
                                                   ocpp::messages::ocpp20::ChangeAvailabilityConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::ClearCacheReq, ocpp::messages::ocpp20::ClearCacheConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::ClearChargingProfileReq,
                                                   ocpp::messages::ocpp20::ClearChargingProfileConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::ClearDisplayMessageReq,
                                                   ocpp::messages::ocpp20::ClearDisplayMessageConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::ClearVariableMonitoringReq,
                                                   ocpp::messages::ocpp20::ClearVariableMonitoringConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::CustomerInformationReq,
                                                   ocpp::messages::ocpp20::CustomerInformationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::DataTransferReq, ocpp::messages::ocpp20::DataTransferConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::DeleteCertificateReq,
                                                   ocpp::messages::ocpp20::DeleteCertificateConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::Get15118EVCertificateReq,
                                                   ocpp::messages::ocpp20::Get15118EVCertificateConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::GetBaseReportReq, ocpp::messages::ocpp20::GetBaseReportConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::GetCertificateStatusReq,
                                                   ocpp::messages::ocpp20::GetCertificateStatusConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::GetChargingProfilesReq,
                                                   ocpp::messages::ocpp20::GetChargingProfilesConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::GetCompositeScheduleReq,
                                                   ocpp::messages::ocpp20::GetCompositeScheduleConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::GetDisplayMessagesReq,
                                                   ocpp::messages::ocpp20::GetDisplayMessagesConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::GetInstalledCertificateIdsReq,
                                                   ocpp::messages::ocpp20::GetInstalledCertificateIdsConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::GetLocalListVersionReq,
                                                   ocpp::messages::ocpp20::GetLocalListVersionConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::GetLogReq, ocpp::messages::ocpp20::GetLogConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::GetMonitoringReportReq,
                                                   ocpp::messages::ocpp20::GetMonitoringReportConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::GetReportReq, ocpp::messages::ocpp20::GetReportConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::GetTransactionStatusReq,
                                                   ocpp::messages::ocpp20::GetTransactionStatusConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::GetVariablesReq, ocpp::messages::ocpp20::GetVariablesConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::InstallCertificateReq,
                                                   ocpp::messages::ocpp20::InstallCertificateConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::PublishFirmwareReq, ocpp::messages::ocpp20::PublishFirmwareConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::RequestStartTransactionReq,
                                                   ocpp::messages::ocpp20::RequestStartTransactionConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::RequestStopTransactionReq,
                                                   ocpp::messages::ocpp20::RequestStopTransactionConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::ReserveNowReq, ocpp::messages::ocpp20::ReserveNowConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::ResetReq, ocpp::messages::ocpp20::ResetConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::SendLocalListReq, ocpp::messages::ocpp20::SendLocalListConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::SetChargingProfileReq,
                                                   ocpp::messages::ocpp20::SetChargingProfileConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::SetDisplayMessageReq,
                                                   ocpp::messages::ocpp20::SetDisplayMessageConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::SetMonitoringBaseReq,
                                                   ocpp::messages::ocpp20::SetMonitoringBaseConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::SetMonitoringLevelReq,
                                                   ocpp::messages::ocpp20::SetMonitoringLevelConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::SetNetworkProfileReq,
                                                   ocpp::messages::ocpp20::SetNetworkProfileConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::SetVariableMonitoringReq,
                                                   ocpp::messages::ocpp20::SetVariableMonitoringConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::SetVariablesReq, ocpp::messages::ocpp20::SetVariablesConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::TriggerMessageReq, ocpp::messages::ocpp20::TriggerMessageConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::UnlockConnectorReq, ocpp::messages::ocpp20::UnlockConnectorConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::UnpublishFirmwareReq,
                                                   ocpp::messages::ocpp20::UnpublishFirmwareConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::UpdateFirmwareReq, ocpp::messages::ocpp20::UpdateFirmwareConf>
{
  public:
    /** @brief Constructor */
    ChargePoint20(const ocpp::config::IChargePointConfig20&                      stack_config,
                  IChargePointEventsHandler20&                                   events_handler,
                  std::shared_ptr<ocpp::helpers::ITimerPool>                     timer_pool,
                  std::shared_ptr<ocpp::helpers::WorkerThreadPool>               worker_pool,
                  std::unique_ptr<ocpp::messages::ocpp20::MessagesConverter20>&& messages_converter);

    /** @brief Destructor */
    virtual ~ChargePoint20();

    // IChargePoint20 interface

    /** @copydoc ocpp::helpers::ITimerPool& IChargePoint20::getTimerPool() */
    ocpp::helpers::ITimerPool& getTimerPool() override { return *m_timer_pool.get(); }

    /** @copydoc ocpp::helpers::WorkerThreadPool& IChargePoint20::getWorkerPool() */
    ocpp::helpers::WorkerThreadPool& getWorkerPool() override { return *m_worker_pool.get(); }

    /** @copydoc bool IChargePoint20::resetData() */
    bool resetData() override;

    /** @copydoc bool IChargePoint20::start() */
    bool start() override;

    /** @copydoc bool IChargePoint20::stop() */
    bool stop() override;

    /** @copydoc bool IChargePoint20::reconnect() */
    bool reconnect() override;

    // RpcClient::IListener interface

    /** @copydoc void RpcClient::IListener::rpcClientConnected() */
    void rpcClientConnected() override;

    /** @copydoc void RpcClient::IListener::rpcClientFailed() */
    void rpcClientFailed() override;

    // IRpc::IListener interface

    /** @copydoc void IRpc::IListener::rpcDisconnected() */
    void rpcDisconnected() override;

    /** @copydoc void IRpc::IListener::rpcError() */
    void rpcError() override;

    /** @copydoc void IRpc::IListener::rpcCallReceived(const std::string&,
                                                       const rapidjson::Value&,
                                                       rapidjson::Document&,
                                                       std::string&,
                                                       std::string&) */
    bool rpcCallReceived(const std::string&      action,
                         const rapidjson::Value& payload,
                         rapidjson::Document&    response,
                         std::string&            error_code,
                         std::string&            error_message) override;

    // IRpc::ISpy interface

    /** @copydoc void IRpc::ISpy::rcpMessageReceived(const std::string&) */
    void rcpMessageReceived(const std::string& msg) override;

    /** @copydoc void IRpc::ISpy::rcpMessageSent(const std::string&) */
    void rcpMessageSent(const std::string& msg) override;

    // OCPP operations

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::BootNotificationReq&,
     *                                          ocpp::messages::ocpp20::BootNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::BootNotificationReq& request,
              ocpp::messages::ocpp20::BootNotificationConf&      response,
              std::string&                                       error,
              std::string&                                       message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::AuthorizeReq&,
     *                                          ocpp::messages::ocpp20::AuthorizeConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::AuthorizeReq& request,
              ocpp::messages::ocpp20::AuthorizeConf&      response,
              std::string&                                error,
              std::string&                                message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::ClearedChargingLimitReq&,
     *                                          ocpp::messages::ocpp20::ClearedChargingLimitConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::ClearedChargingLimitReq& request,
              ocpp::messages::ocpp20::ClearedChargingLimitConf&      response,
              std::string&                                           error,
              std::string&                                           message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::CostUpdatedReq&,
     *                                          ocpp::messages::ocpp20::CostUpdatedConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::CostUpdatedReq& request,
              ocpp::messages::ocpp20::CostUpdatedConf&      response,
              std::string&                                  error,
              std::string&                                  message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::DataTransferReq&,
     *                                          ocpp::messages::ocpp20::DataTransferConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::DataTransferReq& request,
              ocpp::messages::ocpp20::DataTransferConf&      response,
              std::string&                                   error,
              std::string&                                   message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::FirmwareStatusNotificationReq&,
     *                                          ocpp::messages::ocpp20::FirmwareStatusNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::FirmwareStatusNotificationReq& request,
              ocpp::messages::ocpp20::FirmwareStatusNotificationConf&      response,
              std::string&                                                 error,
              std::string&                                                 message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::HeartbeatReq&,
     *                                          ocpp::messages::ocpp20::HeartbeatConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::HeartbeatReq& request,
              ocpp::messages::ocpp20::HeartbeatConf&      response,
              std::string&                                error,
              std::string&                                message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::LogStatusNotificationReq&,
     *                                          ocpp::messages::ocpp20::LogStatusNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::LogStatusNotificationReq& request,
              ocpp::messages::ocpp20::LogStatusNotificationConf&      response,
              std::string&                                            error,
              std::string&                                            message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::MeterValuesReq&,
     *                                          ocpp::messages::ocpp20::MeterValuesConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::MeterValuesReq& request,
              ocpp::messages::ocpp20::MeterValuesConf&      response,
              std::string&                                  error,
              std::string&                                  message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::NotifyChargingLimitReq&,
     *                                          ocpp::messages::ocpp20::NotifyChargingLimitConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::NotifyChargingLimitReq& request,
              ocpp::messages::ocpp20::NotifyChargingLimitConf&      response,
              std::string&                                          error,
              std::string&                                          message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::NotifyCustomerInformationReq&,
     *                                          ocpp::messages::ocpp20::NotifyCustomerInformationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::NotifyCustomerInformationReq& request,
              ocpp::messages::ocpp20::NotifyCustomerInformationConf&      response,
              std::string&                                                error,
              std::string&                                                message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::NotifyDisplayMessagesReq&,
     *                                          ocpp::messages::ocpp20::NotifyDisplayMessagesConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::NotifyDisplayMessagesReq& request,
              ocpp::messages::ocpp20::NotifyDisplayMessagesConf&      response,
              std::string&                                            error,
              std::string&                                            message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::NotifyEVChargingNeedsReq&,
     *                                          ocpp::messages::ocpp20::NotifyEVChargingNeedsConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::NotifyEVChargingNeedsReq& request,
              ocpp::messages::ocpp20::NotifyEVChargingNeedsConf&      response,
              std::string&                                            error,
              std::string&                                            message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::NotifyEVChargingScheduleReq&,
     *                                          ocpp::messages::ocpp20::NotifyEVChargingScheduleConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::NotifyEVChargingScheduleReq& request,
              ocpp::messages::ocpp20::NotifyEVChargingScheduleConf&      response,
              std::string&                                               error,
              std::string&                                               message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::NotifyEventReq&,
     *                                          ocpp::messages::ocpp20::NotifyEventConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::NotifyEventReq& request,
              ocpp::messages::ocpp20::NotifyEventConf&      response,
              std::string&                                  error,
              std::string&                                  message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::NotifyMonitoringReportReq&,
     *                                          ocpp::messages::ocpp20::NotifyMonitoringReportConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::NotifyMonitoringReportReq& request,
              ocpp::messages::ocpp20::NotifyMonitoringReportConf&      response,
              std::string&                                             error,
              std::string&                                             message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::NotifyReportReq&,
     *                                          ocpp::messages::ocpp20::NotifyReportConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::NotifyReportReq& request,
              ocpp::messages::ocpp20::NotifyReportConf&      response,
              std::string&                                   error,
              std::string&                                   message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::PublishFirmwareStatusNotificationReq&,
     *                                          ocpp::messages::ocpp20::PublishFirmwareStatusNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::PublishFirmwareStatusNotificationReq& request,
              ocpp::messages::ocpp20::PublishFirmwareStatusNotificationConf&      response,
              std::string&                                                        error,
              std::string&                                                        message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::ReportChargingProfilesReq&,
     *                                          ocpp::messages::ocpp20::ReportChargingProfilesConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::ReportChargingProfilesReq& request,
              ocpp::messages::ocpp20::ReportChargingProfilesConf&      response,
              std::string&                                             error,
              std::string&                                             message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::ReservationStatusUpdateReq&,
     *                                          ocpp::messages::ocpp20::ReservationStatusUpdateConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::ReservationStatusUpdateReq& request,
              ocpp::messages::ocpp20::ReservationStatusUpdateConf&      response,
              std::string&                                              error,
              std::string&                                              message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::SecurityEventNotificationReq&,
     *                                          ocpp::messages::ocpp20::SecurityEventNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::SecurityEventNotificationReq& request,
              ocpp::messages::ocpp20::SecurityEventNotificationConf&      response,
              std::string&                                                error,
              std::string&                                                message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::SignCertificateReq&,
     *                                          ocpp::messages::ocpp20::SignCertificateConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::SignCertificateReq& request,
              ocpp::messages::ocpp20::SignCertificateConf&      response,
              std::string&                                      error,
              std::string&                                      message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::StatusNotificationReq&,
     *                                          ocpp::messages::ocpp20::StatusNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::StatusNotificationReq& request,
              ocpp::messages::ocpp20::StatusNotificationConf&      response,
              std::string&                                         error,
              std::string&                                         message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::TransactionEventReq&,
     *                                          ocpp::messages::ocpp20::TransactionEventConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::TransactionEventReq& request,
              ocpp::messages::ocpp20::TransactionEventConf&      response,
              std::string&                                       error,
              std::string&                                       message) override;

    // OCPP handlers

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::CancelReservationReq& request,
                       ocpp::messages::ocpp20::CancelReservationConf&      response,
                       std::string&                                        error_code,
                       std::string&                                        error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::CertificateSignedReq& request,
                       ocpp::messages::ocpp20::CertificateSignedConf&      response,
                       std::string&                                        error_code,
                       std::string&                                        error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::ChangeAvailabilityReq& request,
                       ocpp::messages::ocpp20::ChangeAvailabilityConf&      response,
                       std::string&                                         error_code,
                       std::string&                                         error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::ClearCacheReq& request,
                       ocpp::messages::ocpp20::ClearCacheConf&      response,
                       std::string&                                 error_code,
                       std::string&                                 error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::ClearChargingProfileReq& request,
                       ocpp::messages::ocpp20::ClearChargingProfileConf&      response,
                       std::string&                                           error_code,
                       std::string&                                           error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::ClearDisplayMessageReq& request,
                       ocpp::messages::ocpp20::ClearDisplayMessageConf&      response,
                       std::string&                                          error_code,
                       std::string&                                          error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::ClearVariableMonitoringReq& request,
                       ocpp::messages::ocpp20::ClearVariableMonitoringConf&      response,
                       std::string&                                              error_code,
                       std::string&                                              error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::CustomerInformationReq& request,
                       ocpp::messages::ocpp20::CustomerInformationConf&      response,
                       std::string&                                          error_code,
                       std::string&                                          error_message) override;

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
    bool handleMessage(const ocpp::messages::ocpp20::DeleteCertificateReq& request,
                       ocpp::messages::ocpp20::DeleteCertificateConf&      response,
                       std::string&                                        error_code,
                       std::string&                                        error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::Get15118EVCertificateReq& request,
                       ocpp::messages::ocpp20::Get15118EVCertificateConf&      response,
                       std::string&                                            error_code,
                       std::string&                                            error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::GetBaseReportReq& request,
                       ocpp::messages::ocpp20::GetBaseReportConf&      response,
                       std::string&                                    error_code,
                       std::string&                                    error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::GetCertificateStatusReq& request,
                       ocpp::messages::ocpp20::GetCertificateStatusConf&      response,
                       std::string&                                           error_code,
                       std::string&                                           error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::GetChargingProfilesReq& request,
                       ocpp::messages::ocpp20::GetChargingProfilesConf&      response,
                       std::string&                                          error_code,
                       std::string&                                          error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::GetCompositeScheduleReq& request,
                       ocpp::messages::ocpp20::GetCompositeScheduleConf&      response,
                       std::string&                                           error_code,
                       std::string&                                           error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::GetDisplayMessagesReq& request,
                       ocpp::messages::ocpp20::GetDisplayMessagesConf&      response,
                       std::string&                                         error_code,
                       std::string&                                         error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::GetInstalledCertificateIdsReq& request,
                       ocpp::messages::ocpp20::GetInstalledCertificateIdsConf&      response,
                       std::string&                                                 error_code,
                       std::string&                                                 error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::GetLocalListVersionReq& request,
                       ocpp::messages::ocpp20::GetLocalListVersionConf&      response,
                       std::string&                                          error_code,
                       std::string&                                          error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::GetLogReq& request,
                       ocpp::messages::ocpp20::GetLogConf&      response,
                       std::string&                             error_code,
                       std::string&                             error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::GetMonitoringReportReq& request,
                       ocpp::messages::ocpp20::GetMonitoringReportConf&      response,
                       std::string&                                          error_code,
                       std::string&                                          error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::GetReportReq& request,
                       ocpp::messages::ocpp20::GetReportConf&      response,
                       std::string&                                error_code,
                       std::string&                                error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::GetTransactionStatusReq& request,
                       ocpp::messages::ocpp20::GetTransactionStatusConf&      response,
                       std::string&                                           error_code,
                       std::string&                                           error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::GetVariablesReq& request,
                       ocpp::messages::ocpp20::GetVariablesConf&      response,
                       std::string&                                   error_code,
                       std::string&                                   error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::InstallCertificateReq& request,
                       ocpp::messages::ocpp20::InstallCertificateConf&      response,
                       std::string&                                         error_code,
                       std::string&                                         error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::PublishFirmwareReq& request,
                       ocpp::messages::ocpp20::PublishFirmwareConf&      response,
                       std::string&                                      error_code,
                       std::string&                                      error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::RequestStartTransactionReq& request,
                       ocpp::messages::ocpp20::RequestStartTransactionConf&      response,
                       std::string&                                              error_code,
                       std::string&                                              error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::RequestStopTransactionReq& request,
                       ocpp::messages::ocpp20::RequestStopTransactionConf&      response,
                       std::string&                                             error_code,
                       std::string&                                             error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::ReserveNowReq& request,
                       ocpp::messages::ocpp20::ReserveNowConf&      response,
                       std::string&                                 error_code,
                       std::string&                                 error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::ResetReq& request,
                       ocpp::messages::ocpp20::ResetConf&      response,
                       std::string&                            error_code,
                       std::string&                            error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::SendLocalListReq& request,
                       ocpp::messages::ocpp20::SendLocalListConf&      response,
                       std::string&                                    error_code,
                       std::string&                                    error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::SetChargingProfileReq& request,
                       ocpp::messages::ocpp20::SetChargingProfileConf&      response,
                       std::string&                                         error_code,
                       std::string&                                         error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::SetDisplayMessageReq& request,
                       ocpp::messages::ocpp20::SetDisplayMessageConf&      response,
                       std::string&                                        error_code,
                       std::string&                                        error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::SetMonitoringBaseReq& request,
                       ocpp::messages::ocpp20::SetMonitoringBaseConf&      response,
                       std::string&                                        error_code,
                       std::string&                                        error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::SetMonitoringLevelReq& request,
                       ocpp::messages::ocpp20::SetMonitoringLevelConf&      response,
                       std::string&                                         error_code,
                       std::string&                                         error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::SetNetworkProfileReq& request,
                       ocpp::messages::ocpp20::SetNetworkProfileConf&      response,
                       std::string&                                        error_code,
                       std::string&                                        error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::SetVariableMonitoringReq& request,
                       ocpp::messages::ocpp20::SetVariableMonitoringConf&      response,
                       std::string&                                            error_code,
                       std::string&                                            error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::SetVariablesReq& request,
                       ocpp::messages::ocpp20::SetVariablesConf&      response,
                       std::string&                                   error_code,
                       std::string&                                   error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::TriggerMessageReq& request,
                       ocpp::messages::ocpp20::TriggerMessageConf&      response,
                       std::string&                                     error_code,
                       std::string&                                     error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::UnlockConnectorReq& request,
                       ocpp::messages::ocpp20::UnlockConnectorConf&      response,
                       std::string&                                      error_code,
                       std::string&                                      error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::UnpublishFirmwareReq& request,
                       ocpp::messages::ocpp20::UnpublishFirmwareConf&      response,
                       std::string&                                        error_code,
                       std::string&                                        error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::UpdateFirmwareReq& request,
                       ocpp::messages::ocpp20::UpdateFirmwareConf&      response,
                       std::string&                                     error_code,
                       std::string&                                     error_message) override;

  private:
    /** @brief Stack configuration */
    const ocpp::config::IChargePointConfig20& m_stack_config;
    /** @brief User defined events handler */
    IChargePointEventsHandler20& m_events_handler;

    /** @brief Timer pool */
    std::shared_ptr<ocpp::helpers::ITimerPool> m_timer_pool;
    /** @brief Worker thread pool */
    std::shared_ptr<ocpp::helpers::WorkerThreadPool> m_worker_pool;

    /** @brief Database */
    ocpp::database::Database m_database;
    /** @brief Internal configuration manager */
    ocpp::config::InternalConfigManager m_internal_config;

    /** @brief Messages converter */
    std::unique_ptr<ocpp::messages::ocpp20::MessagesConverter20> m_messages_converter;
    /** @brief Messaes validator */
    ocpp::messages::ocpp20::MessagesValidator20 m_messages_validator;
    /** @brief Indicate that a stop process is in progress */
    std::atomic<bool> m_stop_in_progress;
    /** @brief Indicate that a reconnection process has been scheduled */
    std::atomic<bool> m_reconnect_scheduled;

    /** @brief Websocket s*/
    std::unique_ptr<ocpp::websockets::IWebsocketClient> m_ws_client;
    /** @brief RPC client */
    std::unique_ptr<ocpp::rpc::RpcClient> m_rpc_client;
    /** @brief Message dispatcher */
    std::unique_ptr<ocpp::messages::MessageDispatcher> m_msg_dispatcher;
    /** @brief Message sender */
    std::unique_ptr<ocpp::messages::GenericMessageSender> m_msg_sender;

    /** @brief Uptime timer */
    ocpp::helpers::Timer m_uptime_timer;
    /** @brief Uptime in seconds */
    unsigned int m_uptime;
    /** @brief Disconnected time in seconds */
    unsigned int m_disconnected_time;
    /** @brief Total uptime in seconds */
    unsigned int m_total_uptime;
    /** @brief Total disconnected time in seconds */
    unsigned int m_total_disconnected_time;

    /** @brief Initialize the database */
    void initDatabase();
    /** @brief Process uptime */
    void processUptime();
    /** @brief Save the uptime counter in database */
    void saveUptime();

    /** @brief Schedule a reconnection to the Central System */
    void scheduleReconnect();
    /** @brief Start the connection process to the Central System */
    bool doConnect();

    /**
     * @brief Execute a call request
     * @param action RPC action for the request
     * @param request Request payload
     * @param response Response payload
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    template <typename RequestType, typename ResponseType>
    bool call(const std::string& action, const RequestType& request, ResponseType& response, std::string& error, std::string& message)
    {
        bool ret = false;

        std::string identifier = m_stack_config.chargePointIdentifier();
        LOG_DEBUG << "[" << identifier << "] - " << action;

        if (m_msg_sender && !m_stop_in_progress)
        {
            ocpp::messages::CallResult res = m_msg_sender->call(action, request, response, error, message);
            if (res != ocpp::messages::CallResult::Ok)
            {
                LOG_ERROR << "[" << identifier << "] - " << action << " => "
                          << (res == ocpp::messages::CallResult::Failed ? "Timeout" : "Error");
            }
            else
            {
                ret = true;
            }
        }

        return ret;
    }
};

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_OCPP20_CHARGEPOINT20_H
