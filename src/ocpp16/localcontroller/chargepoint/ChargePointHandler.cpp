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

#include "ChargePointHandler.h"
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
ChargePointHandler::ChargePointHandler(const std::string&                       identifier,
                                       const ocpp::messages::MessagesConverter& messages_converter,
                                       ocpp::messages::MessageDispatcher&       msg_dispatcher,
                                       ICentralSystemProxy&                     central_system)
    : GenericMessageHandler<AuthorizeReq, AuthorizeConf>(AUTHORIZE_ACTION, messages_converter),
      GenericMessageHandler<BootNotificationReq, BootNotificationConf>(BOOT_NOTIFICATION_ACTION, messages_converter),
      GenericMessageHandler<DataTransferReq, DataTransferConf>(DATA_TRANSFER_ACTION, messages_converter),
      GenericMessageHandler<DiagnosticsStatusNotificationReq, DiagnosticsStatusNotificationConf>(DIAGNOSTIC_STATUS_NOTIFICATION_ACTION,
                                                                                                 messages_converter),
      GenericMessageHandler<FirmwareStatusNotificationReq, FirmwareStatusNotificationConf>(FIRMWARE_STATUS_NOTIFICATION_ACTION,
                                                                                           messages_converter),
      GenericMessageHandler<HeartbeatReq, HeartbeatConf>(HEARTBEAT_ACTION, messages_converter),
      GenericMessageHandler<MeterValuesReq, MeterValuesConf>(METER_VALUES_ACTION, messages_converter),
      GenericMessageHandler<StartTransactionReq, StartTransactionConf>(START_TRANSACTION_ACTION, messages_converter),
      GenericMessageHandler<StatusNotificationReq, StatusNotificationConf>(STATUS_NOTIFICATION_ACTION, messages_converter),
      GenericMessageHandler<StopTransactionReq, StopTransactionConf>(STOP_TRANSACTION_ACTION, messages_converter),
      // Security extensions
      GenericMessageHandler<LogStatusNotificationReq, LogStatusNotificationConf>(LOG_STATUS_NOTIFICATION_ACTION, messages_converter),
      GenericMessageHandler<SecurityEventNotificationReq, SecurityEventNotificationConf>(SECURITY_EVENT_NOTIFICATION_ACTION,
                                                                                         messages_converter),
      GenericMessageHandler<SignCertificateReq, SignCertificateConf>(SIGN_CERTIFICATE_ACTION, messages_converter),
      GenericMessageHandler<SignedFirmwareStatusNotificationReq, SignedFirmwareStatusNotificationConf>(
          SIGNED_FIRMWARE_STATUS_NOTIFICATION_ACTION, messages_converter),
      m_identifier(identifier),
      m_central_system(central_system)
{
    msg_dispatcher.registerHandler(AUTHORIZE_ACTION, *dynamic_cast<GenericMessageHandler<AuthorizeReq, AuthorizeConf>*>(this), true);
    msg_dispatcher.registerHandler(
        BOOT_NOTIFICATION_ACTION, *dynamic_cast<GenericMessageHandler<BootNotificationReq, BootNotificationConf>*>(this), true);
    msg_dispatcher.registerHandler(
        DATA_TRANSFER_ACTION, *dynamic_cast<GenericMessageHandler<DataTransferReq, DataTransferConf>*>(this), true);
    msg_dispatcher.registerHandler(
        DIAGNOSTIC_STATUS_NOTIFICATION_ACTION,
        *dynamic_cast<GenericMessageHandler<DiagnosticsStatusNotificationReq, DiagnosticsStatusNotificationConf>*>(this),
        true);
    msg_dispatcher.registerHandler(
        FIRMWARE_STATUS_NOTIFICATION_ACTION,
        *dynamic_cast<GenericMessageHandler<FirmwareStatusNotificationReq, FirmwareStatusNotificationConf>*>(this),
        true);
    msg_dispatcher.registerHandler(HEARTBEAT_ACTION, *dynamic_cast<GenericMessageHandler<HeartbeatReq, HeartbeatConf>*>(this), true);
    msg_dispatcher.registerHandler(METER_VALUES_ACTION, *dynamic_cast<GenericMessageHandler<MeterValuesReq, MeterValuesConf>*>(this), true);
    msg_dispatcher.registerHandler(
        START_TRANSACTION_ACTION, *dynamic_cast<GenericMessageHandler<StartTransactionReq, StartTransactionConf>*>(this), true);
    msg_dispatcher.registerHandler(
        STATUS_NOTIFICATION_ACTION, *dynamic_cast<GenericMessageHandler<StatusNotificationReq, StatusNotificationConf>*>(this), true);
    msg_dispatcher.registerHandler(
        STOP_TRANSACTION_ACTION, *dynamic_cast<GenericMessageHandler<StopTransactionReq, StopTransactionConf>*>(this), true);
    msg_dispatcher.registerHandler(LOG_STATUS_NOTIFICATION_ACTION,
                                   *dynamic_cast<GenericMessageHandler<LogStatusNotificationReq, LogStatusNotificationConf>*>(this),
                                   true);
    msg_dispatcher.registerHandler(SECURITY_EVENT_NOTIFICATION_ACTION,
                                   *dynamic_cast<GenericMessageHandler<SecurityEventNotificationReq, SecurityEventNotificationConf>*>(this),
                                   true);
    msg_dispatcher.registerHandler(
        SIGN_CERTIFICATE_ACTION, *dynamic_cast<GenericMessageHandler<SignCertificateReq, SignCertificateConf>*>(this), true);

    msg_dispatcher.registerHandler(
        SIGNED_FIRMWARE_STATUS_NOTIFICATION_ACTION,
        *dynamic_cast<GenericMessageHandler<SignedFirmwareStatusNotificationReq, SignedFirmwareStatusNotificationConf>*>(this),
        true);
}
/** @brief Destructor */
ChargePointHandler::~ChargePointHandler() { }

// OCPP handlers

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::AuthorizeReq& request,
                                       ocpp::messages::AuthorizeConf&      response,
                                       std::string&                        error_code,
                                       std::string&                        error_message)
{
    return forward(AUTHORIZE_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::BootNotificationReq& request,
                                       ocpp::messages::BootNotificationConf&      response,
                                       std::string&                               error_code,
                                       std::string&                               error_message)
{
    return forward(BOOT_NOTIFICATION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::DataTransferReq& request,
                                       ocpp::messages::DataTransferConf&      response,
                                       std::string&                           error_code,
                                       std::string&                           error_message)
{
    return forward(DATA_TRANSFER_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::DiagnosticsStatusNotificationReq& request,
                                       ocpp::messages::DiagnosticsStatusNotificationConf&      response,
                                       std::string&                                            error_code,
                                       std::string&                                            error_message)
{
    return forward(DIAGNOSTIC_STATUS_NOTIFICATION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::FirmwareStatusNotificationReq& request,
                                       ocpp::messages::FirmwareStatusNotificationConf&      response,
                                       std::string&                                         error_code,
                                       std::string&                                         error_message)
{
    return forward(FIRMWARE_STATUS_NOTIFICATION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::HeartbeatReq& request,
                                       ocpp::messages::HeartbeatConf&      response,
                                       std::string&                        error_code,
                                       std::string&                        error_message)
{
    return forward(HEARTBEAT_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::MeterValuesReq& request,
                                       ocpp::messages::MeterValuesConf&      response,
                                       std::string&                          error_code,
                                       std::string&                          error_message)
{
    return forward(METER_VALUES_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::StartTransactionReq& request,
                                       ocpp::messages::StartTransactionConf&      response,
                                       std::string&                               error_code,
                                       std::string&                               error_message)
{
    return forward(START_TRANSACTION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::StatusNotificationReq& request,
                                       ocpp::messages::StatusNotificationConf&      response,
                                       std::string&                                 error_code,
                                       std::string&                                 error_message)
{
    return forward(STATUS_NOTIFICATION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::StopTransactionReq& request,
                                       ocpp::messages::StopTransactionConf&      response,
                                       std::string&                              error_code,
                                       std::string&                              error_message)
{
    return forward(STOP_TRANSACTION_ACTION, request, response, error_code, error_message);
}

// Security extensions

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::LogStatusNotificationReq& request,
                                       ocpp::messages::LogStatusNotificationConf&      response,
                                       std::string&                                    error_code,
                                       std::string&                                    error_message)
{
    return forward(LOG_STATUS_NOTIFICATION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::SecurityEventNotificationReq& request,
                                       ocpp::messages::SecurityEventNotificationConf&      response,
                                       std::string&                                        error_code,
                                       std::string&                                        error_message)
{
    return forward(SECURITY_EVENT_NOTIFICATION_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::SignCertificateReq& request,
                                       ocpp::messages::SignCertificateConf&      response,
                                       std::string&                              error_code,
                                       std::string&                              error_message)
{
    return forward(SIGN_CERTIFICATE_ACTION, request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::SignedFirmwareStatusNotificationReq& request,
                                       ocpp::messages::SignedFirmwareStatusNotificationConf&      response,
                                       std::string&                                               error_code,
                                       std::string&                                               error_message)
{
    return forward(SIGNED_FIRMWARE_STATUS_NOTIFICATION_ACTION, request, response, error_code, error_message);
}

} // namespace localcontroller
} // namespace ocpp
