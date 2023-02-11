/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License version 2.1
as published by the Free Software Foundation.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPENOCPP_CHARGEPOINTPROXY_H
#define OPENOCPP_CHARGEPOINTPROXY_H

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
    ChargePointProxy(const std::string&                            identifier,
                     std::shared_ptr<ocpp::rpc::RpcServer::Client> rpc,
                     const ocpp::messages::MessagesValidator&      messages_validator,
                     ocpp::messages::MessagesConverter&            messages_converter,
                     const ocpp::config::ILocalControllerConfig&   stack_config,
                     std::shared_ptr<ICentralSystemProxy>          central_system);
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

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::CancelReservationReq&,
     *                                        ocpp::messages::CancelReservationConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::CancelReservationReq& request,
              ocpp::messages::CancelReservationConf&      response,
              std::string&                                error,
              std::string&                                message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ChangeAvailabilityReq&,
     *                                        ocpp::messages::ChangeAvailabilityConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ChangeAvailabilityReq& request,
              ocpp::messages::ChangeAvailabilityConf&      response,
              std::string&                                 error,
              std::string&                                 message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ChangeConfigurationReq&,
     *                                        ocpp::messages::ChangeConfigurationConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ChangeConfigurationReq& request,
              ocpp::messages::ChangeConfigurationConf&      response,
              std::string&                                  error,
              std::string&                                  message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ClearCacheReq&,
     *                                        ocpp::messages::ClearCacheConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ClearCacheReq& request,
              ocpp::messages::ClearCacheConf&      response,
              std::string&                         error,
              std::string&                         message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ClearChargingProfileReq&,
     *                                        cpp::messages::ClearChargingProfileConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ClearChargingProfileReq& request,
              ocpp::messages::ClearChargingProfileConf&      response,
              std::string&                                   error,
              std::string&                                   message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::DataTransferReq&,
     *                                        ocpp::messages::DataTransferConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::DataTransferReq& request,
              ocpp::messages::DataTransferConf&      response,
              std::string&                           error,
              std::string&                           message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::GetCompositeScheduleReq&,
     *                                        ocpp::messages::GetCompositeScheduleConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::GetCompositeScheduleReq& request,
              ocpp::messages::GetCompositeScheduleConf&      response,
              std::string&                                   error,
              std::string&                                   message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::GetConfigurationReq&,
     *                                        ocpp::messages::GetConfigurationConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::GetConfigurationReq& request,
              ocpp::messages::GetConfigurationConf&      response,
              std::string&                               error,
              std::string&                               message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::GetDiagnosticsReq&,
     *                                        ocpp::messages::GetDiagnosticsConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::GetDiagnosticsReq& request,
              ocpp::messages::GetDiagnosticsConf&      response,
              std::string&                             error,
              std::string&                             message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::GetLocalListVersionReq&,
     *                                        ocpp::messages::GetLocalListVersionConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::GetLocalListVersionReq& request,
              ocpp::messages::GetLocalListVersionConf&      response,
              std::string&                                  error,
              std::string&                                  message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::RemoteStartTransactionReq&,
     *                                        ocpp::messages::RemoteStartTransactionConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::RemoteStartTransactionReq& request,
              ocpp::messages::RemoteStartTransactionConf&      response,
              std::string&                                     error,
              std::string&                                     message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::RemoteStopTransactionReq&,
     *                                        ocpp::messages::RemoteStopTransactionConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::RemoteStopTransactionReq& request,
              ocpp::messages::RemoteStopTransactionConf&      response,
              std::string&                                    error,
              std::string&                                    message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ReserveNowReq&,
     *                                        ocpp::messages::ReserveNowConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ReserveNowReq& request,
              ocpp::messages::ReserveNowConf&      response,
              std::string&                         error,
              std::string&                         message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ResetReq&,
     *                                        ocpp::messages::ResetConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ResetReq& request,
              ocpp::messages::ResetConf&      response,
              std::string&                    error,
              std::string&                    message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::SendLocalListReq&,
     *                                        ocpp::messages::SendLocalListConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::SendLocalListReq& request,
              ocpp::messages::SendLocalListConf&      response,
              std::string&                            error,
              std::string&                            message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::SetChargingProfileReq&,
     *                                        ocpp::messages::SetChargingProfileConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::SetChargingProfileReq& request,
              ocpp::messages::SetChargingProfileConf&      response,
              std::string&                                 error,
              std::string&                                 message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::TriggerMessageReq&,
     *                                        opp::messages::TriggerMessageConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::TriggerMessageReq& request,
              ocpp::messages::TriggerMessageConf&      response,
              std::string&                             error,
              std::string&                             message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::UnlockConnectorReq&,
     *                                        ocpp::messages::UnlockConnectorConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::UnlockConnectorReq& request,
              ocpp::messages::UnlockConnectorConf&      response,
              std::string&                              error,
              std::string&                              message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::UpdateFirmwareReq&,
     *                                        ocpp::messages::UpdateFirmwareConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::UpdateFirmwareReq& request,
              ocpp::messages::UpdateFirmwareConf&      response,
              std::string&                             error,
              std::string&                             message) override;

    // Security extensions

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::CertificateSignedReq&,
     *                                        ocpp::messages::CertificateSignedConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::CertificateSignedReq& request,
              ocpp::messages::CertificateSignedConf&      response,
              std::string&                                error,
              std::string&                                message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::DeleteCertificateReq&,
     *                                        ocpp::messages::DeleteCertificateConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::DeleteCertificateReq& request,
              ocpp::messages::DeleteCertificateConf&      response,
              std::string&                                error,
              std::string&                                message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::ExtendedTriggerMessageReq&,
     *                                        ocpp::messages::ExtendedTriggerMessageConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::ExtendedTriggerMessageReq& request,
              ocpp::messages::ExtendedTriggerMessageConf&      response,
              std::string&                                     error,
              std::string&                                     message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::GetInstalledCertificateIdsReq&,
     *                                        ocpp::messages::GetInstalledCertificateIdsConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::GetInstalledCertificateIdsReq& request,
              ocpp::messages::GetInstalledCertificateIdsConf&      response,
              std::string&                                         error,
              std::string&                                         message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::GetLogReq&,
     *                                        ocpp::messages::GetLogConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::GetLogReq& request,
              ocpp::messages::GetLogConf&      response,
              std::string&                     error,
              std::string&                     message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::InstallCertificateReq&,
     *                                        ocpp::messages::InstallCertificateConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::InstallCertificateReq& request,
              ocpp::messages::InstallCertificateConf&      response,
              std::string&                                 error,
              std::string&                                 message) override;

    /** @copydoc bool IChargePointProxy::call(const ocpp::messages::SignedUpdateFirmwareReq&,
     *                                        ocpp::messages::SignedUpdateFirmwareConf&,
     *                                        std::string&,
     *                                        std::string&) */
    bool call(const ocpp::messages::SignedUpdateFirmwareReq& request,
              ocpp::messages::SignedUpdateFirmwareConf&      response,
              std::string&                                   error,
              std::string&                                   message) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::AuthorizeReq&, ocpp::messages::AuthorizeConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(const ocpp::messages::AuthorizeReq&, ocpp::messages::AuthorizeConf&, std::string&, std::string&)> handler)
        override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::BootNotificationReq&, ocpp::messages::BootNotificationConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(const ocpp::messages::BootNotificationReq&, ocpp::messages::BootNotificationConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::DataTransferReq&, ocpp::messages::DataTransferConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(const ocpp::messages::DataTransferReq&, ocpp::messages::DataTransferConf&, std::string&, std::string&)> handler)
        override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::DiagnosticsStatusNotificationReq&,
                                         ocpp::messages::DiagnosticsStatusNotificationConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::DiagnosticsStatusNotificationReq&,
                                            ocpp::messages::DiagnosticsStatusNotificationConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::FirmwareStatusNotificationReq&,
                                         ocpp::messages::FirmwareStatusNotificationConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::FirmwareStatusNotificationReq&,
                                            ocpp::messages::FirmwareStatusNotificationConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::HeartbeatReq&, ocpp::messages::HeartbeatConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(const ocpp::messages::HeartbeatReq&, ocpp::messages::HeartbeatConf&, std::string&, std::string&)> handler)
        override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::MeterValuesReq&, ocpp::messages::MeterValuesConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(const ocpp::messages::MeterValuesReq&, ocpp::messages::MeterValuesConf&, std::string&, std::string&)> handler)
        override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::StartTransactionReq&, ocpp::messages::StartTransactionConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(const ocpp::messages::StartTransactionReq&, ocpp::messages::StartTransactionConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::StatusNotificationReq&, ocpp::messages::StatusNotificationConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(
            const ocpp::messages::StatusNotificationReq&, ocpp::messages::StatusNotificationConf&, std::string&, std::string&)> handler)
        override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::StopTransactionReq&, ocpp::messages::StopTransactionConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(const ocpp::messages::StopTransactionReq&, ocpp::messages::StopTransactionConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::LogStatusNotificationReq&, ocpp::messages::LogStatusNotificationConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<
            bool(const ocpp::messages::LogStatusNotificationReq&, ocpp::messages::LogStatusNotificationConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::SecurityEventNotificationReq&,
                                         ocpp::messages::SecurityEventNotificationConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::SecurityEventNotificationReq&,
                                            ocpp::messages::SecurityEventNotificationConf&,
                                            std::string&,
                                            std::string&)> handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::SignCertificateReq&, ocpp::messages::SignCertificateConf&, std::string&, std::string&)>) */
    bool registerHandler(
        std::function<bool(const ocpp::messages::SignCertificateReq&, ocpp::messages::SignCertificateConf&, std::string&, std::string&)>
            handler) override;

    /** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::SignedFirmwareStatusNotificationReq&,
                                         ocpp::messages::SignedFirmwareStatusNotificationConf&, std::string&, std::string&)>) */
    bool registerHandler(std::function<bool(const ocpp::messages::SignedFirmwareStatusNotificationReq&,
                                            ocpp::messages::SignedFirmwareStatusNotificationConf&,
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
    ocpp::messages::MessagesConverter& m_messages_converter;
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
        UserMessageHandler<RequestType, ResponseType>* msg_handler =
            new UserMessageHandler<RequestType, ResponseType>(action, m_messages_converter, handler);
        m_user_handlers.push_back(std::shared_ptr<ocpp::messages::IMessageDispatcher::IMessageHandler>(msg_handler));
        return m_msg_dispatcher.registerHandler(action, *msg_handler, true);
    }
};

} // namespace localcontroller
} // namespace ocpp

#endif // OPENOCPP_CHARGEPOINTPROXY_H
