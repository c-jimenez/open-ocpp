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

#include "CentralSystemHandler.h"
#include "ILocalControllerConfig.h"
#include "MessageDispatcher.h"
#include "MessagesConverter.h"

using namespace ocpp::messages;
using namespace ocpp::types;

namespace ocpp
{

namespace localcontroller
{

/** @brief Constructor */
CentralSystemHandler::CentralSystemHandler(const std::string&                       identifier,
                                           const ocpp::messages::MessagesConverter& messages_converter,
                                           ocpp::messages::MessageDispatcher&       msg_dispatcher)
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
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::CancelReservationReq& request,
                                         ocpp::messages::CancelReservationConf&      response,
                                         const char*&                                error_code,
                                         std::string&                                error_message)
{
    return forward(CANCEL_RESERVATION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ChangeAvailabilityReq& request,
                                         ocpp::messages::ChangeAvailabilityConf&      response,
                                         const char*&                                 error_code,
                                         std::string&                                 error_message)
{
    return forward(CHANGE_AVAILABILITY_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ChangeConfigurationReq& request,
                                         ocpp::messages::ChangeConfigurationConf&      response,
                                         const char*&                                  error_code,
                                         std::string&                                  error_message)
{
    return forward(CHANGE_CONFIGURATION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ClearCacheReq& request,
                                         ocpp::messages::ClearCacheConf&      response,
                                         const char*&                         error_code,
                                         std::string&                         error_message)
{
    return forward(CLEAR_CACHE_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ClearChargingProfileReq& request,
                                         ocpp::messages::ClearChargingProfileConf&      response,
                                         const char*&                                   error_code,
                                         std::string&                                   error_message)
{
    return forward(CLEAR_CHARGING_PROFILE_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::DataTransferReq& request,
                                         ocpp::messages::DataTransferConf&      response,
                                         const char*&                           error_code,
                                         std::string&                           error_message)
{
    return forward(DATA_TRANSFER_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::GetCompositeScheduleReq& request,
                                         ocpp::messages::GetCompositeScheduleConf&      response,
                                         const char*&                                   error_code,
                                         std::string&                                   error_message)
{
    return forward(GET_COMPOSITE_SCHEDULE_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::GetConfigurationReq& request,
                                         ocpp::messages::GetConfigurationConf&      response,
                                         const char*&                               error_code,
                                         std::string&                               error_message)
{
    return forward(GET_CONFIGURATION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::GetDiagnosticsReq& request,
                                         ocpp::messages::GetDiagnosticsConf&      response,
                                         const char*&                             error_code,
                                         std::string&                             error_message)
{
    return forward(GET_DIAGNOSTICS_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::GetLocalListVersionReq& request,
                                         ocpp::messages::GetLocalListVersionConf&      response,
                                         const char*&                                  error_code,
                                         std::string&                                  error_message)
{
    return forward(GET_LOCAL_LIST_VERSION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::RemoteStartTransactionReq& request,
                                         ocpp::messages::RemoteStartTransactionConf&      response,
                                         const char*&                                     error_code,
                                         std::string&                                     error_message)
{
    return forward(REMOTE_START_TRANSACTION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::RemoteStopTransactionReq& request,
                                         ocpp::messages::RemoteStopTransactionConf&      response,
                                         const char*&                                    error_code,
                                         std::string&                                    error_message)
{
    return forward(REMOTE_STOP_TRANSACTION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ReserveNowReq& request,
                                         ocpp::messages::ReserveNowConf&      response,
                                         const char*&                         error_code,
                                         std::string&                         error_message)
{
    return forward(RESERVE_NOW_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ResetReq& request,
                                         ocpp::messages::ResetConf&      response,
                                         const char*&                    error_code,
                                         std::string&                    error_message)
{
    return forward(RESET_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::SendLocalListReq& request,
                                         ocpp::messages::SendLocalListConf&      response,
                                         const char*&                            error_code,
                                         std::string&                            error_message)
{
    return forward(SEND_LOCAL_LIST_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::SetChargingProfileReq& request,
                                         ocpp::messages::SetChargingProfileConf&      response,
                                         const char*&                                 error_code,
                                         std::string&                                 error_message)
{
    return forward(SET_CHARGING_PROFILE_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::TriggerMessageReq& request,
                                         ocpp::messages::TriggerMessageConf&      response,
                                         const char*&                             error_code,
                                         std::string&                             error_message)
{
    return forward(TRIGGER_MESSAGE_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::UnlockConnectorReq& request,
                                         ocpp::messages::UnlockConnectorConf&      response,
                                         const char*&                              error_code,
                                         std::string&                              error_message)
{
    return forward(UNLOCK_CONNECTOR_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::UpdateFirmwareReq& request,
                                         ocpp::messages::UpdateFirmwareConf&      response,
                                         const char*&                             error_code,
                                         std::string&                             error_message)
{
    return forward(UPDATE_FIRMWARE_ACTION, request, response, error_code, error_message);
}

// Security extensions

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::CertificateSignedReq& request,
                                         ocpp::messages::CertificateSignedConf&      response,
                                         const char*&                                error_code,
                                         std::string&                                error_message)
{
    return forward(CERTIFICATE_SIGNED_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::DeleteCertificateReq& request,
                                         ocpp::messages::DeleteCertificateConf&      response,
                                         const char*&                                error_code,
                                         std::string&                                error_message)
{
    return forward(DELETE_CERTIFICATE_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::ExtendedTriggerMessageReq& request,
                                         ocpp::messages::ExtendedTriggerMessageConf&      response,
                                         const char*&                                     error_code,
                                         std::string&                                     error_message)
{
    return forward(EXTENDED_TRIGGER_MESSAGE_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::GetInstalledCertificateIdsReq& request,
                                         ocpp::messages::GetInstalledCertificateIdsConf&      response,
                                         const char*&                                         error_code,
                                         std::string&                                         error_message)
{
    return forward(GET_INSTALLED_CERTIFICATE_IDS_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::GetLogReq& request,
                                         ocpp::messages::GetLogConf&      response,
                                         const char*&                     error_code,
                                         std::string&                     error_message)
{
    return forward(GET_LOG_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::InstallCertificateReq& request,
                                         ocpp::messages::InstallCertificateConf&      response,
                                         const char*&                                 error_code,
                                         std::string&                                 error_message)
{
    return forward(INSTALL_CERTIFICATE_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool CentralSystemHandler::handleMessage(const ocpp::messages::SignedUpdateFirmwareReq& request,
                                         ocpp::messages::SignedUpdateFirmwareConf&      response,
                                         const char*&                                   error_code,
                                         std::string&                                   error_message)
{
    return forward(SIGNED_UPDATE_FIRMWARE_ACTION, request, response, error_code, error_message);
}

} // namespace localcontroller
} // namespace ocpp
