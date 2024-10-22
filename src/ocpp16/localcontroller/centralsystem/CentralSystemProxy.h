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

#ifndef OPENOCPP_CENTRALSYSTEMPROXY_H
#define OPENOCPP_CENTRALSYSTEMPROXY_H

#include "CentralSystemHandler.h"
#include "GenericMessageSender.h"
#include "ICentralSystemProxy.h"
#include "MessageDispatcher.h"
#include "MessagesConverter.h"
#include "MessagesValidator.h"
#include "RpcClient.h"
#include "UserMessageHandler.h"

namespace ocpp
{
namespace config
{
class ILocalControllerConfig;
} // namespace config
namespace rpc
{
class RpcPool;
} // namespace rpc

namespace localcontroller
{

/** @brief Central system proxy */
class CentralSystemProxy : public ICentralSystemProxy,
                           public ocpp::rpc::IRpc::IListener,
                           public ocpp::rpc::IRpc::ISpy,
                           public ocpp::rpc::RpcClient::IListener
{
  public:
    /**
     * @brief Constructor
     * @param identifier Charge point's identifier
     * @param messages_validator JSON schemas needed to validate payloads
     * @param messages_converter Converter from/to OCPP to/from JSON messages
     * @param stack_config Stack configuration
     * @param rpc_pool RPC pool
     */
    CentralSystemProxy(const std::string&                               identifier,
                       const ocpp::messages::ocpp16::MessagesValidator& messages_validator,
                       ocpp::messages::ocpp16::MessagesConverter&       messages_converter,
                       const ocpp::config::ILocalControllerConfig&      stack_config,
                       ocpp::rpc::RpcPool&                              rpc_pool);

    /** @brief Destructor */
    virtual ~CentralSystemProxy();

    /**
     * @brief Set the proxy to forward requests to the charge point
     * @param central_system Proxy to forward requests to the charge point
     */
    void setChargePointProxy(std::weak_ptr<IChargePointProxy> charge_point)
    {
        m_charge_point = charge_point;
        m_handler.setChargePointProxy(charge_point);
    }

    /** @copydoc const std::string& ICentralSystemProxy::disconnect() const */
    const std::string& identifier() const override { return m_identifier; }

    /** @copydoc void ICentralSystemProxy::setTimeout(std::chrono::milliseconds) */
    void setTimeout(std::chrono::milliseconds timeout) override;

    /** @copydoc bool ICentralSystemProxy::connect(const std::string&,
                                                   const ocpp::websockets::IWebsocketClient::Credentials&,
                                                   std::chrono::milliseconds,
                                                   std::chrono::milliseconds,
                                                   std::chrono::milliseconds) */
    bool connect(const std::string&                                     url,
                 const ocpp::websockets::IWebsocketClient::Credentials& credentials,
                 std::chrono::milliseconds                              connect_timeout,
                 std::chrono::milliseconds                              retry_interval,
                 std::chrono::milliseconds                              ping_interval) override;

    /** @copydoc void ICentralSystemProxy::disconnect() */
    void disconnect() override;

    /** @brief Indicate if the connection with the central system is active */
    bool isConnected() const override;

    /** @copydoc void ICentralSystemProxy::registerListener(ILocalControllerProxyEventsHandler&) */
    void registerListener(ILocalControllerProxyEventsHandler& listener) override { m_listener = &listener; }

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::ocpp16::BootNotificationReq&,
     *                                          ocpp::messages::ocpp16::BootNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp16::BootNotificationReq& request,
              ocpp::messages::ocpp16::BootNotificationConf&      response,
              std::string&                                       error,
              std::string&                                       message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::ocpp16::StatusNotificationReq&,
     *                                          ocpp::messages::ocpp16::StatusNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp16::StatusNotificationReq& request,
              ocpp::messages::ocpp16::StatusNotificationConf&      response,
              std::string&                                         error,
              std::string&                                         message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::ocpp16::AuthorizeReq&,
     *                                          ocpp::messages::ocpp16::AuthorizeConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp16::AuthorizeReq& request,
              ocpp::messages::ocpp16::AuthorizeConf&      response,
              std::string&                                error,
              std::string&                                message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::ocpp16::StartTransactionReq&,
     *                                          ocpp::messages::ocpp16::StartTransactionConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp16::StartTransactionReq& request,
              ocpp::messages::ocpp16::StartTransactionConf&      response,
              std::string&                                       error,
              std::string&                                       message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::ocpp16::StopTransactionReq&,
     *                                          ocpp::messages::ocpp16::StopTransactionConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp16::StopTransactionReq& request,
              ocpp::messages::ocpp16::StopTransactionConf&      response,
              std::string&                                      error,
              std::string&                                      message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::ocpp16::DataTransferReq&,
     *                                          ocpp::messages::ocpp16::DataTransferConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp16::DataTransferReq& request,
              ocpp::messages::ocpp16::DataTransferConf&      response,
              std::string&                                   error,
              std::string&                                   message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::ocpp16::MeterValuesReq&,
     *                                          ocpp::messages::ocpp16::MeterValuesConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp16::MeterValuesReq& request,
              ocpp::messages::ocpp16::MeterValuesConf&      response,
              std::string&                                  error,
              std::string&                                  message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::ocpp16::DiagnosticsStatusNotificationReq&,
     *                                          ocpp::messages::ocpp16::DiagnosticsStatusNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp16::DiagnosticsStatusNotificationReq& request,
              ocpp::messages::ocpp16::DiagnosticsStatusNotificationConf&      response,
              std::string&                                                    error,
              std::string&                                                    message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::ocpp16::FirmwareStatusNotificationReq&,
     *                                          ocpp::messages::ocpp16::FirmwareStatusNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp16::FirmwareStatusNotificationReq& request,
              ocpp::messages::ocpp16::FirmwareStatusNotificationConf&      response,
              std::string&                                                 error,
              std::string&                                                 message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::ocpp16::HeartbeatReq&,
     *                                          ocpp::messages::ocpp16::HeartbeatConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp16::HeartbeatReq& request,
              ocpp::messages::ocpp16::HeartbeatConf&      response,
              std::string&                                error,
              std::string&                                message) override;

    // Security extensions

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::ocpp16::SecurityEventNotificationReq&,
     *                                          ocpp::messages::ocpp16::SecurityEventNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp16::SecurityEventNotificationReq& request,
              ocpp::messages::ocpp16::SecurityEventNotificationConf&      response,
              std::string&                                                error,
              std::string&                                                message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::ocpp16::SignCertificateReq&,
     *                                          ocpp::messages::ocpp16::SignCertificateConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp16::SignCertificateReq& request,
              ocpp::messages::ocpp16::SignCertificateConf&      response,
              std::string&                                      error,
              std::string&                                      message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::ocpp16::LogStatusNotificationReq&,
     *                                          ocpp::messages::ocpp16::LogStatusNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp16::LogStatusNotificationReq& request,
              ocpp::messages::ocpp16::LogStatusNotificationConf&      response,
              std::string&                                            error,
              std::string&                                            message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::ocpp16::SignedFirmwareStatusNotificationReq&,
     *                                          ocpp::messages::ocpp16::SignedFirmwareStatusNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp16::SignedFirmwareStatusNotificationReq& request,
              ocpp::messages::ocpp16::SignedFirmwareStatusNotificationConf&      response,
              std::string&                                                       error,
              std::string&                                                       message) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::CancelReservationReq&, ocpp::messages::ocpp16::CancelReservationConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::CancelReservationReq&,
                                            ocpp::messages::ocpp16::CancelReservationConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::ChangeAvailabilityReq&, ocpp::messages::ocpp16::ChangeAvailabilityConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::ChangeAvailabilityReq&,
                                            ocpp::messages::ocpp16::ChangeAvailabilityConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::ChangeConfigurationReq&, ocpp::messages::ocpp16::ChangeConfigurationConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::ChangeConfigurationReq&,
                                            ocpp::messages::ocpp16::ChangeConfigurationConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::ClearCacheReq&, ocpp::messages::ocpp16::ClearCacheConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(
            const ocpp::messages::ocpp16::ClearCacheReq&, ocpp::messages::ocpp16::ClearCacheConf&, std::string&, std::string&)> handler)
        override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::ClearChargingProfileReq&, ocpp::messages::ocpp16::ClearChargingProfileConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::ClearChargingProfileReq&,
                                            ocpp::messages::ocpp16::ClearChargingProfileConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::DataTransferReq&, ocpp::messages::ocpp16::DataTransferConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(
            const ocpp::messages::ocpp16::DataTransferReq&, ocpp::messages::ocpp16::DataTransferConf&, std::string&, std::string&)> handler)
        override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::GetCompositeScheduleReq&, ocpp::messages::ocpp16::GetCompositeScheduleConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::GetCompositeScheduleReq&,
                                            ocpp::messages::ocpp16::GetCompositeScheduleConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::GetConfigurationReq&, ocpp::messages::ocpp16::GetConfigurationConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(
            const ocpp::messages::ocpp16::GetConfigurationReq&, ocpp::messages::ocpp16::GetConfigurationConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::GetDiagnosticsReq&, ocpp::messages::ocpp16::GetDiagnosticsConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<
            bool(const ocpp::messages::ocpp16::GetDiagnosticsReq&, ocpp::messages::ocpp16::GetDiagnosticsConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::GetLocalListVersionReq&, ocpp::messages::ocpp16::GetLocalListVersionConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::GetLocalListVersionReq&,
                                            ocpp::messages::ocpp16::GetLocalListVersionConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::RemoteStartTransactionReq&, ocpp::messages::ocpp16::RemoteStartTransactionConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::RemoteStartTransactionReq&,
                                            ocpp::messages::ocpp16::RemoteStartTransactionConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::RemoteStopTransactionReq&, ocpp::messages::ocpp16::RemoteStopTransactionConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::RemoteStopTransactionReq&,
                                            ocpp::messages::ocpp16::RemoteStopTransactionConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::ReserveNowReq&, ocpp::messages::ocpp16::ReserveNowConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(
            const ocpp::messages::ocpp16::ReserveNowReq&, ocpp::messages::ocpp16::ReserveNowConf&, std::string&, std::string&)> handler)
        override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::ResetReq&, ocpp::messages::ocpp16::ResetConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(const ocpp::messages::ocpp16::ResetReq&, ocpp::messages::ocpp16::ResetConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::SendLocalListReq&, ocpp::messages::ocpp16::SendLocalListConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<
            bool(const ocpp::messages::ocpp16::SendLocalListReq&, ocpp::messages::ocpp16::SendLocalListConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::SetChargingProfileReq&, ocpp::messages::ocpp16::SetChargingProfileConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::SetChargingProfileReq&,
                                            ocpp::messages::ocpp16::SetChargingProfileConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::TriggerMessageReq&, ocpp::messages::ocpp16::TriggerMessageConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<
            bool(const ocpp::messages::ocpp16::TriggerMessageReq&, ocpp::messages::ocpp16::TriggerMessageConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::UnlockConnectorReq&, ocpp::messages::ocpp16::UnlockConnectorConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(
            const ocpp::messages::ocpp16::UnlockConnectorReq&, ocpp::messages::ocpp16::UnlockConnectorConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::UpdateFirmwareReq&, ocpp::messages::ocpp16::UpdateFirmwareConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<
            bool(const ocpp::messages::ocpp16::UpdateFirmwareReq&, ocpp::messages::ocpp16::UpdateFirmwareConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::CertificateSignedReq&, ocpp::messages::ocpp16::CertificateSignedConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::CertificateSignedReq&,
                                            ocpp::messages::ocpp16::CertificateSignedConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::DeleteCertificateReq&, ocpp::messages::ocpp16::DeleteCertificateConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::DeleteCertificateReq&,
                                            ocpp::messages::ocpp16::DeleteCertificateConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::ExtendedTriggerMessageReq&, ocpp::messages::ocpp16::ExtendedTriggerMessageConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::ExtendedTriggerMessageReq&,
                                            ocpp::messages::ocpp16::ExtendedTriggerMessageConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::GetInstalledCertificateIdsReq&, ocpp::messages::ocpp16::GetInstalledCertificateIdsConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::GetInstalledCertificateIdsReq&,
                                            ocpp::messages::ocpp16::GetInstalledCertificateIdsConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::GetLogReq&, ocpp::messages::ocpp16::GetLogConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(const ocpp::messages::ocpp16::GetLogReq&, ocpp::messages::ocpp16::GetLogConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::InstallCertificateReq&, ocpp::messages::ocpp16::InstallCertificateConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::InstallCertificateReq&,
                                            ocpp::messages::ocpp16::InstallCertificateConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::SignedUpdateFirmwareReq&, ocpp::messages::ocpp16::SignedUpdateFirmwareConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::SignedUpdateFirmwareReq&,
                                            ocpp::messages::ocpp16::SignedUpdateFirmwareConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    // IRpc::IListener interface

    /** @copydoc void IRpc::IListener::rpcDisconnected() */
    void rpcDisconnected() override;

    /** @copydoc void IRpc::IListener::rpcError() */
    void rpcError() override;

    /** @copydoc bool IRpc::IListener::rpcCallReceived(const std::string&,
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

    /** @copydoc void IRpc::ISpy::rcpMessageReceived(const std::string& msg) */
    void rcpMessageReceived(const std::string& msg) override;

    /** @copydoc void IRpc::ISpy::rcpMessageSent(const std::string& msg) */
    void rcpMessageSent(const std::string& msg) override;

    // RpcClient::IListener interface

    /** @brief Called when connection is successfull */
    void rpcClientConnected() override;

    /** @brief Called when connection failed */
    void rpcClientFailed() override;

  private:
    /** @brief Charge point's identifier */
    std::string m_identifier;
    /** @brief Stack configuration */
    const ocpp::config::ILocalControllerConfig& m_stack_config;
    /** @brief Client websocket */
    std::unique_ptr<ocpp::websockets::IWebsocketClient> m_websocket;
    /** @brief RPC connection */
    ocpp::rpc::RpcClient m_rpc;
    /** @brief Messages converter */
    ocpp::messages::ocpp16::MessagesConverter& m_messages_converter;
    /** @brief Message dispatcher */
    ocpp::messages::MessageDispatcher m_msg_dispatcher;
    /** @brief Message sender */
    ocpp::messages::GenericMessageSender m_msg_sender;
    /** @brief Request handler */
    CentralSystemHandler m_handler;
    /** @brief Proxy to forward requests to the charge point */
    std::weak_ptr<IChargePointProxy> m_charge_point;
    /** @brief Listener to the proxy events */
    ILocalControllerProxyEventsHandler* m_listener;
    /** @brief User message handlers */
    std::vector<std::shared_ptr<ocpp::messages::IMessageDispatcher::IMessageHandler>> m_user_handlers;

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
        bool ret = true;

        LOG_DEBUG << "[" << m_identifier << "] - " << action;

        ocpp::messages::CallResult res = m_msg_sender.call(action, request, response, error, message);
        if (res != ocpp::messages::CallResult::Ok)
        {
            LOG_ERROR << "[" << m_identifier << "] - " << action << " => "
                      << (res == ocpp::messages::CallResult::Failed ? "Timeout" : "Error");
            ret = false;
        }

        return ret;
    }

    /**
     * @brief Register a user handler for an incoming request
     * @param action RPC action for the request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    template <typename RequestType, typename ResponseType>
    bool registerHandler(const std::string&                                                                 action,
                         std::function<bool(const RequestType&, ResponseType&, std::string&, std::string&)> handler)
    {
        ocpp::messages::UserMessageHandler<RequestType, ResponseType>* msg_handler =
            new ocpp::messages::UserMessageHandler<RequestType, ResponseType>(action, m_messages_converter, handler);
        m_user_handlers.push_back(std::shared_ptr<ocpp::messages::IMessageDispatcher::IMessageHandler>(msg_handler));
        return m_msg_dispatcher.registerHandler(action, *msg_handler, true);
    }
};

} // namespace localcontroller
} // namespace ocpp

#endif // OPENOCPP_CENTRALSYSTEMPROXY_H
