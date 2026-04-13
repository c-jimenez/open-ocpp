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

#include "CentralSystemHandler.h"
#include "ILocalControllerConfig.h"
#include "MessageDispatcher.h"
#include "MessagesConverter.h"

using namespace ocpp::messages;
using namespace ocpp::messages::ocpp16;
using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{

namespace localcontroller
{

/** @brief Constructor */
CentralSystemHandler::CentralSystemHandler(const std::string&                               identifier,
                                           const ocpp::messages::ocpp16::MessagesConverter& messages_converter,
                                           ocpp::messages::MessageDispatcher&               msg_dispatcher)
    : GenericMessageHandler<CancelReservationReq, CancelReservationConf>(CANCEL_RESERVATION_ACTION, messages_converter),
      GenericMessageHandler<ChangeAvailabilityReq, ChangeAvailabilityConf>(CHANGE_AVAILABILITY_ACTION, messages_converter),
      GenericMessageHandler<ChangeConfigurationReq, ChangeConfigurationConf>(CHANGE_CONFIGURATION_ACTION, messages_converter),
      GenericMessageHandler<ClearCacheReq, ClearCacheConf>(CLEAR_CACHE_ACTION, messages_converter),
      GenericMessageHandler<ClearChargingProfileReq, ClearChargingProfileConf>(CLEAR_CHARGING_PROFILE_ACTION, messages_converter),
      GenericMessageHandler<DataTransferReq, DataTransferConf>(DATA_TRANSFER_ACTION, messages_converter),
      GenericMessageHandler<GetCompositeScheduleReq, GetCompositeScheduleConf>(GET_COMPOSITE_SCHEDULE_ACTION, messages_converter),
      GenericMessageHandler<GetConfigurationReq, GetConfigurationConf>(GET_CONFIGURATION_ACTION, messages_converter),
      GenericMessageHandler<GetDiagnosticsReq, GetDiagnosticsConf>(GET_DIAGNOSTICS_ACTION, messages_converter),
      GenericMessageHandler<GetLocalListVersionReq, GetLocalListVersionConf>(GET_LOCAL_LIST_VERSION_ACTION, messages_converter),
      GenericMessageHandler<RemoteStartTransactionReq, RemoteStartTransactionConf>(REMOTE_START_TRANSACTION_ACTION, messages_converter),
      GenericMessageHandler<RemoteStopTransactionReq, RemoteStopTransactionConf>(REMOTE_STOP_TRANSACTION_ACTION, messages_converter),
      GenericMessageHandler<ReserveNowReq, ReserveNowConf>(RESERVE_NOW_ACTION, messages_converter),
      GenericMessageHandler<ResetReq, ResetConf>(RESET_ACTION, messages_converter),
      GenericMessageHandler<SendLocalListReq, SendLocalListConf>(SEND_LOCAL_LIST_ACTION, messages_converter),
      GenericMessageHandler<SetChargingProfileReq, SetChargingProfileConf>(SET_CHARGING_PROFILE_ACTION, messages_converter),
      GenericMessageHandler<TriggerMessageReq, TriggerMessageConf>(TRIGGER_MESSAGE_ACTION, messages_converter),
      GenericMessageHandler<UnlockConnectorReq, UnlockConnectorConf>(UNLOCK_CONNECTOR_ACTION, messages_converter),
      GenericMessageHandler<UpdateFirmwareReq, UpdateFirmwareConf>(UPDATE_FIRMWARE_ACTION, messages_converter),
      // Security extensions
      GenericMessageHandler<CertificateSignedReq, CertificateSignedConf>(CERTIFICATE_SIGNED_ACTION, messages_converter),
      GenericMessageHandler<DeleteCertificateReq, DeleteCertificateConf>(DELETE_CERTIFICATE_ACTION, messages_converter),
      GenericMessageHandler<ExtendedTriggerMessageReq, ExtendedTriggerMessageConf>(EXTENDED_TRIGGER_MESSAGE_ACTION, messages_converter),
      GenericMessageHandler<GetInstalledCertificateIdsReq, GetInstalledCertificateIdsConf>(GET_INSTALLED_CERTIFICATE_IDS_ACTION,
                                                                                           messages_converter),
      GenericMessageHandler<GetLogReq, GetLogConf>(GET_LOG_ACTION, messages_converter),
      GenericMessageHandler<InstallCertificateReq, InstallCertificateConf>(INSTALL_CERTIFICATE_ACTION, messages_converter),
      GenericMessageHandler<SignedUpdateFirmwareReq, SignedUpdateFirmwareConf>(SIGNED_UPDATE_FIRMWARE_ACTION, messages_converter),

      m_identifier(identifier),
      m_charge_point()
{
    msg_dispatcher.registerHandler(CANCEL_RESERVATION_ACTION,
                                   *dynamic_cast<GenericMessageHandler<CancelReservationReq, CancelReservationConf>*>(this));
    msg_dispatcher.registerHandler(CHANGE_AVAILABILITY_ACTION,
                                   *dynamic_cast<GenericMessageHandler<ChangeAvailabilityReq, ChangeAvailabilityConf>*>(this));
    msg_dispatcher.registerHandler(CHANGE_CONFIGURATION_ACTION,
                                   *dynamic_cast<GenericMessageHandler<ChangeConfigurationReq, ChangeConfigurationConf>*>(this));
    msg_dispatcher.registerHandler(CLEAR_CACHE_ACTION, *dynamic_cast<GenericMessageHandler<ClearCacheReq, ClearCacheConf>*>(this));
    msg_dispatcher.registerHandler(CLEAR_CHARGING_PROFILE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<ClearChargingProfileReq, ClearChargingProfileConf>*>(this));
    msg_dispatcher.registerHandler(DATA_TRANSFER_ACTION, *dynamic_cast<GenericMessageHandler<DataTransferReq, DataTransferConf>*>(this));
    msg_dispatcher.registerHandler(GET_COMPOSITE_SCHEDULE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<GetCompositeScheduleReq, GetCompositeScheduleConf>*>(this));
    msg_dispatcher.registerHandler(GET_CONFIGURATION_ACTION,
                                   *dynamic_cast<GenericMessageHandler<GetConfigurationReq, GetConfigurationConf>*>(this));
    msg_dispatcher.registerHandler(GET_DIAGNOSTICS_ACTION,
                                   *dynamic_cast<GenericMessageHandler<GetDiagnosticsReq, GetDiagnosticsConf>*>(this));
    msg_dispatcher.registerHandler(GET_LOCAL_LIST_VERSION_ACTION,
                                   *dynamic_cast<GenericMessageHandler<GetLocalListVersionReq, GetLocalListVersionConf>*>(this));
    msg_dispatcher.registerHandler(REMOTE_START_TRANSACTION_ACTION,
                                   *dynamic_cast<GenericMessageHandler<RemoteStartTransactionReq, RemoteStartTransactionConf>*>(this));
    msg_dispatcher.registerHandler(REMOTE_STOP_TRANSACTION_ACTION,
                                   *dynamic_cast<GenericMessageHandler<RemoteStopTransactionReq, RemoteStopTransactionConf>*>(this));
    msg_dispatcher.registerHandler(RESERVE_NOW_ACTION, *dynamic_cast<GenericMessageHandler<ReserveNowReq, ReserveNowConf>*>(this));
    msg_dispatcher.registerHandler(RESET_ACTION, *dynamic_cast<GenericMessageHandler<ResetReq, ResetConf>*>(this));
    msg_dispatcher.registerHandler(SEND_LOCAL_LIST_ACTION,
                                   *dynamic_cast<GenericMessageHandler<SendLocalListReq, SendLocalListConf>*>(this));
    msg_dispatcher.registerHandler(SET_CHARGING_PROFILE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<SetChargingProfileReq, SetChargingProfileConf>*>(this));
    msg_dispatcher.registerHandler(TRIGGER_MESSAGE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<TriggerMessageReq, TriggerMessageConf>*>(this));
    msg_dispatcher.registerHandler(UNLOCK_CONNECTOR_ACTION,
                                   *dynamic_cast<GenericMessageHandler<UnlockConnectorReq, UnlockConnectorConf>*>(this));
    msg_dispatcher.registerHandler(UPDATE_FIRMWARE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<UpdateFirmwareReq, UpdateFirmwareConf>*>(this));
    // Security extensions
    msg_dispatcher.registerHandler(CERTIFICATE_SIGNED_ACTION,
                                   *dynamic_cast<GenericMessageHandler<CertificateSignedReq, CertificateSignedConf>*>(this));
    msg_dispatcher.registerHandler(DELETE_CERTIFICATE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<DeleteCertificateReq, DeleteCertificateConf>*>(this));
    msg_dispatcher.registerHandler(EXTENDED_TRIGGER_MESSAGE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<ExtendedTriggerMessageReq, ExtendedTriggerMessageConf>*>(this));
    msg_dispatcher.registerHandler(
        GET_INSTALLED_CERTIFICATE_IDS_ACTION,
        *dynamic_cast<GenericMessageHandler<GetInstalledCertificateIdsReq, GetInstalledCertificateIdsConf>*>(this));
    msg_dispatcher.registerHandler(GET_LOG_ACTION, *dynamic_cast<GenericMessageHandler<GetLogReq, GetLogConf>*>(this));
    msg_dispatcher.registerHandler(INSTALL_CERTIFICATE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<InstallCertificateReq, InstallCertificateConf>*>(this));
    msg_dispatcher.registerHandler(SIGNED_UPDATE_FIRMWARE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<SignedUpdateFirmwareReq, SignedUpdateFirmwareConf>*>(this));
}

/** @brief Destructor */
CentralSystemHandler::~CentralSystemHandler() { }

// OCPP handlers

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::CancelReservationReq& request,
                                         ocpp::messages::ocpp16::CancelReservationConf&      response,
                                         std::string&                                        error_code,
                                         std::string&                                        error_message)
{
    return forward(CANCEL_RESERVATION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::ChangeAvailabilityReq& request,
                                         ocpp::messages::ocpp16::ChangeAvailabilityConf&      response,
                                         std::string&                                         error_code,
                                         std::string&                                         error_message)
{
    return forward(CHANGE_AVAILABILITY_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::ChangeConfigurationReq& request,
                                         ocpp::messages::ocpp16::ChangeConfigurationConf&      response,
                                         std::string&                                          error_code,
                                         std::string&                                          error_message)
{
    return forward(CHANGE_CONFIGURATION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::ClearCacheReq& request,
                                         ocpp::messages::ocpp16::ClearCacheConf&      response,
                                         std::string&                                 error_code,
                                         std::string&                                 error_message)
{
    return forward(CLEAR_CACHE_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::ClearChargingProfileReq& request,
                                         ocpp::messages::ocpp16::ClearChargingProfileConf&      response,
                                         std::string&                                           error_code,
                                         std::string&                                           error_message)
{
    return forward(CLEAR_CHARGING_PROFILE_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::DataTransferReq& request,
                                         ocpp::messages::ocpp16::DataTransferConf&      response,
                                         std::string&                                   error_code,
                                         std::string&                                   error_message)
{
    return forward(DATA_TRANSFER_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::GetCompositeScheduleReq& request,
                                         ocpp::messages::ocpp16::GetCompositeScheduleConf&      response,
                                         std::string&                                           error_code,
                                         std::string&                                           error_message)
{
    return forward(GET_COMPOSITE_SCHEDULE_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::GetConfigurationReq& request,
                                         ocpp::messages::ocpp16::GetConfigurationConf&      response,
                                         std::string&                                       error_code,
                                         std::string&                                       error_message)
{
    return forward(GET_CONFIGURATION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::GetDiagnosticsReq& request,
                                         ocpp::messages::ocpp16::GetDiagnosticsConf&      response,
                                         std::string&                                     error_code,
                                         std::string&                                     error_message)
{
    return forward(GET_DIAGNOSTICS_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::GetLocalListVersionReq& request,
                                         ocpp::messages::ocpp16::GetLocalListVersionConf&      response,
                                         std::string&                                          error_code,
                                         std::string&                                          error_message)
{
    return forward(GET_LOCAL_LIST_VERSION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::RemoteStartTransactionReq& request,
                                         ocpp::messages::ocpp16::RemoteStartTransactionConf&      response,
                                         std::string&                                             error_code,
                                         std::string&                                             error_message)
{
    return forward(REMOTE_START_TRANSACTION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::RemoteStopTransactionReq& request,
                                         ocpp::messages::ocpp16::RemoteStopTransactionConf&      response,
                                         std::string&                                            error_code,
                                         std::string&                                            error_message)
{
    return forward(REMOTE_STOP_TRANSACTION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::ReserveNowReq& request,
                                         ocpp::messages::ocpp16::ReserveNowConf&      response,
                                         std::string&                                 error_code,
                                         std::string&                                 error_message)
{
    return forward(RESERVE_NOW_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::ResetReq& request,
                                         ocpp::messages::ocpp16::ResetConf&      response,
                                         std::string&                            error_code,
                                         std::string&                            error_message)
{
    return forward(RESET_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::SendLocalListReq& request,
                                         ocpp::messages::ocpp16::SendLocalListConf&      response,
                                         std::string&                                    error_code,
                                         std::string&                                    error_message)
{
    return forward(SEND_LOCAL_LIST_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::SetChargingProfileReq& request,
                                         ocpp::messages::ocpp16::SetChargingProfileConf&      response,
                                         std::string&                                         error_code,
                                         std::string&                                         error_message)
{
    return forward(SET_CHARGING_PROFILE_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::TriggerMessageReq& request,
                                         ocpp::messages::ocpp16::TriggerMessageConf&      response,
                                         std::string&                                     error_code,
                                         std::string&                                     error_message)
{
    return forward(TRIGGER_MESSAGE_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::UnlockConnectorReq& request,
                                         ocpp::messages::ocpp16::UnlockConnectorConf&      response,
                                         std::string&                                      error_code,
                                         std::string&                                      error_message)
{
    return forward(UNLOCK_CONNECTOR_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::UpdateFirmwareReq& request,
                                         ocpp::messages::ocpp16::UpdateFirmwareConf&      response,
                                         std::string&                                     error_code,
                                         std::string&                                     error_message)
{
    return forward(UPDATE_FIRMWARE_ACTION, request, response, error_code, error_message);
}

// Security extensions

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::CertificateSignedReq& request,
                                         ocpp::messages::ocpp16::CertificateSignedConf&      response,
                                         std::string&                                        error_code,
                                         std::string&                                        error_message)
{
    return forward(CERTIFICATE_SIGNED_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::DeleteCertificateReq& request,
                                         ocpp::messages::ocpp16::DeleteCertificateConf&      response,
                                         std::string&                                        error_code,
                                         std::string&                                        error_message)
{
    return forward(DELETE_CERTIFICATE_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::ExtendedTriggerMessageReq& request,
                                         ocpp::messages::ocpp16::ExtendedTriggerMessageConf&      response,
                                         std::string&                                             error_code,
                                         std::string&                                             error_message)
{
    return forward(EXTENDED_TRIGGER_MESSAGE_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::GetInstalledCertificateIdsReq& request,
                                         ocpp::messages::ocpp16::GetInstalledCertificateIdsConf&      response,
                                         std::string&                                                 error_code,
                                         std::string&                                                 error_message)
{
    return forward(GET_INSTALLED_CERTIFICATE_IDS_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::GetLogReq& request,
                                         ocpp::messages::ocpp16::GetLogConf&      response,
                                         std::string&                             error_code,
                                         std::string&                             error_message)
{
    return forward(GET_LOG_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::InstallCertificateReq& request,
                                         ocpp::messages::ocpp16::InstallCertificateConf&      response,
                                         std::string&                                         error_code,
                                         std::string&                                         error_message)
{
    return forward(INSTALL_CERTIFICATE_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ocpp16::SignedUpdateFirmwareReq& request,
                                         ocpp::messages::ocpp16::SignedUpdateFirmwareConf&      response,
                                         std::string&                                           error_code,
                                         std::string&                                           error_message)
{
    return forward(SIGNED_UPDATE_FIRMWARE_ACTION, request, response, error_code, error_message);
}

} // namespace localcontroller
} // namespace ocpp
