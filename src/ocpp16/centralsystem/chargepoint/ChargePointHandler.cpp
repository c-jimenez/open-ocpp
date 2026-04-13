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
#include "ICentralSystemConfig.h"
#include "IChargePointRequestHandler.h"
#include "IRpc.h"
#include "Iso15118.h"
#include "MessageDispatcher.h"

using namespace ocpp::messages;
using namespace ocpp::messages::ocpp16;
using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace centralsystem
{

/** @brief Constructor */
ChargePointHandler::ChargePointHandler(const std::string&                               identifier,
                                       const ocpp::messages::ocpp16::MessagesConverter& messages_converter,
                                       ocpp::messages::MessageDispatcher&               msg_dispatcher,
                                       const ocpp::config::ICentralSystemConfig&        stack_config)
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
      GenericMessageHandler<LogStatusNotificationReq, LogStatusNotificationConf>(LOG_STATUS_NOTIFICATION_ACTION, messages_converter),
      GenericMessageHandler<SecurityEventNotificationReq, SecurityEventNotificationConf>(SECURITY_EVENT_NOTIFICATION_ACTION,
                                                                                         messages_converter),
      GenericMessageHandler<SignCertificateReq, SignCertificateConf>(SIGN_CERTIFICATE_ACTION, messages_converter),
      GenericMessageHandler<SignedFirmwareStatusNotificationReq, SignedFirmwareStatusNotificationConf>(
          SIGNED_FIRMWARE_STATUS_NOTIFICATION_ACTION, messages_converter),
      m_identifier(identifier),
      m_stack_config(stack_config),
      m_messages_converter(messages_converter),
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
    msg_dispatcher.registerHandler(LOG_STATUS_NOTIFICATION_ACTION,
                                   *dynamic_cast<GenericMessageHandler<LogStatusNotificationReq, LogStatusNotificationConf>*>(this));
    msg_dispatcher.registerHandler(
        SECURITY_EVENT_NOTIFICATION_ACTION,
        *dynamic_cast<GenericMessageHandler<SecurityEventNotificationReq, SecurityEventNotificationConf>*>(this));
    msg_dispatcher.registerHandler(SIGN_CERTIFICATE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<SignCertificateReq, SignCertificateConf>*>(this));

    msg_dispatcher.registerHandler(
        SIGNED_FIRMWARE_STATUS_NOTIFICATION_ACTION,
        *dynamic_cast<GenericMessageHandler<SignedFirmwareStatusNotificationReq, SignedFirmwareStatusNotificationConf>*>(this));
}
/** @brief Destructor */
ChargePointHandler::~ChargePointHandler() { }

// OCPP handlers

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::ocpp16::AuthorizeReq& request,
                                       ocpp::messages::ocpp16::AuthorizeConf&      response,
                                       std::string&                                error_code,
                                       std::string&                                error_message)
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
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::ocpp16::BootNotificationReq& request,
                                       ocpp::messages::ocpp16::BootNotificationConf&      response,
                                       std::string&                                       error_code,
                                       std::string&                                       error_message)
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
            response.interval = static_cast<unsigned int>(m_stack_config.heartbeatInterval().count());
        }
        else
        {
            response.interval = static_cast<unsigned int>(m_stack_config.bootNotificationRetryInterval().count());
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
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::ocpp16::DataTransferReq& request,
                                       ocpp::messages::ocpp16::DataTransferConf&      response,
                                       std::string&                                   error_code,
                                       std::string&                                   error_message)
{
    bool ret = false;
    (void)error_message;

    // Check if ISO15518 support is enabled
    bool message_handled = false;
    if (m_stack_config.iso15118PnCEnabled() && (request.vendorId == ISO15118_VENDOR_ID) && m_handler)
    {
        // Known messages
        if (request.messageId.value() == ISO15118_AUTHORIZE_ACTION)
        {
            // Iso15118Authorize
            response.status = handleMessage<Iso15118AuthorizeReq, Iso15118AuthorizeConf>(
                "Iso15118Authorize", request.data.value(), response.data.value());
        }
        else if (request.messageId.value() == GET_15118_EV_CERTIFICATE_ACTION)
        {
            // Get15118EVCertificate
            response.status = handleMessage<Get15118EVCertificateReq, Get15118EVCertificateConf>(
                "Get15118EVCertificate", request.data.value(), response.data.value());
        }
        else if (request.messageId.value() == GET_CERTIFICATE_STATUS_ACTION)
        {
            // GetCertificateStatus
            response.status = handleMessage<GetCertificateStatusReq, GetCertificateStatusConf>(
                "GetCertificateStatus", request.data.value(), response.data.value());
        }
        else if (request.messageId.value() == SIGN_CERTIFICATE_ACTION)
        {
            // SignCertificate
            response.status =
                handleMessage<SignCertificateReq, SignCertificateConf>("SignCertificate", request.data.value(), response.data.value());
        }
        else
        {
            // Unknown message
            LOG_ERROR << "[ISO15118] Unknown message : " << request.messageId.value().str();
            response.status = DataTransferStatus::UnknownMessageId;
        }

        message_handled = true;
        ret             = true;
    }

    // Notify message if it hasn't been handled already
    if (!message_handled)
    {
        LOG_INFO << "[" << m_identifier << "] - Data transfer requested : vendorId = " << request.vendorId.str()
                 << " - messageId = " << (request.messageId.isSet() ? request.messageId.value().str() : "not set")
                 << " - data = " << (request.data.isSet() ? request.data.value() : "not set");

        // Notify request
        if (m_handler)
        {
            response.status =
                m_handler->dataTransfer(request.vendorId, request.messageId.value(), request.data.value(), response.data.value());
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
    }

    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::ocpp16::DiagnosticsStatusNotificationReq& request,
                                       ocpp::messages::ocpp16::DiagnosticsStatusNotificationConf&      response,
                                       std::string&                                                    error_code,
                                       std::string&                                                    error_message)
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
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::ocpp16::FirmwareStatusNotificationReq& request,
                                       ocpp::messages::ocpp16::FirmwareStatusNotificationConf&      response,
                                       std::string&                                                 error_code,
                                       std::string&                                                 error_message)
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
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::ocpp16::HeartbeatReq& request,
                                       ocpp::messages::ocpp16::HeartbeatConf&      response,
                                       std::string&                                error_code,
                                       std::string&                                error_message)
{
    (void)error_code;
    (void)error_message;

    LOG_INFO << "[" << m_identifier << "] - Heartbeat received";

    // Empty request
    (void)request;

    // Notify request
    if (m_handler)
    {
        m_handler->heartbeat();
    }

    // Prepare response
    response.currentTime = DateTime::now();

    return true;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::ocpp16::MeterValuesReq& request,
                                       ocpp::messages::ocpp16::MeterValuesConf&      response,
                                       std::string&                                  error_code,
                                       std::string&                                  error_message)
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
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::ocpp16::StartTransactionReq& request,
                                       ocpp::messages::ocpp16::StartTransactionConf&      response,
                                       std::string&                                       error_code,
                                       std::string&                                       error_message)
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
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::ocpp16::StatusNotificationReq& request,
                                       ocpp::messages::ocpp16::StatusNotificationConf&      response,
                                       std::string&                                         error_code,
                                       std::string&                                         error_message)
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
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::ocpp16::StopTransactionReq& request,
                                       ocpp::messages::ocpp16::StopTransactionConf&      response,
                                       std::string&                                      error_code,
                                       std::string&                                      error_message)
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
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::ocpp16::LogStatusNotificationReq& request,
                                       ocpp::messages::ocpp16::LogStatusNotificationConf&      response,
                                       std::string&                                            error_code,
                                       std::string&                                            error_message)
{
    bool ret = false;
    (void)error_message;

    LOG_INFO << "[" << m_identifier
             << "] - Log status notification received : status = " << UploadLogStatusEnumTypeHelper.toString(request.status)
             << " - requestId = " << (request.requestId.isSet() ? std::to_string(request.requestId) : "not set");

    // Notify request
    if (m_handler)
    {
        m_handler->logStatusNotification(request.status, request.requestId);

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
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::ocpp16::SecurityEventNotificationReq& request,
                                       ocpp::messages::ocpp16::SecurityEventNotificationConf&      response,
                                       std::string&                                                error_code,
                                       std::string&                                                error_message)
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

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::ocpp16::SignCertificateReq& request,
                                       ocpp::messages::ocpp16::SignCertificateConf&      response,
                                       std::string&                                      error_code,
                                       std::string&                                      error_message)
{
    bool ret = false;
    (void)error_message;

    LOG_INFO << "[" << m_identifier << "] - Sign certificate requested : csr size = " << request.csr.size();

    if (m_handler)
    {
        // Prepare response
        response.status = GenericStatusEnumType::Rejected;

        // Load certificate request
        ocpp::x509::CertificateRequest certificate_request(request.csr);
        if (certificate_request.isValid())
        {
            // Notify request
            if (m_handler->signCertificate(certificate_request))
            {
                response.status = GenericStatusEnumType::Accepted;
            }
        }

        LOG_INFO << "[" << m_identifier << "] - Sign certificate : " << GenericStatusEnumTypeHelper.toString(response.status);

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
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ChargePointHandler::handleMessage(const ocpp::messages::ocpp16::SignedFirmwareStatusNotificationReq& request,
                                       ocpp::messages::ocpp16::SignedFirmwareStatusNotificationConf&      response,
                                       std::string&                                                       error_code,
                                       std::string&                                                       error_message)
{
    bool ret = false;
    (void)error_message;

    LOG_INFO << "[" << m_identifier << "] - Signed firmware update status notification received : requestId = "
             << (request.requestId.isSet() ? std::to_string(request.requestId) : "not set")
             << " -  status = " << FirmwareStatusEnumTypeHelper.toString(request.status);

    // Notify request
    if (m_handler)
    {
        m_handler->signedFirmwareUpdateStatusNotification(request.status, request.requestId);

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

// ISO 15118 PnC extensions

/** @brief Handle an Iso15118Authorize request */
void ChargePointHandler::handleMessage(const ocpp::messages::ocpp16::Iso15118AuthorizeReq& request,
                                       ocpp::messages::ocpp16::Iso15118AuthorizeConf&      response)
{
    LOG_INFO << "[" << m_identifier << "] - [ISO15118] Authorize requested : idToken = " << request.idToken.str()
             << " -  certificate = " << (request.certificate.isSet() ? std::to_string(request.certificate.value().size()) : "not set");

    // Load certificate
    ocpp::x509::Certificate certificate(request.certificate.value());
    if (!request.certificate.isSet() || certificate.isValid())
    {
        // Notify request
        response.idTokenInfo =
            m_handler->iso15118Authorize(certificate, request.idToken, request.iso15118CertificateHashData, response.certificateStatus);
    }
    else
    {
        response.certificateStatus  = AuthorizeCertificateStatusEnumType::CertChainError;
        response.idTokenInfo.status = AuthorizationStatus::Invalid;
    }

    LOG_INFO << "[" << m_identifier
             << "] - [ISO15118] Authorize status : " << AuthorizationStatusHelper.toString(response.idTokenInfo.status);
}

/** @brief Handle a Get15118EVCertificate request */
void ChargePointHandler::handleMessage(const ocpp::messages::ocpp16::Get15118EVCertificateReq& request,
                                       ocpp::messages::ocpp16::Get15118EVCertificateConf&      response)
{
    LOG_INFO << "[" << m_identifier
             << "] - [ISO15118] Get EV certificate requested : action = " << CertificateActionEnumTypeHelper.toString(request.action)
             << " -  iso15118SchemaVersion = " << request.iso15118SchemaVersion.str();

    // Notify request
    std::string exi_response;
    response.status = m_handler->iso15118GetEVCertificate(request.iso15118SchemaVersion, request.action, request.exiRequest, exi_response);
    if (response.status == Iso15118EVCertificateStatusEnumType::Accepted)
    {
        response.exiResponse.assign(exi_response);
    }

    LOG_INFO << "[" << m_identifier
             << "] - [ISO15118] Get EV certificate status : " << Iso15118EVCertificateStatusEnumTypeHelper.toString(response.status);
}

/** @brief Handle a GetCertificateStatus request */
void ChargePointHandler::handleMessage(const ocpp::messages::ocpp16::GetCertificateStatusReq& request,
                                       ocpp::messages::ocpp16::GetCertificateStatusConf&      response)
{
    LOG_INFO << "[" << m_identifier << "] - [ISO15118] Get certificate status requested : hashAlgorithm = "
             << HashAlgorithmEnumTypeHelper.toString(request.ocspRequestData.hashAlgorithm)
             << " -  issuerKeyHash = " << request.ocspRequestData.issuerKeyHash.str()
             << " -  issuerNameHash = " << request.ocspRequestData.issuerNameHash.str()
             << " -  responderURL = " << request.ocspRequestData.responderURL.str()
             << " -  serialNumber = " << request.ocspRequestData.serialNumber.str();

    // Notify request
    std::string ocsp_result;
    response.status = m_handler->iso15118GetCertificateStatus(request.ocspRequestData, ocsp_result);
    if (response.status == GetCertificateStatusEnumType::Accepted)
    {
        if (!ocsp_result.empty())
        {
            response.ocspResult.value().assign(ocsp_result);
        }
    }

    LOG_INFO << "[" << m_identifier
             << "] - [ISO15118] Get certificate status : " << GetCertificateStatusEnumTypeHelper.toString(response.status);
}

/** @brief Handle a SignCertificate request */
void ChargePointHandler::handleMessage(const ocpp::messages::ocpp16::SignCertificateReq& request,
                                       ocpp::messages::ocpp16::SignCertificateConf&      response)
{
    LOG_INFO << "[" << m_identifier << "] - [ISO15118] Sign certificate requested : csr size = " << request.csr.size();

    // Prepare response
    response.status = GenericStatusEnumType::Rejected;

    // Load certificate request
    ocpp::x509::CertificateRequest certificate_request(request.csr);
    if (certificate_request.isValid())
    {
        // Notify request
        if (m_handler->iso15118SignCertificate(certificate_request))
        {
            response.status = GenericStatusEnumType::Accepted;
        }
    }

    LOG_INFO << "[" << m_identifier << "] - [ISO15118] Sign certificate : " << GenericStatusEnumTypeHelper.toString(response.status);
}

} // namespace centralsystem
} // namespace ocpp
