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

#ifndef OPENOCPP_LC_CHARGEPOINTPROXY_H
#define OPENOCPP_LC_CHARGEPOINTPROXY_H

#include "ChargePointHandler.h"
#include "GenericMessageSender.h"
#include "IChargePointProxy.h"
#include "Logger.h"
#include "MessageDispatcher.h"
#include "MessagesValidator.h"
#include "RpcServer.h"
#include "UserMessageHandler.h"

namespace ocpp
{
namespace config
{
class ILocalControllerConfig;
} // namespace config
namespace localcontroller
{

/** @brief Charge point proxy */
class ChargePointProxy : public IChargePointProxy, public ocpp::rpc::IRpc::IListener, public ocpp::rpc::IRpc::ISpy
{
  public:
    /**
     * @brief Constructor
     * @param identifier Charge point's identifier
     * @param rpc RPC connection with the charge point
     * @param messages_validator JSON schemas needed to validate payloads
     * @param messages_converter Converter from/to OCPP to/from JSON messages
     * @param stack_config Stack configuration
     * @param central_system Proxy to forward requests to the central system
     */
    ChargePointProxy(const std::string&                               identifier,
                     std::shared_ptr<ocpp::rpc::RpcServer::Client>    rpc,
                     const ocpp::messages::ocpp16::MessagesValidator& messages_validator,
                     ocpp::messages::ocpp16::MessagesConverter&       messages_converter,
                     const ocpp::config::ILocalControllerConfig&      stack_config,
                     std::shared_ptr<ICentralSystemProxy>             central_system);
    /** @brief Destructor */
    virtual ~ChargePointProxy();

    // IChargePointProxy interface

    /** @copydoc const std::string& IChargePointProxy::ipAddress() const */
    const std::string& ipAddress() const override;

    /** @copydoc const std::string& IChargePointProxy::identifier() const */
    const std::string& identifier() const override { return m_identifier; }

    /** @copydoc void IChargePointProxy::setTimeout(std::chrono::milliseconds) */
    void setTimeout(std::chrono::milliseconds timeout) override;

    /** @copydoc void IChargePointProxy::disconnect() */
    void disconnect() override;

    /** @copydoc std::shared_ptr<ICentralSystemProxy> IChargePointProxy::centralSystemProxy() */
    std::shared_ptr<ICentralSystemProxy> centralSystemProxy() override { return m_central_system; }

    /** @copydoc void IChargePointProxy::registerListener(ILocalControllerProxyEventsHandler&) */
    void registerListener(ILocalControllerProxyEventsHandler& listener) override
    {
        m_listener = &listener;
        m_central_system->registerListener(listener);
    }

    // OCPP operations

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::CancelReservationReq&,
     *                                        ocpp::messages::ocpp16::CancelReservationConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::CancelReservationReq& request,
              ocpp::messages::ocpp16::CancelReservationConf&      response,
              std::string&                                        error,
              std::string&                                        message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::ChangeAvailabilityReq&,
     *                                        ocpp::messages::ocpp16::ChangeAvailabilityConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::ChangeAvailabilityReq& request,
              ocpp::messages::ocpp16::ChangeAvailabilityConf&      response,
              std::string&                                         error,
              std::string&                                         message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::ChangeConfigurationReq&,
     *                                        ocpp::messages::ocpp16::ChangeConfigurationConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::ChangeConfigurationReq& request,
              ocpp::messages::ocpp16::ChangeConfigurationConf&      response,
              std::string&                                          error,
              std::string&                                          message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::ClearCacheReq&,
     *                                        ocpp::messages::ocpp16::ClearCacheConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::ClearCacheReq& request,
              ocpp::messages::ocpp16::ClearCacheConf&      response,
              std::string&                                 error,
              std::string&                                 message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::ClearChargingProfileReq&,
     *                                        cpp::messages::ClearChargingProfileConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::ClearChargingProfileReq& request,
              ocpp::messages::ocpp16::ClearChargingProfileConf&      response,
              std::string&                                           error,
              std::string&                                           message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::DataTransferReq&,
     *                                        ocpp::messages::ocpp16::DataTransferConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::DataTransferReq& request,
              ocpp::messages::ocpp16::DataTransferConf&      response,
              std::string&                                   error,
              std::string&                                   message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::GetCompositeScheduleReq&,
     *                                        ocpp::messages::ocpp16::GetCompositeScheduleConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::GetCompositeScheduleReq& request,
              ocpp::messages::ocpp16::GetCompositeScheduleConf&      response,
              std::string&                                           error,
              std::string&                                           message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::GetConfigurationReq&,
     *                                        ocpp::messages::ocpp16::GetConfigurationConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::GetConfigurationReq& request,
              ocpp::messages::ocpp16::GetConfigurationConf&      response,
              std::string&                                       error,
              std::string&                                       message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::GetDiagnosticsReq&,
     *                                        ocpp::messages::ocpp16::GetDiagnosticsConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::GetDiagnosticsReq& request,
              ocpp::messages::ocpp16::GetDiagnosticsConf&      response,
              std::string&                                     error,
              std::string&                                     message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::GetLocalListVersionReq&,
     *                                        ocpp::messages::ocpp16::GetLocalListVersionConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::GetLocalListVersionReq& request,
              ocpp::messages::ocpp16::GetLocalListVersionConf&      response,
              std::string&                                          error,
              std::string&                                          message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::RemoteStartTransactionReq&,
     *                                        ocpp::messages::ocpp16::RemoteStartTransactionConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::RemoteStartTransactionReq& request,
              ocpp::messages::ocpp16::RemoteStartTransactionConf&      response,
              std::string&                                             error,
              std::string&                                             message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::RemoteStopTransactionReq&,
     *                                        ocpp::messages::ocpp16::RemoteStopTransactionConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::RemoteStopTransactionReq& request,
              ocpp::messages::ocpp16::RemoteStopTransactionConf&      response,
              std::string&                                            error,
              std::string&                                            message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::ReserveNowReq&,
     *                                        ocpp::messages::ocpp16::ReserveNowConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::ReserveNowReq& request,
              ocpp::messages::ocpp16::ReserveNowConf&      response,
              std::string&                                 error,
              std::string&                                 message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::ResetReq&,
     *                                        ocpp::messages::ocpp16::ResetConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::ResetReq& request,
              ocpp::messages::ocpp16::ResetConf&      response,
              std::string&                            error,
              std::string&                            message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::SendLocalListReq&,
     *                                        ocpp::messages::ocpp16::SendLocalListConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::SendLocalListReq& request,
              ocpp::messages::ocpp16::SendLocalListConf&      response,
              std::string&                                    error,
              std::string&                                    message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::SetChargingProfileReq&,
     *                                        ocpp::messages::ocpp16::SetChargingProfileConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::SetChargingProfileReq& request,
              ocpp::messages::ocpp16::SetChargingProfileConf&      response,
              std::string&                                         error,
              std::string&                                         message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::TriggerMessageReq&,
     *                                        opp::messages::TriggerMessageConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::TriggerMessageReq& request,
              ocpp::messages::ocpp16::TriggerMessageConf&      response,
              std::string&                                     error,
              std::string&                                     message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::UnlockConnectorReq&,
     *                                        ocpp::messages::ocpp16::UnlockConnectorConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::UnlockConnectorReq& request,
              ocpp::messages::ocpp16::UnlockConnectorConf&      response,
              std::string&                                      error,
              std::string&                                      message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::UpdateFirmwareReq&,
     *                                        ocpp::messages::ocpp16::UpdateFirmwareConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::UpdateFirmwareReq& request,
              ocpp::messages::ocpp16::UpdateFirmwareConf&      response,
              std::string&                                     error,
              std::string&                                     message) override;

    // Security extensions

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::CertificateSignedReq&,
     *                                        ocpp::messages::ocpp16::CertificateSignedConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::CertificateSignedReq& request,
              ocpp::messages::ocpp16::CertificateSignedConf&      response,
              std::string&                                        error,
              std::string&                                        message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::DeleteCertificateReq&,
     *                                        ocpp::messages::ocpp16::DeleteCertificateConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::DeleteCertificateReq& request,
              ocpp::messages::ocpp16::DeleteCertificateConf&      response,
              std::string&                                        error,
              std::string&                                        message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::ExtendedTriggerMessageReq&,
     *                                        ocpp::messages::ocpp16::ExtendedTriggerMessageConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::ExtendedTriggerMessageReq& request,
              ocpp::messages::ocpp16::ExtendedTriggerMessageConf&      response,
              std::string&                                             error,
              std::string&                                             message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::GetInstalledCertificateIdsReq&,
     *                                        ocpp::messages::ocpp16::GetInstalledCertificateIdsConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::GetInstalledCertificateIdsReq& request,
              ocpp::messages::ocpp16::GetInstalledCertificateIdsConf&      response,
              std::string&                                                 error,
              std::string&                                                 message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::GetLogReq&,
     *                                        ocpp::messages::ocpp16::GetLogConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::GetLogReq& request,
              ocpp::messages::ocpp16::GetLogConf&      response,
              std::string&                             error,
              std::string&                             message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::InstallCertificateReq&,
     *                                        ocpp::messages::ocpp16::InstallCertificateConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::InstallCertificateReq& request,
              ocpp::messages::ocpp16::InstallCertificateConf&      response,
              std::string&                                         error,
              std::string&                                         message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::SignedUpdateFirmwareReq&,
     *                                        ocpp::messages::ocpp16::SignedUpdateFirmwareConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ocpp16::SignedUpdateFirmwareReq& request,
              ocpp::messages::ocpp16::SignedUpdateFirmwareConf&      response,
              std::string&                                           error,
              std::string&                                           message) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::AuthorizeReq&, ocpp::messages::ocpp16::AuthorizeConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(const ocpp::messages::ocpp16::AuthorizeReq&, ocpp::messages::ocpp16::AuthorizeConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::BootNotificationReq&, ocpp::messages::ocpp16::BootNotificationConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(
            const ocpp::messages::ocpp16::BootNotificationReq&, ocpp::messages::ocpp16::BootNotificationConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::DataTransferReq&, ocpp::messages::ocpp16::DataTransferConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(
            const ocpp::messages::ocpp16::DataTransferReq&, ocpp::messages::ocpp16::DataTransferConf&, std::string&, std::string&)> handler)
        override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::DiagnosticsStatusNotificationReq&,
                                         ocpp::messages::ocpp16::DiagnosticsStatusNotificationConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::DiagnosticsStatusNotificationReq&,
                                            ocpp::messages::ocpp16::DiagnosticsStatusNotificationConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::FirmwareStatusNotificationReq&,
                                         ocpp::messages::ocpp16::FirmwareStatusNotificationConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::FirmwareStatusNotificationReq&,
                                            ocpp::messages::ocpp16::FirmwareStatusNotificationConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::HeartbeatReq&, ocpp::messages::ocpp16::HeartbeatConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(const ocpp::messages::ocpp16::HeartbeatReq&, ocpp::messages::ocpp16::HeartbeatConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::MeterValuesReq&, ocpp::messages::ocpp16::MeterValuesConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(
            const ocpp::messages::ocpp16::MeterValuesReq&, ocpp::messages::ocpp16::MeterValuesConf&, std::string&, std::string&)> handler)
        override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::StartTransactionReq&, ocpp::messages::ocpp16::StartTransactionConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(
            const ocpp::messages::ocpp16::StartTransactionReq&, ocpp::messages::ocpp16::StartTransactionConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::StatusNotificationReq&, ocpp::messages::ocpp16::StatusNotificationConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::StatusNotificationReq&,
                                            ocpp::messages::ocpp16::StatusNotificationConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::StopTransactionReq&, ocpp::messages::ocpp16::StopTransactionConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(
            const ocpp::messages::ocpp16::StopTransactionReq&, ocpp::messages::ocpp16::StopTransactionConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::LogStatusNotificationReq&, ocpp::messages::ocpp16::LogStatusNotificationConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::LogStatusNotificationReq&,
                                            ocpp::messages::ocpp16::LogStatusNotificationConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::SecurityEventNotificationReq&,
                                         ocpp::messages::ocpp16::SecurityEventNotificationConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::SecurityEventNotificationReq&,
                                            ocpp::messages::ocpp16::SecurityEventNotificationConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::SignCertificateReq&, ocpp::messages::ocpp16::SignCertificateConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(
            const ocpp::messages::ocpp16::SignCertificateReq&, ocpp::messages::ocpp16::SignCertificateConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::SignedFirmwareStatusNotificationReq&,
                                         ocpp::messages::ocpp16::SignedFirmwareStatusNotificationConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::SignedFirmwareStatusNotificationReq&,
                                            ocpp::messages::ocpp16::SignedFirmwareStatusNotificationConf&,
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

  private:
    /** @brief Charge point's identifier */
    std::string m_identifier;
    /** @brief RPC connection */
    std::shared_ptr<ocpp::rpc::RpcServer::Client> m_rpc;
    /** @brief Messages converter */
    ocpp::messages::ocpp16::MessagesConverter& m_messages_converter;
    /** @brief Message dispatcher */
    ocpp::messages::MessageDispatcher m_msg_dispatcher;
    /** @brief Message sender */
    ocpp::messages::GenericMessageSender m_msg_sender;
    /** @brief Proxy to forward requests to the central system */
    std::shared_ptr<ICentralSystemProxy> m_central_system;
    /** @brief Request handler */
    ChargePointHandler m_handler;
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

#endif // OPENOCPP_LC_CHARGEPOINTPROXY_H
