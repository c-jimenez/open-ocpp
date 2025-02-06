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

#ifndef OPENOCPP_CS_CHARGEPOINTHANDLER_H
#define OPENOCPP_CS_CHARGEPOINTHANDLER_H

#include "Authorize.h"
#include "BootNotification.h"
#include "DataTransfer.h"
#include "DiagnosticsStatusNotification.h"
#include "FirmwareStatusNotification.h"
#include "GenericMessageHandler.h"
#include "Get15118EVCertificate.h"
#include "GetCertificateStatus.h"
#include "Heartbeat.h"
#include "Iso15118Authorize.h"
#include "LogStatusNotification.h"
#include "Logger.h"
#include "MessagesConverter.h"
#include "MeterValues.h"
#include "SecurityEventNotification.h"
#include "SignCertificate.h"
#include "SignedFirmwareStatusNotification.h"
#include "StartTransaction.h"
#include "StatusNotification.h"
#include "StopTransaction.h"

namespace ocpp
{
namespace config
{
class ICentralSystemConfig;
} // namespace config
namespace messages
{
class MessageDispatcher;
class MessagesConverter;
} // namespace messages

namespace centralsystem
{

class IChargePointRequestHandler;

/** @brief Handler for charge point requests */
class ChargePointHandler
    : public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::AuthorizeReq, ocpp::messages::ocpp16::AuthorizeConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::BootNotificationReq,
                                                   ocpp::messages::ocpp16::BootNotificationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::DataTransferReq, ocpp::messages::ocpp16::DataTransferConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::DiagnosticsStatusNotificationReq,
                                                   ocpp::messages::ocpp16::DiagnosticsStatusNotificationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::FirmwareStatusNotificationReq,
                                                   ocpp::messages::ocpp16::FirmwareStatusNotificationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::HeartbeatReq, ocpp::messages::ocpp16::HeartbeatConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::MeterValuesReq, ocpp::messages::ocpp16::MeterValuesConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::StartTransactionReq,
                                                   ocpp::messages::ocpp16::StartTransactionConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::StatusNotificationReq,
                                                   ocpp::messages::ocpp16::StatusNotificationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::StopTransactionReq, ocpp::messages::ocpp16::StopTransactionConf>,
      // Security extensions
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::LogStatusNotificationReq,
                                                   ocpp::messages::ocpp16::LogStatusNotificationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::SecurityEventNotificationReq,
                                                   ocpp::messages::ocpp16::SecurityEventNotificationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::SignCertificateReq, ocpp::messages::ocpp16::SignCertificateConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::SignedFirmwareStatusNotificationReq,
                                                   ocpp::messages::ocpp16::SignedFirmwareStatusNotificationConf>
{
  public:
    /**
     * @brief Constructor
     * @param identifier Charge point's identifier
     * @param messages_converter Converter from/to OCPP to/from JSON messages
     * @param msg_dispatcher Message dispatcher
     * @param stack_config Stack configuration
     */
    ChargePointHandler(const std::string&                               identifier,
                       const ocpp::messages::ocpp16::MessagesConverter& messages_converter,
                       ocpp::messages::MessageDispatcher&               msg_dispatcher,
                       const ocpp::config::ICentralSystemConfig&        stack_config);
    /** @brief Destructor */
    virtual ~ChargePointHandler();

    /** @brief Register the event handler */
    void registerHandler(IChargePointRequestHandler& handler) { m_handler = &handler; }

    // OCPP handlers

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::AuthorizeReq& request,
                       ocpp::messages::ocpp16::AuthorizeConf&      response,
                       std::string&                                error_code,
                       std::string&                                error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::BootNotificationReq& request,
                       ocpp::messages::ocpp16::BootNotificationConf&      response,
                       std::string&                                       error_code,
                       std::string&                                       error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::DataTransferReq& request,
                       ocpp::messages::ocpp16::DataTransferConf&      response,
                       std::string&                                   error_code,
                       std::string&                                   error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::DiagnosticsStatusNotificationReq& request,
                       ocpp::messages::ocpp16::DiagnosticsStatusNotificationConf&      response,
                       std::string&                                                    error_code,
                       std::string&                                                    error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::FirmwareStatusNotificationReq& request,
                       ocpp::messages::ocpp16::FirmwareStatusNotificationConf&      response,
                       std::string&                                                 error_code,
                       std::string&                                                 error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::HeartbeatReq& request,
                       ocpp::messages::ocpp16::HeartbeatConf&      response,
                       std::string&                                error_code,
                       std::string&                                error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::MeterValuesReq& request,
                       ocpp::messages::ocpp16::MeterValuesConf&      response,
                       std::string&                                  error_code,
                       std::string&                                  error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::StartTransactionReq& request,
                       ocpp::messages::ocpp16::StartTransactionConf&      response,
                       std::string&                                       error_code,
                       std::string&                                       error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::StatusNotificationReq& request,
                       ocpp::messages::ocpp16::StatusNotificationConf&      response,
                       std::string&                                         error_code,
                       std::string&                                         error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::StopTransactionReq& request,
                       ocpp::messages::ocpp16::StopTransactionConf&      response,
                       std::string&                                      error_code,
                       std::string&                                      error_message) override;

    // Security extensions

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::LogStatusNotificationReq& request,
                       ocpp::messages::ocpp16::LogStatusNotificationConf&      response,
                       std::string&                                            error_code,
                       std::string&                                            error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::SecurityEventNotificationReq& request,
                       ocpp::messages::ocpp16::SecurityEventNotificationConf&      response,
                       std::string&                                                error_code,
                       std::string&                                                error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::SignCertificateReq& request,
                       ocpp::messages::ocpp16::SignCertificateConf&      response,
                       std::string&                                      error_code,
                       std::string&                                      error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::SignedFirmwareStatusNotificationReq& request,
                       ocpp::messages::ocpp16::SignedFirmwareStatusNotificationConf&      response,
                       std::string&                                                       error_code,
                       std::string&                                                       error_message) override;

  private:
    /** @brief Charge point's identifier */
    const std::string m_identifier;
    /** @brief Stack configuration */
    const ocpp::config::ICentralSystemConfig& m_stack_config;
    /** @brief Messages converters */
    const ocpp::messages::GenericMessagesConverter& m_messages_converter;
    /** @brief Request handler */
    IChargePointRequestHandler* m_handler;

    // ISO 15118 PnC extensions

    /**
     * @brief Generic ISO15118 request handler
     * @param type_id Type of message
     * @param request_data Data associated to the request
     * @param response_data Data associated with the response
     * @return Response status (see DataTransferStatus enum)
     */
    template <typename RequestType, typename ResponseType>
    ocpp::types::ocpp16::DataTransferStatus handleMessage(const std::string& type_id,
                                                          const std::string& request_data,
                                                          std::string&       response_data)
    {
        ocpp::types::ocpp16::DataTransferStatus status         = ocpp::types::ocpp16::DataTransferStatus::Rejected;
        auto                                    req_converter  = m_messages_converter.getRequestConverter<RequestType>(type_id);
        auto                                    resp_converter = m_messages_converter.getResponseConverter<ResponseType>(type_id);
        try
        {
            // Parse JSON
            rapidjson::Document request;
            request.Parse(request_data.c_str());
            if (!request.HasParseError())
            {
                // Convert request from JSON
                RequestType req;
                std::string error_code;
                std::string error_message;
                if (req_converter->fromJson(request, req, error_code, error_message))
                {
                    // Handle message
                    ResponseType resp;
                    handleMessage(req, resp);

                    // Convert response to JSON
                    rapidjson::Document response(rapidjson::kObjectType);
                    resp_converter->setAllocator(&response.GetAllocator());
                    if (resp_converter->toJson(resp, response))
                    {

                        // Serialize response
                        rapidjson::StringBuffer                    buffer;
                        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                        response.Accept(writer);
                        response_data = buffer.GetString();

                        status = ocpp::types::ocpp16::DataTransferStatus::Accepted;
                    }
                }
                else
                {
                    LOG_ERROR << "[ISO15118] << " << type_id << " : Invalid JSON received";
                }
            }
            else
            {
                LOG_ERROR << "[ISO15118] << " << type_id << " : Invalid JSON received";
            }
        }
        catch (const std::exception&)
        {
            LOG_ERROR << "[ISO15118] << " << type_id << " : Invalid JSON received";
        }
        return status;
    }

    /** @brief Handle an Iso15118Authorize request */
    void handleMessage(const ocpp::messages::ocpp16::Iso15118AuthorizeReq& request,
                       ocpp::messages::ocpp16::Iso15118AuthorizeConf&      response);
    /** @brief Handle a Get15118EVCertificate request */
    void handleMessage(const ocpp::messages::ocpp16::Get15118EVCertificateReq& request,
                       ocpp::messages::ocpp16::Get15118EVCertificateConf&      response);
    /** @brief Handle a GetCertificateStatus request */
    void handleMessage(const ocpp::messages::ocpp16::GetCertificateStatusReq& request,
                       ocpp::messages::ocpp16::GetCertificateStatusConf&      response);
    /** @brief Handle a SignCertificate request */
    void handleMessage(const ocpp::messages::ocpp16::SignCertificateReq& request, ocpp::messages::ocpp16::SignCertificateConf& response);
};

} // namespace centralsystem
} // namespace ocpp

#endif // OPENOCPP_CS_CHARGEPOINTHANDLER_H
