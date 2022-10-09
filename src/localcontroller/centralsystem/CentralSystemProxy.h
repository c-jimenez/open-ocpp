/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
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
#include "MessagesValidator.h"
#include "RpcClient.h"

namespace ocpp
{
namespace config
{
class ILocalControllerConfig;
} // namespace config

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
     */
    CentralSystemProxy(const std::string&                          identifier,
                       const ocpp::messages::MessagesValidator&    messages_validator,
                       ocpp::messages::MessagesConverter&          messages_converter,
                       const ocpp::config::ILocalControllerConfig& stack_config);

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

    /** @copydoc void ICentralSystemProxy::registerListener(ILocalControllerProxyEventsHandler&) */
    void registerListener(ILocalControllerProxyEventsHandler& listener) override { m_listener = &listener; }

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::BootNotificationReq&,
     *                                          ocpp::messages::BootNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::BootNotificationReq& request,
              ocpp::messages::BootNotificationConf&      response,
              std::string&                               error,
              std::string&                               message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::StatusNotificationReq&,
     *                                          ocpp::messages::StatusNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::StatusNotificationReq& request,
              ocpp::messages::StatusNotificationConf&      response,
              std::string&                                 error,
              std::string&                                 message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::AuthorizeReq&,
     *                                          ocpp::messages::AuthorizeConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::AuthorizeReq& request,
              ocpp::messages::AuthorizeConf&      response,
              std::string&                        error,
              std::string&                        message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::StartTransactionReq&,
     *                                          ocpp::messages::StartTransactionConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::StartTransactionReq& request,
              ocpp::messages::StartTransactionConf&      response,
              std::string&                               error,
              std::string&                               message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::StopTransactionReq&,
     *                                          ocpp::messages::StopTransactionConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::StopTransactionReq& request,
              ocpp::messages::StopTransactionConf&      response,
              std::string&                              error,
              std::string&                              message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::DataTransferReq&,
     *                                          ocpp::messages::DataTransferConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::DataTransferReq& request,
              ocpp::messages::DataTransferConf&      response,
              std::string&                           error,
              std::string&                           message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::MeterValuesReq&,
     *                                          ocpp::messages::MeterValuesConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::MeterValuesReq& request,
              ocpp::messages::MeterValuesConf&      response,
              std::string&                          error,
              std::string&                          message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::DiagnosticsStatusNotificationReq&,
     *                                          ocpp::messages::DiagnosticsStatusNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::DiagnosticsStatusNotificationReq& request,
              ocpp::messages::DiagnosticsStatusNotificationConf&      response,
              std::string&                                            error,
              std::string&                                            message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::FirmwareStatusNotificationReq&,
     *                                          ocpp::messages::FirmwareStatusNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::FirmwareStatusNotificationReq& request,
              ocpp::messages::FirmwareStatusNotificationConf&      response,
              std::string&                                         error,
              std::string&                                         message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::HeartbeatReq&,
     *                                          ocpp::messages::HeartbeatConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::HeartbeatReq& request,
              ocpp::messages::HeartbeatConf&      response,
              std::string&                        error,
              std::string&                        message) override;

    // Security extensions

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::SecurityEventNotificationReq&,
     *                                          ocpp::messages::SecurityEventNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::SecurityEventNotificationReq& request,
              ocpp::messages::SecurityEventNotificationConf&      response,
              std::string&                                        error,
              std::string&                                        message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::SignCertificateReq&,
     *                                          ocpp::messages::SignCertificateConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::SignCertificateReq& request,
              ocpp::messages::SignCertificateConf&      response,
              std::string&                              error,
              std::string&                              message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::LogStatusNotificationReq&,
     *                                          ocpp::messages::LogStatusNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::LogStatusNotificationReq& request,
              ocpp::messages::LogStatusNotificationConf&      response,
              std::string&                                    error,
              std::string&                                    message) override;

    /** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::SignedFirmwareStatusNotificationReq&,
     *                                          ocpp::messages::SignedFirmwareStatusNotificationConf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::SignedFirmwareStatusNotificationReq& request,
              ocpp::messages::SignedFirmwareStatusNotificationConf&      response,
              std::string&                                               error,
              std::string&                                               message) override;

    // IRpc::IListener interface

    /** @copydoc void IRpc::IListener::rpcDisconnected() */
    void rpcDisconnected() override;

    /** @copydoc void IRpc::IListener::rpcError() */
    void rpcError() override;

    /** @copydoc bool IRpc::IListener::rpcCallReceived(const std::string&,
                                                       const rapidjson::Value&,
                                                       rapidjson::Document&,
                                                       const char*&,
                                                       std::string&) */
    bool rpcCallReceived(const std::string&      action,
                         const rapidjson::Value& payload,
                         rapidjson::Document&    response,
                         const char*&            error_code,
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
};

} // namespace localcontroller
} // namespace ocpp

#endif // OPENOCPP_CENTRALSYSTEMPROXY_H
