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

#include "ChargePointHandler.h"
#include "ICentralSystemConfig.h"
#include "IChargePointRequestHandler.h"
#include "IRpc.h"
#include "Logger.h"
#include "MessageDispatcher.h"
#include "MessagesConverter.h"

using namespace ocpp::messages;
using namespace ocpp::types;

namespace ocpp
{
namespace centralsystem
{

/** @brief Constructor */
ChargePointHandler::ChargePointHandler(const std::string&                        identifier,
                                       const ocpp::messages::MessagesConverter&  messages_converter,
                                       ocpp::messages::MessageDispatcher&        msg_dispatcher,
                                       const ocpp::config::ICentralSystemConfig& stack_config)
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
      GenericMessageHandler<SecurityEventNotificationReq, SecurityEventNotificationConf>(SECURITY_EVENT_NOTIFICATION_ACTION,
                                                                                         messages_converter),
      m_identifier(identifier),
      m_stack_config(stack_config),
      m_handler(nullptr)
{
    msg_dispatcher.registerHandler(AUTHORIZE_ACTION, *dynamic_cast<GenericMessageHandler<AuthorizeReq, AuthorizeConf>*>(this));
    msg_dispatcher.registerHandler(BOOT_NOTIFICATION_ACTION,
                                   *dynamic_cast<GenericMessageHandler<BootNotificationReq, BootNotificationConf>*>(this));
    msg_dispatcher.registerHandler(DATA_TRANSFER_ACTION, *dynamic_cast<GenericMessageHandler<DataTransferReq, DataTransferConf>*>(this));
    msg_dispatcher.registerHandler(
        DIAGNOSTIC_STATUS_NOTIFICATION_ACTION,
        *dynamic_cast<GenericMessageHandler<DiagnosticsStatusNotificationReq, DiagnosticsStatusNotificationConf>*>(this));
    msg_dispatcher.registerHandler(
        FIRMWARE_STATUS_NOTIFICATION_ACTION,
        *dynamic_cast<GenericMessageHandler<FirmwareStatusNotificationReq, FirmwareStatusNotificationConf>*>(this));
    msg_dispatcher.registerHandler(HEARTBEAT_ACTION, *dynamic_cast<GenericMessageHandler<HeartbeatReq, HeartbeatConf>*>(this));
    msg_dispatcher.registerHandler(METER_VALUES_ACTION, *dynamic_cast<GenericMessageHandler<MeterValuesReq, MeterValuesConf>*>(this));
    msg_dispatcher.registerHandler(START_TRANSACTION_ACTION,
                                   *dynamic_cast<GenericMessageHandler<StartTransactionReq, StartTransactionConf>*>(this));
    msg_dispatcher.registerHandler(STATUS_NOTIFICATION_ACTION,
                                   *dynamic_cast<GenericMessageHandler<StatusNotificationReq, StatusNotificationConf>*>(this));
    msg_dispatcher.registerHandler(STOP_TRANSACTION_ACTION,
                                   *dynamic_cast<GenericMessageHandler<StopTransactionReq, StopTransactionConf>*>(this));
    msg_dispatcher.registerHandler(
        SECURITY_EVENT_NOTIFICATION_ACTION,
        *dynamic_cast<GenericMessageHandler<SecurityEventNotificationReq, SecurityEventNotificationConf>*>(this));
}
/** @brief Destructor */
ChargePointHandler::~ChargePointHandler() { }

// OCPP handlers

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::AuthorizeReq& request,
                                       ocpp::messages::AuthorizeConf&      response,
                                       const char*&                        error_code,
                                       std::string&                        error_message)
{
    bool ret = false;
    (void)error_message;

    LOG_INFO << "[" << m_identifier << "] - Authorize requested : idTag = " << request.idTag.str();

    // Notify request
    if (m_handler)
    {
        response.idTagInfo = m_handler->authorize(request.idTag.str());

        LOG_INFO << "[" << m_identifier << "] - Authorize status : " << AuthorizationStatusHelper.toString(response.idTagInfo.status);
        ret = true;
    }
    else
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_INTERNAL;
    }

    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::BootNotificationReq& request,
                                       ocpp::messages::BootNotificationConf&      response,
                                       const char*&                               error_code,
                                       std::string&                               error_message)
{
    bool ret = false;
    (void)error_message;

    LOG_INFO << "[" << m_identifier << "] - Boot notification received : chargePointVendor = " << request.chargePointVendor.str()
             << " - chargePointModel = " << request.chargePointModel.str() << " - chargePointSerialNumber = "
             << (request.chargePointSerialNumber.isSet() ? request.chargePointSerialNumber.value().str() : "not set");

    // Notify request
    if (m_handler)
    {
        response.status = m_handler->bootNotification(request.chargePointModel,
                                                      request.chargePointSerialNumber.value(),
                                                      request.chargePointVendor,
                                                      request.firmwareVersion.value(),
                                                      request.iccid.value(),
                                                      request.imsi.value(),
                                                      request.meterSerialNumber.value(),
                                                      request.meterType.value());
        if (response.status == RegistrationStatus::Accepted)
        {
            response.interval = m_stack_config.heartbeatInterval().count();
        }
        else
        {
            response.interval = m_stack_config.bootNotificationRetryInterval().count();
        }
        response.currentTime = DateTime::now();

        LOG_INFO << "[" << m_identifier << "] - Boot notification status : " << RegistrationStatusHelper.toString(response.status);
        ret = true;
    }
    else
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_INTERNAL;
    }

    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::DataTransferReq& request,
                                       ocpp::messages::DataTransferConf&      response,
                                       const char*&                           error_code,
                                       std::string&                           error_message)
{
    bool ret = false;
    (void)error_message;

    LOG_INFO << "[" << m_identifier << "] - Data transfer requested : vendorId = " << request.vendorId.str()
             << " - messageId = " << (request.messageId.isSet() ? request.messageId.value().str() : "not set")
             << " - data = " << (request.data.isSet() ? request.data.value() : "not set");

    // Notify request
    if (m_handler)
    {
        response.status = m_handler->dataTransfer(request.vendorId, request.messageId.value(), request.data.value(), response.data.value());
        if (response.data.value().empty())
        {
            response.data.clear();
        }

        LOG_INFO << "[" << m_identifier << "] - Data transfer : status = " << DataTransferStatusHelper.toString(response.status)
                 << " - data = " << (request.data.isSet() ? request.data.value() : "not set");
        ret = true;
    }
    else
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_INTERNAL;
    }

    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::DiagnosticsStatusNotificationReq& request,
                                       ocpp::messages::DiagnosticsStatusNotificationConf&      response,
                                       const char*&                                            error_code,
                                       std::string&                                            error_message)
{
    bool ret = false;
    (void)error_message;

    LOG_INFO << "[" << m_identifier
             << "] - Diagnostic status notification received : status = " << DiagnosticsStatusHelper.toString(request.status);

    // Notify request
    if (m_handler)
    {
        m_handler->diagnosticStatusNotification(request.status);

        // Empty response
        (void)response;
        ret = true;
    }
    else
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_INTERNAL;
    }

    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::FirmwareStatusNotificationReq& request,
                                       ocpp::messages::FirmwareStatusNotificationConf&      response,
                                       const char*&                                         error_code,
                                       std::string&                                         error_message)
{
    bool ret = false;
    (void)error_message;

    LOG_INFO << "[" << m_identifier
             << "] - Firmware status notification received : status = " << FirmwareStatusHelper.toString(request.status);

    // Notify request
    if (m_handler)
    {
        m_handler->firmwareStatusNotification(request.status);

        // Empty response
        (void)response;
        ret = true;
    }
    else
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_INTERNAL;
    }

    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::HeartbeatReq& request,
                                       ocpp::messages::HeartbeatConf&      response,
                                       const char*&                        error_code,
                                       std::string&                        error_message)
{
    (void)error_code;
    (void)error_message;

    LOG_INFO << "[" << m_identifier << "] - Heartbeat received";

    // Empty request
    (void)request;

    // Prepare response
    response.currentTime = DateTime::now();

    return true;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::MeterValuesReq& request,
                                       ocpp::messages::MeterValuesConf&      response,
                                       const char*&                          error_code,
                                       std::string&                          error_message)
{
    bool ret = false;
    (void)error_message;

    LOG_INFO << "[" << m_identifier << "] - Meter values received : connectorId = " << request.connectorId
             << " - transactionId = " << (request.transactionId.isSet() ? std::to_string(request.transactionId) : "not set")
             << " - meterValue count = " << request.meterValue.size();

    // Notify request
    if (m_handler)
    {
        m_handler->meterValues(request.connectorId, request.transactionId, request.meterValue);

        // Empty response
        (void)response;
        ret = true;
    }
    else
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_INTERNAL;
    }

    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::StartTransactionReq& request,
                                       ocpp::messages::StartTransactionConf&      response,
                                       const char*&                               error_code,
                                       std::string&                               error_message)
{
    bool ret = false;
    (void)error_message;

    LOG_INFO << "[" << m_identifier << "] - Start transaction requested : connectorId = " << request.connectorId
             << " - idTag = " << request.idTag.str() << " - timestamp = " << request.timestamp.str()
             << " - meterStart = " << request.meterStart
             << " - reservationId = " << (request.reservationId.isSet() ? std::to_string(request.reservationId) : "not set");

    // Notify request
    if (m_handler)
    {
        response.idTagInfo = m_handler->startTransaction(
            request.connectorId, request.idTag.str(), request.meterStart, request.reservationId, request.timestamp, response.transactionId);

        LOG_INFO << "[" << m_identifier
                 << "] - Start transaction : status = " << AuthorizationStatusHelper.toString(response.idTagInfo.status)
                 << " - transactionId = " << response.transactionId;
        ret = true;
    }
    else
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_INTERNAL;
    }

    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::StatusNotificationReq& request,
                                       ocpp::messages::StatusNotificationConf&      response,
                                       const char*&                                 error_code,
                                       std::string&                                 error_message)
{
    bool ret = false;
    (void)error_message;

    LOG_INFO << "[" << m_identifier << "] - Status notification received : connectorId = " << request.connectorId
             << " -  status = " << ChargePointStatusHelper.toString(request.status)
             << " - errorCode = " << ChargePointErrorCodeHelper.toString(request.errorCode);

    // Notify request
    if (m_handler)
    {
        m_handler->statusNotification(request.connectorId,
                                      request.errorCode,
                                      request.info.value(),
                                      request.status,
                                      request.timestamp.value(),
                                      request.vendorId.value(),
                                      request.vendorErrorCode.value());

        // Empty response
        (void)response;
        ret = true;
    }
    else
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_INTERNAL;
    }

    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::StopTransactionReq& request,
                                       ocpp::messages::StopTransactionConf&      response,
                                       const char*&                              error_code,
                                       std::string&                              error_message)
{
    bool ret = false;
    (void)error_message;

    LOG_INFO << "[" << m_identifier << "] - Stop transaction requested : transactionId = " << request.transactionId
             << " - idTag = " << (request.idTag.isSet() ? request.idTag.value().str() : "not set")
             << " - timestamp = " << request.timestamp.str() << " - meterStop = " << request.meterStop
             << " - reason = " << ReasonHelper.toString(request.reason) << " - transactionData count = " << request.transactionData.size();

    // Notify request
    if (m_handler)
    {
        response.idTagInfo = m_handler->stopTransaction(request.idTag.value().str(),
                                                        request.meterStop,
                                                        request.timestamp,
                                                        request.transactionId,
                                                        request.reason,
                                                        request.transactionData);

        LOG_INFO << "[" << m_identifier << "] - Stop transaction : status = "
                 << (response.idTagInfo.isSet() ? AuthorizationStatusHelper.toString(response.idTagInfo.value().status) : "not set");
        ret = true;
    }
    else
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_INTERNAL;
    }

    return ret;
}

// Security extensions

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::SecurityEventNotificationReq& request,
                                       ocpp::messages::SecurityEventNotificationConf&      response,
                                       const char*&                                        error_code,
                                       std::string&                                        error_message)
{
    bool ret = false;
    (void)error_message;

    LOG_INFO << "[" << m_identifier << "] - Security event notification received : type = " << request.type.str()
             << " -  timestamp = " << request.timestamp.str()
             << " - techInfo = " << (request.techInfo.isSet() ? request.techInfo.value().str() : "not set");

    // Notify request
    if (m_handler)
    {
        m_handler->securityEventNotification(request.type, request.timestamp, request.techInfo.value());

        // Empty response
        (void)response;
        ret = true;
    }
    else
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_INTERNAL;
    }

    return ret;
}

} // namespace centralsystem
} // namespace ocpp
