/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License version 2.1
as published by the Free Software Foundation.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPENOCPP_CHARGEPOINTHANDLER_H
#define OPENOCPP_CHARGEPOINTHANDLER_H

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
    : public ocpp::messages::GenericMessageHandler<ocpp::messages::AuthorizeReq, ocpp::messages::AuthorizeConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::BootNotificationReq, ocpp::messages::BootNotificationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::DataTransferReq, ocpp::messages::DataTransferConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::DiagnosticsStatusNotificationReq,
                                                   ocpp::messages::DiagnosticsStatusNotificationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::FirmwareStatusNotificationReq,
                                                   ocpp::messages::FirmwareStatusNotificationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::HeartbeatReq, ocpp::messages::HeartbeatConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::MeterValuesReq, ocpp::messages::MeterValuesConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::StartTransactionReq, ocpp::messages::StartTransactionConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::StatusNotificationReq, ocpp::messages::StatusNotificationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::StopTransactionReq, ocpp::messages::StopTransactionConf>,
      // Security extensions
      public ocpp::messages::GenericMessageHandler<ocpp::messages::LogStatusNotificationReq, ocpp::messages::LogStatusNotificationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::SecurityEventNotificationReq,
                                                   ocpp::messages::SecurityEventNotificationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::SignCertificateReq, ocpp::messages::SignCertificateConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::SignedFirmwareStatusNotificationReq,
                                                   ocpp::messages::SignedFirmwareStatusNotificationConf>
{
  public:
    /**
     * @brief Constructor
     * @param identifier Charge point's identifier
     * @param messages_converter Converter from/to OCPP to/from JSON messages
     * @param msg_dispatcher Message dispatcher
     * @param stack_config Stack configuration
     */
    ChargePointHandler(const std::string&                        identifier,
                       const ocpp::messages::MessagesConverter&  messages_converter,
                       ocpp::messages::MessageDispatcher&        msg_dispatcher,
                       const ocpp::config::ICentralSystemConfig& stack_config);
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
    bool handleMessage(const ocpp::messages::AuthorizeReq& request,
                       ocpp::messages::AuthorizeConf&      response,
                       std::string&                        error_code,
                       std::string&                        error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::BootNotificationReq& request,
                       ocpp::messages::BootNotificationConf&      response,
                       std::string&                               error_code,
                       std::string&                               error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::DataTransferReq& request,
                       ocpp::messages::DataTransferConf&      response,
                       std::string&                           error_code,
                       std::string&                           error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::DiagnosticsStatusNotificationReq& request,
                       ocpp::messages::DiagnosticsStatusNotificationConf&      response,
                       std::string&                                            error_code,
                       std::string&                                            error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::FirmwareStatusNotificationReq& request,
                       ocpp::messages::FirmwareStatusNotificationConf&      response,
                       std::string&                                         error_code,
                       std::string&                                         error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::HeartbeatReq& request,
                       ocpp::messages::HeartbeatConf&      response,
                       std::string&                        error_code,
                       std::string&                        error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::MeterValuesReq& request,
                       ocpp::messages::MeterValuesConf&      response,
                       std::string&                          error_code,
                       std::string&                          error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::StartTransactionReq& request,
                       ocpp::messages::StartTransactionConf&      response,
                       std::string&                               error_code,
                       std::string&                               error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::StatusNotificationReq& request,
                       ocpp::messages::StatusNotificationConf&      response,
                       std::string&                                 error_code,
                       std::string&                                 error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::StopTransactionReq& request,
                       ocpp::messages::StopTransactionConf&      response,
                       std::string&                              error_code,
                       std::string&                              error_message) override;

    // Security extensions

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::LogStatusNotificationReq& request,
                       ocpp::messages::LogStatusNotificationConf&      response,
                       std::string&                                    error_code,
                       std::string&                                    error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::SecurityEventNotificationReq& request,
                       ocpp::messages::SecurityEventNotificationConf&      response,
                       std::string&                                        error_code,
                       std::string&                                        error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::SignCertificateReq& request,
                       ocpp::messages::SignCertificateConf&      response,
                       std::string&                              error_code,
                       std::string&                              error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::SignedFirmwareStatusNotificationReq& request,
                       ocpp::messages::SignedFirmwareStatusNotificationConf&      response,
                       std::string&                                               error_code,
                       std::string&                                               error_message) override;

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
    ocpp::types::DataTransferStatus handleMessage(const std::string& type_id, const std::string& request_data, std::string& response_data)
    {
        ocpp::types::DataTransferStatus status         = ocpp::types::DataTransferStatus::Rejected;
        auto                            req_converter  = m_messages_converter.getRequestConverter<RequestType>(type_id);
        auto                            resp_converter = m_messages_converter.getResponseConverter<ResponseType>(type_id);
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
                    rapidjson::Document response;
                    response.Parse("{}");
                    resp_converter->setAllocator(&response.GetAllocator());
                    if (resp_converter->toJson(resp, response))
                    {

                        // Serialize response
                        rapidjson::StringBuffer                    buffer;
                        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                        response.Accept(writer);
                        response_data = buffer.GetString();

                        status = ocpp::types::DataTransferStatus::Accepted;
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
    void handleMessage(const ocpp::messages::Iso15118AuthorizeReq& request, ocpp::messages::Iso15118AuthorizeConf& response);
    /** @brief Handle a Get15118EVCertificate request */
    void handleMessage(const ocpp::messages::Get15118EVCertificateReq& request, ocpp::messages::Get15118EVCertificateConf& response);
    /** @brief Handle a GetCertificateStatus request */
    void handleMessage(const ocpp::messages::GetCertificateStatusReq& request, ocpp::messages::GetCertificateStatusConf& response);
    /** @brief Handle a SignCertificate request */
    void handleMessage(const ocpp::messages::SignCertificateReq& request, ocpp::messages::SignCertificateConf& response);
};

} // namespace centralsystem
} // namespace ocpp

#endif // OPENOCPP_CHARGEPOINTHANDLER_H
