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

#include "CentralSystemProxy.h"
#include "ILocalControllerConfig.h"
#include "Url.h"
#include "WebsocketFactory.h"

#include <sstream>

using namespace ocpp::types;
using namespace ocpp::messages;

namespace ocpp
{
namespace localcontroller
{

/** @brief Constructor */
CentralSystemProxy::CentralSystemProxy(const std::string&                          identifier,
                                       const ocpp::messages::MessagesValidator&    messages_validator,
                                       ocpp::messages::MessagesConverter&          messages_converter,
                                       const ocpp::config::ILocalControllerConfig& stack_config,
                                       ocpp::rpc::RpcPool&                         rpc_pool)
    : m_identifier(identifier),
      m_stack_config(stack_config),
      m_websocket(ocpp::websockets::WebsocketFactory::newClientFromPool()),
      m_rpc(*m_websocket, "ocpp1.6", &rpc_pool),
      m_messages_converter(messages_converter),
      m_msg_dispatcher(messages_validator),
      m_msg_sender(m_rpc, messages_converter, messages_validator, stack_config.callRequestTimeout()),
      m_handler(m_identifier, messages_converter, m_msg_dispatcher),
      m_listener(nullptr),
      m_user_handlers()
{
    m_rpc.registerSpy(*this);
    m_rpc.registerListener(*this);
    m_rpc.registerClientListener(*this);
}

/** @brief Destructor */
CentralSystemProxy::~CentralSystemProxy()
{
    disconnect();
}

/** @copydoc void ICentralSystemProxy::setTimeout(std::chrono::milliseconds) */
void CentralSystemProxy::setTimeout(std::chrono::milliseconds timeout)
{
    m_msg_sender.setTimeout(timeout);
}

/** @copydoc bool ICentralSystemProxy::connect(const std::string&,
                                                   const ocpp::websockets::IWebsocketClient::Credentials&,
                                                   std::chrono::milliseconds,
                                                   std::chrono::milliseconds,
                                                   std::chrono::milliseconds) */
bool CentralSystemProxy::connect(const std::string&                                     url,
                                 const ocpp::websockets::IWebsocketClient::Credentials& credentials,
                                 std::chrono::milliseconds                              connect_timeout,
                                 std::chrono::milliseconds                              retry_interval,
                                 std::chrono::milliseconds                              ping_interval)
{
    bool ret = false;

    if (!url.empty())
    {
        // Build URL
        std::stringstream full_url;
        full_url << url;
        if (url[url.size() - 1] != '/')
        {
            full_url << "/";
        }
        full_url << ocpp::websockets::Url::encode(m_identifier);

        // Connect
        ret = m_rpc.start(full_url.str(), credentials, connect_timeout, retry_interval, ping_interval);
    }
    return ret;
}

/** @copydoc void ICentralSystemProxy::disconnect() */
void CentralSystemProxy::disconnect()
{
    m_rpc.stop();
}

/** @brief Indicate if the connection with the central system is active */
bool CentralSystemProxy::isConnected() const
{
    return m_rpc.isConnected();
}

/** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::BootNotificationReq&,
 *                                          ocpp::messages::BootNotificationConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool CentralSystemProxy::call(const ocpp::messages::BootNotificationReq& request,
                              ocpp::messages::BootNotificationConf&      response,
                              std::string&                               error,
                              std::string&                               message)
{
    return call(BOOT_NOTIFICATION_ACTION, request, response, error, message);
}

/** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::StatusNotificationReq&,
     *                                      ocpp::messages::StatusNotificationConf&,
     *                                      std::string&,
     *                                      std::string&) */
bool CentralSystemProxy::call(const ocpp::messages::StatusNotificationReq& request,
                              ocpp::messages::StatusNotificationConf&      response,
                              std::string&                                 error,
                              std::string&                                 message)
{
    return call(STATUS_NOTIFICATION_ACTION, request, response, error, message);
}

/** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::AuthorizeReq&,
     *                                      ocpp::messages::AuthorizeConf&,
     *                                      std::string&,
     *                                      std::string&) */
bool CentralSystemProxy::call(const ocpp::messages::AuthorizeReq& request,
                              ocpp::messages::AuthorizeConf&      response,
                              std::string&                        error,
                              std::string&                        message)
{
    return call(AUTHORIZE_ACTION, request, response, error, message);
}

/** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::StartTransactionReq&,
     *                                      ocpp::messages::StartTransactionConf&,
     *                                      std::string&,
     *                                      std::string&) */
bool CentralSystemProxy::call(const ocpp::messages::StartTransactionReq& request,
                              ocpp::messages::StartTransactionConf&      response,
                              std::string&                               error,
                              std::string&                               message)
{
    return call(START_TRANSACTION_ACTION, request, response, error, message);
}

/** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::StopTransactionReq&,
     *                                      ocpp::messages::StopTransactionConf&,
     *                                      std::string&,
     *                                      std::string&) */
bool CentralSystemProxy::call(const ocpp::messages::StopTransactionReq& request,
                              ocpp::messages::StopTransactionConf&      response,
                              std::string&                              error,
                              std::string&                              message)
{
    return call(STOP_TRANSACTION_ACTION, request, response, error, message);
}

/** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::DataTransferReq&,
     *                                      ocpp::messages::DataTransferConf&,
     *                                      std::string&,
     *                                      std::string&) */
bool CentralSystemProxy::call(const ocpp::messages::DataTransferReq& request,
                              ocpp::messages::DataTransferConf&      response,
                              std::string&                           error,
                              std::string&                           message)
{
    return call(DATA_TRANSFER_ACTION, request, response, error, message);
}

/** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::MeterValuesReq&,
     *                                      ocpp::messages::MeterValuesConf&,
     *                                      std::string&,
     *                                      std::string&) */
bool CentralSystemProxy::call(const ocpp::messages::MeterValuesReq& request,
                              ocpp::messages::MeterValuesConf&      response,
                              std::string&                          error,
                              std::string&                          message)
{
    return call(METER_VALUES_ACTION, request, response, error, message);
}

/** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::DiagnosticsStatusNotificationReq&,
     *                                      ocpp::messages::DiagnosticsStatusNotificationConf&,
     *                                      std::string&,
     *                                      std::string&) */
bool CentralSystemProxy::call(const ocpp::messages::DiagnosticsStatusNotificationReq& request,
                              ocpp::messages::DiagnosticsStatusNotificationConf&      response,
                              std::string&                                            error,
                              std::string&                                            message)
{
    return call(DIAGNOSTIC_STATUS_NOTIFICATION_ACTION, request, response, error, message);
}

/** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::FirmwareStatusNotificationReq&,
     *                                      ocpp::messages::FirmwareStatusNotificationConf&,
     *                                      std::string&,
     *                                      std::string&) */
bool CentralSystemProxy::call(const ocpp::messages::FirmwareStatusNotificationReq& request,
                              ocpp::messages::FirmwareStatusNotificationConf&      response,
                              std::string&                                         error,
                              std::string&                                         message)
{
    return call(FIRMWARE_STATUS_NOTIFICATION_ACTION, request, response, error, message);
}

/** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::HeartbeatReq&,
     *                                      ocpp::messages::HeartbeatConf&,
     *                                      std::string&,
     *                                      std::string&) */
bool CentralSystemProxy::call(const ocpp::messages::HeartbeatReq& request,
                              ocpp::messages::HeartbeatConf&      response,
                              std::string&                        error,
                              std::string&                        message)
{
    return call(HEARTBEAT_ACTION, request, response, error, message);
}

// Security extensions

/** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::SecurityEventNotificationReq&,
     *                                      ocpp::messages::SecurityEventNotificationConf&,
     *                                      std::string&,
     *                                      std::string&) */
bool CentralSystemProxy::call(const ocpp::messages::SecurityEventNotificationReq& request,
                              ocpp::messages::SecurityEventNotificationConf&      response,
                              std::string&                                        error,
                              std::string&                                        message)
{
    return call(SECURITY_EVENT_NOTIFICATION_ACTION, request, response, error, message);
}

/** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::SignCertificateReq&,
     *                                      ocpp::messages::SignCertificateConf&,
     *                                      std::string&,
     *                                      std::string&) */
bool CentralSystemProxy::call(const ocpp::messages::SignCertificateReq& request,
                              ocpp::messages::SignCertificateConf&      response,
                              std::string&                              error,
                              std::string&                              message)
{
    return call(SIGN_CERTIFICATE_ACTION, request, response, error, message);
}

/** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::LogStatusNotificationReq&,
     *                                      ocpp::messages::LogStatusNotificationConf&,
     *                                      std::string&,
     *                                      std::string&) */
bool CentralSystemProxy::call(const ocpp::messages::LogStatusNotificationReq& request,
                              ocpp::messages::LogStatusNotificationConf&      response,
                              std::string&                                    error,
                              std::string&                                    message)
{
    return call(LOG_STATUS_NOTIFICATION_ACTION, request, response, error, message);
}

/** @copydoc bool ICentralSystemProxy::call(const ocpp::messages::SignedFirmwareStatusNotificationReq&,
     *                                      ocpp::messages::SignedFirmwareStatusNotificationConf&,
     *                                      std::string&,
     *                                      std::string&) */
bool CentralSystemProxy::call(const ocpp::messages::SignedFirmwareStatusNotificationReq& request,
                              ocpp::messages::SignedFirmwareStatusNotificationConf&      response,
                              std::string&                                               error,
                              std::string&                                               message)
{
    return call(SIGNED_FIRMWARE_STATUS_NOTIFICATION_ACTION, request, response, error, message);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::CancelReservationReq&, ocpp::messages::CancelReservationConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::CancelReservationReq&, ocpp::messages::CancelReservationConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(CANCEL_RESERVATION_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ChangeAvailabilityReq&, ocpp::messages::ChangeAvailabilityConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::ChangeAvailabilityReq&, ocpp::messages::ChangeAvailabilityConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(CHANGE_AVAILABILITY_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ChangeConfigurationReq&, ocpp::messages::ChangeConfigurationConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::ChangeConfigurationReq&, ocpp::messages::ChangeConfigurationConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(CHANGE_CONFIGURATION_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ClearCacheReq&, ocpp::messages::ClearCacheConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::ClearCacheReq&, ocpp::messages::ClearCacheConf&, std::string&, std::string&)> handler)

{
    return registerHandler(CLEAR_CACHE_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ClearChargingProfileReq&, ocpp::messages::ClearChargingProfileConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(
        const ocpp::messages::ClearChargingProfileReq&, ocpp::messages::ClearChargingProfileConf&, std::string&, std::string&)> handler)

{
    return registerHandler(CLEAR_CHARGING_PROFILE_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::DataTransferReq&, ocpp::messages::DataTransferConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::DataTransferReq&, ocpp::messages::DataTransferConf&, std::string&, std::string&)> handler)

{
    return registerHandler(DATA_TRANSFER_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::GetCompositeScheduleReq&, ocpp::messages::GetCompositeScheduleConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(
        const ocpp::messages::GetCompositeScheduleReq&, ocpp::messages::GetCompositeScheduleConf&, std::string&, std::string&)> handler)

{
    return registerHandler(GET_COMPOSITE_SCHEDULE_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::GetConfigurationReq&, ocpp::messages::GetConfigurationConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::GetConfigurationReq&, ocpp::messages::GetConfigurationConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(GET_CONFIGURATION_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::GetDiagnosticsReq&, ocpp::messages::GetDiagnosticsConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::GetDiagnosticsReq&, ocpp::messages::GetDiagnosticsConf&, std::string&, std::string&)> handler)

{
    return registerHandler(GET_DIAGNOSTICS_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::GetLocalListVersionReq&, ocpp::messages::GetLocalListVersionConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::GetLocalListVersionReq&, ocpp::messages::GetLocalListVersionConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(GET_LOCAL_LIST_VERSION_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::RemoteStartTransactionReq&, ocpp::messages::RemoteStartTransactionConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(
        const ocpp::messages::RemoteStartTransactionReq&, ocpp::messages::RemoteStartTransactionConf&, std::string&, std::string&)> handler)

{
    return registerHandler(REMOTE_START_TRANSACTION_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::RemoteStopTransactionReq&, ocpp::messages::RemoteStopTransactionConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(
        const ocpp::messages::RemoteStopTransactionReq&, ocpp::messages::RemoteStopTransactionConf&, std::string&, std::string&)> handler)

{
    return registerHandler(REMOTE_STOP_TRANSACTION_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ReserveNowReq&, ocpp::messages::ReserveNowConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::ReserveNowReq&, ocpp::messages::ReserveNowConf&, std::string&, std::string&)> handler)

{
    return registerHandler(RESERVE_NOW_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ResetReq&, ocpp::messages::ResetConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::ResetReq&, ocpp::messages::ResetConf&, std::string&, std::string&)> handler)
{
    return registerHandler(RESET_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::SendLocalListReq&, ocpp::messages::SendLocalListConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::SendLocalListReq&, ocpp::messages::SendLocalListConf&, std::string&, std::string&)> handler)

{
    return registerHandler(SEND_LOCAL_LIST_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::SetChargingProfileReq&, ocpp::messages::SetChargingProfileConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::SetChargingProfileReq&, ocpp::messages::SetChargingProfileConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(SET_CHARGING_PROFILE_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::TriggerMessageReq&, ocpp::messages::TriggerMessageConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::TriggerMessageReq&, ocpp::messages::TriggerMessageConf&, std::string&, std::string&)> handler)

{
    return registerHandler(TRIGGER_MESSAGE_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::UnlockConnectorReq&, ocpp::messages::UnlockConnectorConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::UnlockConnectorReq&, ocpp::messages::UnlockConnectorConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(UNLOCK_CONNECTOR_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::UpdateFirmwareReq&, ocpp::messages::UpdateFirmwareConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::UpdateFirmwareReq&, ocpp::messages::UpdateFirmwareConf&, std::string&, std::string&)> handler)

{
    return registerHandler(UPDATE_FIRMWARE_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::CertificateSignedReq&, ocpp::messages::CertificateSignedConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::CertificateSignedReq&, ocpp::messages::CertificateSignedConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(CERTIFICATE_SIGNED_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::DeleteCertificateReq&, ocpp::messages::DeleteCertificateConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::DeleteCertificateReq&, ocpp::messages::DeleteCertificateConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(DELETE_CERTIFICATE_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ExtendedTriggerMessageReq&, ocpp::messages::ExtendedTriggerMessageConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(
        const ocpp::messages::ExtendedTriggerMessageReq&, ocpp::messages::ExtendedTriggerMessageConf&, std::string&, std::string&)> handler)

{
    return registerHandler(EXTENDED_TRIGGER_MESSAGE_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::GetInstalledCertificateIdsReq&, ocpp::messages::GetInstalledCertificateIdsConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(
        const ocpp::messages::GetInstalledCertificateIdsReq&, ocpp::messages::GetInstalledCertificateIdsConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(GET_INSTALLED_CERTIFICATE_IDS_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::GetLogReq&, ocpp::messages::GetLogConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::GetLogReq&, ocpp::messages::GetLogConf&, std::string&, std::string&)> handler)
{
    return registerHandler(GET_LOG_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::InstallCertificateReq&, ocpp::messages::InstallCertificateConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(const ocpp::messages::InstallCertificateReq&, ocpp::messages::InstallCertificateConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(INSTALL_CERTIFICATE_ACTION, handler);
}

/** @copydoc bool ICentralSystemProxy::registerHandler(
                      std::function<bool(const ocpp::messages::SignedUpdateFirmwareReq&, ocpp::messages::SignedUpdateFirmwareConf&, std::string&, std::string&)>) */
bool CentralSystemProxy::registerHandler(
    std::function<bool(
        const ocpp::messages::SignedUpdateFirmwareReq&, ocpp::messages::SignedUpdateFirmwareConf&, std::string&, std::string&)> handler)

{
    return registerHandler(SIGNED_UPDATE_FIRMWARE_ACTION, handler);
}

// IRpc::IListener interface

/** @copydoc void IRpc::IListener::rpcDisconnected() */
void CentralSystemProxy::rpcDisconnected()
{
    LOG_WARNING << "[" << m_identifier << "] - Disconnected from Central System";

    // Notify listener
    if (m_listener)
    {
        m_listener->disconnectedFromCentralSystem();
    }

    // Disconnect from Charge Point
    if (m_stack_config.disconnectFromCpWhenCsDisconnected())
    {
        auto charge_point = m_charge_point.lock();
        if (charge_point)
        {
            charge_point->disconnect();
        }
    }
}

/** @copydoc void IRpc::IListener::rpcError() */
void CentralSystemProxy::rpcError()
{
    LOG_ERROR << "[" << m_identifier << "] - Connection error";
}

/** @copydoc bool IRpc::IListener::rpcCallReceived(const std::string&,
                                                   const rapidjson::Value&,
                                                   rapidjson::Document&,
                                                   std::string&,
                                                   std::string&) */
bool CentralSystemProxy::rpcCallReceived(const std::string&      action,
                                         const rapidjson::Value& payload,
                                         rapidjson::Document&    response,
                                         std::string&            error_code,
                                         std::string&            error_message)
{
    return m_msg_dispatcher.dispatchMessage(action, payload, response, error_code, error_message);
}

// IRpc::ISpy interface

/** @copydoc void IRpc::ISpy::rcpMessageReceived(const std::string& msg) */
void CentralSystemProxy::rcpMessageReceived(const std::string& msg)
{
    LOG_COM << "[" << m_identifier << "] - RX : " << msg;
}

/** @copydoc void IRpc::ISpy::rcpMessageSent(const std::string& msg) */
void CentralSystemProxy::rcpMessageSent(const std::string& msg)
{
    LOG_COM << "[" << m_identifier << "] - TX : " << msg;
}

// RpcClient::IListener interface

/** @brief Called when connection is successfull */
void CentralSystemProxy::rpcClientConnected()
{
    LOG_INFO << "[" << m_identifier << "] - Connected to Central System";

    // Notify listener
    if (m_listener)
    {
        m_listener->connectedToCentralSystem();
    }
}

/** @brief Called when connection failed */
void CentralSystemProxy::rpcClientFailed()
{
    LOG_ERROR << "[" << m_identifier << "] - Failed to connect to Central System";

    // Notify listener
    if (m_listener)
    {
        m_listener->failedToConnectToCentralSystem();
    }

    // Disconnect from Charge Point
    if (m_stack_config.disconnectFromCpWhenCsDisconnected())
    {
        auto charge_point = m_charge_point.lock();
        if (charge_point)
        {
            charge_point->disconnect();
        }
    }
}

} // namespace localcontroller
} // namespace ocpp
