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

#include "ChargePointProxy.h"
#include "../centralsystem/chargepoint/ChargePointProxy.h"
#include "CentralSystemProxy.h"
#include "ILocalControllerConfig.h"
#include "MessagesConverter.h"

using namespace ocpp::messages;
using namespace ocpp::messages::ocpp16;
using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace localcontroller
{

/** @brief Instanciate local controller's charge point proxy from a central system's charge point proxy */
std::shared_ptr<IChargePointProxy> IChargePointProxy::createFrom(
    std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint>& central_system_proxy,
    const ocpp::config::ILocalControllerConfig&                         stack_config,
    ocpp::rpc::RpcPool&                                                 rpc_pool)
{
    std::shared_ptr<IChargePointProxy> proxy;

    // Instanciation can only be done from centralsytem::ChargePointProxy instance
    ocpp::centralsystem::ChargePointProxy* cs_proxy = dynamic_cast<ocpp::centralsystem::ChargePointProxy*>(central_system_proxy.get());
    if (cs_proxy)
    {
        // Create associated Central System proxy
        CentralSystemProxy* centralsystem = new CentralSystemProxy(
            cs_proxy->identifier(), cs_proxy->messagesValidator(), cs_proxy->messagesConverter(), stack_config, rpc_pool);

        // Create the proxy
        proxy = std::shared_ptr<IChargePointProxy>(new ChargePointProxy(cs_proxy->identifier(),
                                                                        cs_proxy->rpcClient(),
                                                                        cs_proxy->messagesValidator(),
                                                                        cs_proxy->messagesConverter(),
                                                                        stack_config,
                                                                        std::shared_ptr<ICentralSystemProxy>(centralsystem)));

        // Associate both
        centralsystem->setChargePointProxy(proxy);

        // Unregister old proxy from RPC spy events
        cs_proxy->unregisterFromRpcSpy();
    }

    return proxy;
}

/** @brief Constructor */
ChargePointProxy::ChargePointProxy(const std::string&                               identifier,
                                   std::shared_ptr<ocpp::rpc::RpcServer::Client>    rpc,
                                   const ocpp::messages::ocpp16::MessagesValidator& messages_validator,
                                   ocpp::messages::ocpp16::MessagesConverter&       messages_converter,
                                   const ocpp::config::ILocalControllerConfig&      stack_config,
                                   std::shared_ptr<ICentralSystemProxy>             central_system)
    : m_identifier(identifier),
      m_rpc(rpc),
      m_messages_converter(messages_converter),
      m_msg_dispatcher(messages_validator),
      m_msg_sender(*m_rpc, messages_converter, messages_validator, stack_config.callRequestTimeout()),
      m_central_system(central_system),
      m_handler(m_identifier, messages_converter, m_msg_dispatcher, *central_system.get()),
      m_listener(nullptr),
      m_user_handlers()
{
    m_rpc->registerSpy(*this);
    m_rpc->registerListener(*this);
}

/** @brief Destructor */
ChargePointProxy::~ChargePointProxy()
{
    // Disconnect from the charge point
    m_rpc->disconnect(false);

    // Disconnect from the central system
    m_central_system->disconnect();
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

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::CancelReservationReq&,
 *                                        ocpp::messages::ocpp16::CancelReservationConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::CancelReservationReq& request,
                            ocpp::messages::ocpp16::CancelReservationConf&      response,
                            std::string&                                        error,
                            std::string&                                        message)
{
    return call(CANCEL_RESERVATION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::ChangeAvailabilityReq&,
 *                                        ocpp::messages::ocpp16::ChangeAvailabilityConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::ChangeAvailabilityReq& request,
                            ocpp::messages::ocpp16::ChangeAvailabilityConf&      response,
                            std::string&                                         error,
                            std::string&                                         message)
{
    return call(CHANGE_AVAILABILITY_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::ChangeConfigurationReq&,
 *                                        ocpp::messages::ocpp16::ChangeConfigurationConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::ChangeConfigurationReq& request,
                            ocpp::messages::ocpp16::ChangeConfigurationConf&      response,
                            std::string&                                          error,
                            std::string&                                          message)
{
    return call(CHANGE_CONFIGURATION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::ClearCacheReq&,
 *                                        ocpp::messages::ocpp16::ClearCacheConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::ClearCacheReq& request,
                            ocpp::messages::ocpp16::ClearCacheConf&      response,
                            std::string&                                 error,
                            std::string&                                 message)
{
    return call(CLEAR_CACHE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::ClearChargingProfileReq&,
 *                                        cpp::messages::ClearChargingProfileConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::ClearChargingProfileReq& request,
                            ocpp::messages::ocpp16::ClearChargingProfileConf&      response,
                            std::string&                                           error,
                            std::string&                                           message)
{
    return call(CLEAR_CHARGING_PROFILE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::DataTransferReq&,
 *                                        ocpp::messages::ocpp16::DataTransferConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::DataTransferReq& request,
                            ocpp::messages::ocpp16::DataTransferConf&      response,
                            std::string&                                   error,
                            std::string&                                   message)
{
    return call(DATA_TRANSFER_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::GetCompositeScheduleReq&,
 *                                        ocpp::messages::ocpp16::GetCompositeScheduleConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::GetCompositeScheduleReq& request,
                            ocpp::messages::ocpp16::GetCompositeScheduleConf&      response,
                            std::string&                                           error,
                            std::string&                                           message)
{
    return call(GET_COMPOSITE_SCHEDULE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::GetConfigurationReq&,
 *                                        ocpp::messages::ocpp16::GetConfigurationConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::GetConfigurationReq& request,
                            ocpp::messages::ocpp16::GetConfigurationConf&      response,
                            std::string&                                       error,
                            std::string&                                       message)
{
    return call(GET_CONFIGURATION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::GetDiagnosticsReq&,
 *                                        ocpp::messages::ocpp16::GetDiagnosticsConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::GetDiagnosticsReq& request,
                            ocpp::messages::ocpp16::GetDiagnosticsConf&      response,
                            std::string&                                     error,
                            std::string&                                     message)
{
    return call(GET_DIAGNOSTICS_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::GetLocalListVersionReq&,
 *                                        ocpp::messages::ocpp16::GetLocalListVersionConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::GetLocalListVersionReq& request,
                            ocpp::messages::ocpp16::GetLocalListVersionConf&      response,
                            std::string&                                          error,
                            std::string&                                          message)
{
    return call(GET_LOCAL_LIST_VERSION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::RemoteStartTransactionReq&,
 *                                        ocpp::messages::ocpp16::RemoteStartTransactionConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::RemoteStartTransactionReq& request,
                            ocpp::messages::ocpp16::RemoteStartTransactionConf&      response,
                            std::string&                                             error,
                            std::string&                                             message)
{
    return call(REMOTE_START_TRANSACTION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::RemoteStopTransactionReq&,
 *                                        ocpp::messages::ocpp16::RemoteStopTransactionConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::RemoteStopTransactionReq& request,
                            ocpp::messages::ocpp16::RemoteStopTransactionConf&      response,
                            std::string&                                            error,
                            std::string&                                            message)
{
    return call(REMOTE_STOP_TRANSACTION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::ReserveNowReq&,
 *                                        ocpp::messages::ocpp16::ReserveNowConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::ReserveNowReq& request,
                            ocpp::messages::ocpp16::ReserveNowConf&      response,
                            std::string&                                 error,
                            std::string&                                 message)
{
    return call(RESERVE_NOW_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::ResetReq&,
 *                                        ocpp::messages::ocpp16::ResetConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::ResetReq& request,
                            ocpp::messages::ocpp16::ResetConf&      response,
                            std::string&                            error,
                            std::string&                            message)
{
    return call(RESET_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::SendLocalListReq&,
 *                                        ocpp::messages::ocpp16::SendLocalListConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::SendLocalListReq& request,
                            ocpp::messages::ocpp16::SendLocalListConf&      response,
                            std::string&                                    error,
                            std::string&                                    message)
{
    return call(SEND_LOCAL_LIST_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::SetChargingProfileReq&,
 *                                        ocpp::messages::ocpp16::SetChargingProfileConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::SetChargingProfileReq& request,
                            ocpp::messages::ocpp16::SetChargingProfileConf&      response,
                            std::string&                                         error,
                            std::string&                                         message)
{
    return call(SET_CHARGING_PROFILE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::TriggerMessageReq&,
 *                                        opp::messages::TriggerMessageConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::TriggerMessageReq& request,
                            ocpp::messages::ocpp16::TriggerMessageConf&      response,
                            std::string&                                     error,
                            std::string&                                     message)
{
    return call(TRIGGER_MESSAGE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::UnlockConnectorReq&,
 *                                        ocpp::messages::ocpp16::UnlockConnectorConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::UnlockConnectorReq& request,
                            ocpp::messages::ocpp16::UnlockConnectorConf&      response,
                            std::string&                                      error,
                            std::string&                                      message)
{
    return call(UNLOCK_CONNECTOR_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::UpdateFirmwareReq&,
 *                                        ocpp::messages::ocpp16::UpdateFirmwareConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::UpdateFirmwareReq& request,
                            ocpp::messages::ocpp16::UpdateFirmwareConf&      response,
                            std::string&                                     error,
                            std::string&                                     message)
{
    return call(UPDATE_FIRMWARE_ACTION, request, response, error, message);
}

// Security extensions

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::CertificateSignedReq&,
 *                                        ocpp::messages::ocpp16::CertificateSignedConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::CertificateSignedReq& request,
                            ocpp::messages::ocpp16::CertificateSignedConf&      response,
                            std::string&                                        error,
                            std::string&                                        message)
{
    return call(CERTIFICATE_SIGNED_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::DeleteCertificateReq&,
 *                                        ocpp::messages::ocpp16::DeleteCertificateConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::DeleteCertificateReq& request,
                            ocpp::messages::ocpp16::DeleteCertificateConf&      response,
                            std::string&                                        error,
                            std::string&                                        message)
{
    return call(DELETE_CERTIFICATE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::ExtendedTriggerMessageReq&,
 *                                        ocpp::messages::ocpp16::ExtendedTriggerMessageConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::ExtendedTriggerMessageReq& request,
                            ocpp::messages::ocpp16::ExtendedTriggerMessageConf&      response,
                            std::string&                                             error,
                            std::string&                                             message)
{
    return call(EXTENDED_TRIGGER_MESSAGE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::GetInstalledCertificateIdsReq&,
 *                                        ocpp::messages::ocpp16::GetInstalledCertificateIdsConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::GetInstalledCertificateIdsReq& request,
                            ocpp::messages::ocpp16::GetInstalledCertificateIdsConf&      response,
                            std::string&                                                 error,
                            std::string&                                                 message)
{
    return call(GET_INSTALLED_CERTIFICATE_IDS_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::GetLogReq&,
 *                                        ocpp::messages::ocpp16::GetLogConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::GetLogReq& request,
                            ocpp::messages::ocpp16::GetLogConf&      response,
                            std::string&                             error,
                            std::string&                             message)
{
    return call(GET_LOG_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::InstallCertificateReq&,
 *                                        ocpp::messages::ocpp16::InstallCertificateConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::InstallCertificateReq& request,
                            ocpp::messages::ocpp16::InstallCertificateConf&      response,
                            std::string&                                         error,
                            std::string&                                         message)
{
    return call(INSTALL_CERTIFICATE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::call(const ocpp::messages::ocpp16::SignedUpdateFirmwareReq&,
 *                                        ocpp::messages::ocpp16::SignedUpdateFirmwareConf&,
 *                                        std::string&,
 *                                        std::string&) */
bool ChargePointProxy::call(const ocpp::messages::ocpp16::SignedUpdateFirmwareReq& request,
                            ocpp::messages::ocpp16::SignedUpdateFirmwareConf&      response,
                            std::string&                                           error,
                            std::string&                                           message)
{
    return call(SIGNED_UPDATE_FIRMWARE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::AuthorizeReq&, ocpp::messages::ocpp16::AuthorizeConf&, std::string&, std::string&)>) */
bool ChargePointProxy::registerHandler(
    std::function<bool(const ocpp::messages::ocpp16::AuthorizeReq&, ocpp::messages::ocpp16::AuthorizeConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(AUTHORIZE_ACTION, handler);
}

/** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::BootNotificationReq&, ocpp::messages::ocpp16::BootNotificationConf&, std::string&, std::string&)>) */
bool ChargePointProxy::registerHandler(
    std::function<
        bool(const ocpp::messages::ocpp16::BootNotificationReq&, ocpp::messages::ocpp16::BootNotificationConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(BOOT_NOTIFICATION_ACTION, handler);
}

/** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::DataTransferReq&, ocpp::messages::ocpp16::DataTransferConf&, std::string&, std::string&)>) */
bool ChargePointProxy::registerHandler(
    std::function<bool(
        const ocpp::messages::ocpp16::DataTransferReq&, ocpp::messages::ocpp16::DataTransferConf&, std::string&, std::string&)> handler)

{
    return registerHandler(DATA_TRANSFER_ACTION, handler);
}

/** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::DiagnosticsStatusNotificationReq&,
                                         ocpp::messages::ocpp16::DiagnosticsStatusNotificationConf&, std::string&, std::string&)>) */
bool ChargePointProxy::registerHandler(std::function<bool(const ocpp::messages::ocpp16::DiagnosticsStatusNotificationReq&,
                                                          ocpp::messages::ocpp16::DiagnosticsStatusNotificationConf&,
                                                          std::string&,
                                                          std::string&)> handler)
{
    return registerHandler(DIAGNOSTIC_STATUS_NOTIFICATION_ACTION, handler);
}

/** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::FirmwareStatusNotificationReq&,
                                         ocpp::messages::ocpp16::FirmwareStatusNotificationConf&, std::string&, std::string&)>) */
bool ChargePointProxy::registerHandler(std::function<bool(const ocpp::messages::ocpp16::FirmwareStatusNotificationReq&,
                                                          ocpp::messages::ocpp16::FirmwareStatusNotificationConf&,
                                                          std::string&,
                                                          std::string&)> handler)
{
    return registerHandler(FIRMWARE_STATUS_NOTIFICATION_ACTION, handler);
}

/** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::HeartbeatReq&, ocpp::messages::ocpp16::HeartbeatConf&, std::string&, std::string&)>) */
bool ChargePointProxy::registerHandler(
    std::function<bool(const ocpp::messages::ocpp16::HeartbeatReq&, ocpp::messages::ocpp16::HeartbeatConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(HEARTBEAT_ACTION, handler);
}

/** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::MeterValuesReq&, ocpp::messages::ocpp16::MeterValuesConf&, std::string&, std::string&)>) */
bool ChargePointProxy::registerHandler(
    std::function<bool(const ocpp::messages::ocpp16::MeterValuesReq&, ocpp::messages::ocpp16::MeterValuesConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(METER_VALUES_ACTION, handler);
}

/** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::StartTransactionReq&, ocpp::messages::ocpp16::StartTransactionConf&, std::string&, std::string&)>) */
bool ChargePointProxy::registerHandler(
    std::function<
        bool(const ocpp::messages::ocpp16::StartTransactionReq&, ocpp::messages::ocpp16::StartTransactionConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(START_TRANSACTION_ACTION, handler);
}

/** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::StatusNotificationReq&, ocpp::messages::ocpp16::StatusNotificationConf&, std::string&, std::string&)>) */
bool ChargePointProxy::registerHandler(
    std::function<bool(
        const ocpp::messages::ocpp16::StatusNotificationReq&, ocpp::messages::ocpp16::StatusNotificationConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(STATUS_NOTIFICATION_ACTION, handler);
}

/** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::StopTransactionReq&, ocpp::messages::ocpp16::StopTransactionConf&, std::string&, std::string&)>) */
bool ChargePointProxy::registerHandler(
    std::function<
        bool(const ocpp::messages::ocpp16::StopTransactionReq&, ocpp::messages::ocpp16::StopTransactionConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(STOP_TRANSACTION_ACTION, handler);
}

/** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::LogStatusNotificationReq&, ocpp::messages::ocpp16::LogStatusNotificationConf&, std::string&, std::string&)>) */
bool ChargePointProxy::registerHandler(std::function<bool(const ocpp::messages::ocpp16::LogStatusNotificationReq&,
                                                          ocpp::messages::ocpp16::LogStatusNotificationConf&,
                                                          std::string&,
                                                          std::string&)> handler)

{
    return registerHandler(LOG_STATUS_NOTIFICATION_ACTION, handler);
}

/** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::SecurityEventNotificationReq&,
                                         ocpp::messages::ocpp16::SecurityEventNotificationConf&, std::string&, std::string&)>) */
bool ChargePointProxy::registerHandler(std::function<bool(const ocpp::messages::ocpp16::SecurityEventNotificationReq&,
                                                          ocpp::messages::ocpp16::SecurityEventNotificationConf&,
                                                          std::string&,
                                                          std::string&)> handler)
{
    return registerHandler(SECURITY_EVENT_NOTIFICATION_ACTION, handler);
}

/** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::SignCertificateReq&, ocpp::messages::ocpp16::SignCertificateConf&, std::string&, std::string&)>) */
bool ChargePointProxy::registerHandler(
    std::function<
        bool(const ocpp::messages::ocpp16::SignCertificateReq&, ocpp::messages::ocpp16::SignCertificateConf&, std::string&, std::string&)>
        handler)
{
    return registerHandler(SIGN_CERTIFICATE_ACTION, handler);
}

/** @copydoc bool IChargePointProxy::registerHandler(
                      std::function<bool(const ocpp::messages::ocpp16::SignedFirmwareStatusNotificationReq&,
                                         ocpp::messages::ocpp16::SignedFirmwareStatusNotificationConf&, std::string&, std::string&)>) */
bool ChargePointProxy::registerHandler(std::function<bool(const ocpp::messages::ocpp16::SignedFirmwareStatusNotificationReq&,
                                                          ocpp::messages::ocpp16::SignedFirmwareStatusNotificationConf&,
                                                          std::string&,
                                                          std::string&)> handler)
{
    return registerHandler(SIGNED_FIRMWARE_STATUS_NOTIFICATION_ACTION, handler);
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
                                                   std::string&,
                                                   std::string&) */
bool ChargePointProxy::rpcCallReceived(const std::string&      action,
                                       const rapidjson::Value& payload,
                                       rapidjson::Document&    response,
                                       std::string&            error_code,
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
