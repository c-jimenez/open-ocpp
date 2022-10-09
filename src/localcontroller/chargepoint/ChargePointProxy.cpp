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

#include "ChargePointProxy.h"
#include "ILocalControllerConfig.h"
#include "MessagesConverter.h"

using namespace ocpp::types;
using namespace ocpp::messages;

namespace ocpp
{
namespace localcontroller
{

/** @brief Constructor */
ChargePointProxy::ChargePointProxy(const std::string&                            identifier,
                                   std::shared_ptr<ocpp::rpc::RpcServer::Client> rpc,
                                   const ocpp::messages::MessagesValidator&      messages_validator,
                                   ocpp::messages::MessagesConverter&            messages_converter,
                                   const ocpp::config::ILocalControllerConfig&   stack_config,
                                   std::shared_ptr<ICentralSystemProxy>          central_system)
    : m_identifier(identifier),
      m_rpc(rpc),
      m_msg_dispatcher(messages_validator),
      m_msg_sender(*m_rpc, messages_converter, messages_validator, stack_config.callRequestTimeout()),
      m_central_system(central_system),
      m_handler(m_identifier, messages_converter, m_msg_dispatcher, *central_system.get()),
      m_listener(nullptr)
{
    m_rpc->registerSpy(*this);
    m_rpc->registerListener(*this);
}

/** @brief Destructor */
ChargePointProxy::~ChargePointProxy()
{
    if (m_rpc->isConnected())
    {
        // Disconnect from the charge point
        m_rpc->disconnect(false);

        // Disconnect from the central system
        m_central_system->disconnect();
    }
}

// IChargePointProxy interface

/** @copydoc const std::string& IChargePointProxy::ipAddress() const */
const std::string& ChargePointProxy::ipAddress() const
{
    return m_rpc->ipAddress();
}

/** @copydoc void IChargePointProxy::setTimeout(std::chrono::milliseconds) */
void ChargePointProxy::setTimeout(std::chrono::milliseconds timeout)
{
    m_msg_sender.setTimeout(timeout);
}

/** @copydoc void IChargePointProxy::disconnect() */
void ChargePointProxy::disconnect()
{
    // Disconnect from the charge point
    m_rpc->disconnect(true);

    // Disconnect from the central system
    m_central_system->disconnect();
}

// OCPP operations

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::CancelReservationReq&,
 *                                        ocpp::messages::CancelReservationConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::CancelReservationReq& request,
                            ocpp::messages::CancelReservationConf&      response,
                            std::string&                                error,
                            std::string&                                message)
{
    return call(CANCEL_RESERVATION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ChangeAvailabilityReq&,
 *                                        ocpp::messages::ChangeAvailabilityConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ChangeAvailabilityReq& request,
                            ocpp::messages::ChangeAvailabilityConf&      response,
                            std::string&                                 error,
                            std::string&                                 message)
{
    return call(CHANGE_AVAILABILITY_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ChangeConfigurationReq&,
 *                                        ocpp::messages::ChangeConfigurationConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ChangeConfigurationReq& request,
                            ocpp::messages::ChangeConfigurationConf&      response,
                            std::string&                                  error,
                            std::string&                                  message)
{
    return call(CHANGE_CONFIGURATION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ClearCacheReq&,
 *                                        ocpp::messages::ClearCacheConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ClearCacheReq& request,
                            ocpp::messages::ClearCacheConf&      response,
                            std::string&                         error,
                            std::string&                         message)
{
    return call(CLEAR_CACHE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ClearChargingProfileReq&,
 *                                        cpp::messages::ClearChargingProfileConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ClearChargingProfileReq& request,
                            ocpp::messages::ClearChargingProfileConf&      response,
                            std::string&                                   error,
                            std::string&                                   message)
{
    return call(CLEAR_CHARGING_PROFILE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::DataTransferReq&,
 *                                        ocpp::messages::DataTransferConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::DataTransferReq& request,
                            ocpp::messages::DataTransferConf&      response,
                            std::string&                           error,
                            std::string&                           message)
{
    return call(DATA_TRANSFER_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::GetCompositeScheduleReq&,
 *                                        ocpp::messages::GetCompositeScheduleConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::GetCompositeScheduleReq& request,
                            ocpp::messages::GetCompositeScheduleConf&      response,
                            std::string&                                   error,
                            std::string&                                   message)
{
    return call(GET_COMPOSITE_SCHEDULE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::GetConfigurationReq&,
 *                                        ocpp::messages::GetConfigurationConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::GetConfigurationReq& request,
                            ocpp::messages::GetConfigurationConf&      response,
                            std::string&                               error,
                            std::string&                               message)
{
    return call(GET_CONFIGURATION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::GetDiagnosticsReq&,
 *                                        ocpp::messages::GetDiagnosticsConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::GetDiagnosticsReq& request,
                            ocpp::messages::GetDiagnosticsConf&      response,
                            std::string&                             error,
                            std::string&                             message)
{
    return call(GET_DIAGNOSTICS_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::GetLocalListVersionReq&,
 *                                        ocpp::messages::GetLocalListVersionConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::GetLocalListVersionReq& request,
                            ocpp::messages::GetLocalListVersionConf&      response,
                            std::string&                                  error,
                            std::string&                                  message)
{
    return call(GET_LOCAL_LIST_VERSION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::RemoteStartTransactionReq&,
 *                                        ocpp::messages::RemoteStartTransactionConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::RemoteStartTransactionReq& request,
                            ocpp::messages::RemoteStartTransactionConf&      response,
                            std::string&                                     error,
                            std::string&                                     message)
{
    return call(REMOTE_START_TRANSACTION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::RemoteStopTransactionReq&,
 *                                        ocpp::messages::RemoteStopTransactionConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::RemoteStopTransactionReq& request,
                            ocpp::messages::RemoteStopTransactionConf&      response,
                            std::string&                                    error,
                            std::string&                                    message)
{
    return call(REMOTE_STOP_TRANSACTION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ReserveNowReq&,
 *                                        ocpp::messages::ReserveNowConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ReserveNowReq& request,
                            ocpp::messages::ReserveNowConf&      response,
                            std::string&                         error,
                            std::string&                         message)
{
    return call(RESERVE_NOW_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ResetReq&,
 *                                        ocpp::messages::ResetConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ResetReq& request,
                            ocpp::messages::ResetConf&      response,
                            std::string&                    error,
                            std::string&                    message)
{
    return call(RESET_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::SendLocalListReq&,
 *                                        ocpp::messages::SendLocalListConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::SendLocalListReq& request,
                            ocpp::messages::SendLocalListConf&      response,
                            std::string&                            error,
                            std::string&                            message)
{
    return call(SEND_LOCAL_LIST_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::SetChargingProfileReq&,
 *                                        ocpp::messages::SetChargingProfileConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::SetChargingProfileReq& request,
                            ocpp::messages::SetChargingProfileConf&      response,
                            std::string&                                 error,
                            std::string&                                 message)
{
    return call(SET_CHARGING_PROFILE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::TriggerMessageReq&,
 *                                        opp::messages::TriggerMessageConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::TriggerMessageReq& request,
                            ocpp::messages::TriggerMessageConf&      response,
                            std::string&                             error,
                            std::string&                             message)
{
    return call(TRIGGER_MESSAGE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::UnlockConnectorReq&,
 *                                        ocpp::messages::UnlockConnectorConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::UnlockConnectorReq& request,
                            ocpp::messages::UnlockConnectorConf&      response,
                            std::string&                              error,
                            std::string&                              message)
{
    return call(UNLOCK_CONNECTOR_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::UpdateFirmwareReq&,
 *                                        ocpp::messages::UpdateFirmwareConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::UpdateFirmwareReq& request,
                            ocpp::messages::UpdateFirmwareConf&      response,
                            std::string&                             error,
                            std::string&                             message)
{
    return call(UPDATE_FIRMWARE_ACTION, request, response, error, message);
}

// Security extensions

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::CertificateSignedReq&,
 *                                        ocpp::messages::CertificateSignedConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::CertificateSignedReq& request,
                            ocpp::messages::CertificateSignedConf&      response,
                            std::string&                                error,
                            std::string&                                message)
{
    return call(CERTIFICATE_SIGNED_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::DeleteCertificateReq&,
 *                                        ocpp::messages::DeleteCertificateConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::DeleteCertificateReq& request,
                            ocpp::messages::DeleteCertificateConf&      response,
                            std::string&                                error,
                            std::string&                                message)
{
    return call(DELETE_CERTIFICATE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ExtendedTriggerMessageReq&,
 *                                        ocpp::messages::ExtendedTriggerMessageConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ExtendedTriggerMessageReq& request,
                            ocpp::messages::ExtendedTriggerMessageConf&      response,
                            std::string&                                     error,
                            std::string&                                     message)
{
    return call(EXTENDED_TRIGGER_MESSAGE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::GetInstalledCertificateIdsReq&,
 *                                        ocpp::messages::GetInstalledCertificateIdsConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::GetInstalledCertificateIdsReq& request,
                            ocpp::messages::GetInstalledCertificateIdsConf&      response,
                            std::string&                                         error,
                            std::string&                                         message)
{
    return call(GET_INSTALLED_CERTIFICATE_IDS_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::GetLogReq&,
 *                                        ocpp::messages::GetLogConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::GetLogReq& request,
                            ocpp::messages::GetLogConf&      response,
                            std::string&                     error,
                            std::string&                     message)
{
    return call(GET_LOG_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::InstallCertificateReq&,
 *                                        ocpp::messages::InstallCertificateConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::InstallCertificateReq& request,
                            ocpp::messages::InstallCertificateConf&      response,
                            std::string&                                 error,
                            std::string&                                 message)
{
    return call(INSTALL_CERTIFICATE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::SignedUpdateFirmwareReq&,
 *                                        ocpp::messages::SignedUpdateFirmwareConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::SignedUpdateFirmwareReq& request,
                            ocpp::messages::SignedUpdateFirmwareConf&      response,
                            std::string&                                   error,
                            std::string&                                   message)
{
    return call(SIGNED_UPDATE_FIRMWARE_ACTION, request, response, error, message);
}
// IRpc::IListener interface

/** @copydoc void IRpc::IListener::rpcDisconnected() */
void ChargePointProxy::rpcDisconnected()
{
    LOG_WARNING << "[" << m_identifier << "] - Disconnected from Charge Point";

    // Disconnect from the central system
    m_central_system->disconnect();

    // Notify listener
    if (m_listener)
    {
        m_listener->disconnectedFromChargePoint();
    }
}

/** @copydoc void IRpc::IListener::rpcError() */
void ChargePointProxy::rpcError()
{
    LOG_ERROR << "[" << m_identifier << "] - Connection error";
}

/** @copydoc bool IRpc::IListener::rpcCallReceived(const std::string&,
                                                   const rapidjson::Value&,
                                                   rapidjson::Document&,
                                                   const char*&,
                                                   std::string&) */
bool ChargePointProxy::rpcCallReceived(const std::string&      action,
                                       const rapidjson::Value& payload,
                                       rapidjson::Document&    response,
                                       const char*&            error_code,
                                       std::string&            error_message)
{
    return m_msg_dispatcher.dispatchMessage(action, payload, response, error_code, error_message);
}

// IRpc::ISpy interface

/** @copydoc void IRpc::ISpy::rcpMessageReceived(const std::string& msg) */
void ChargePointProxy::rcpMessageReceived(const std::string& msg)
{
    LOG_COM << "[" << m_identifier << "] - RX : " << msg;
}

void ChargePointProxy::rcpMessageSent(const std::string& msg)
{
    LOG_COM << "[" << m_identifier << "] - TX : " << msg;
}

} // namespace localcontroller
} // namespace ocpp
