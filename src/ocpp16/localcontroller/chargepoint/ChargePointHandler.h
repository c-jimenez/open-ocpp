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

#ifndef OPENOCPP_LC_CHARGEPOINTHANDLER_H
#define OPENOCPP_LC_CHARGEPOINTHANDLER_H

#include "Authorize.h"
#include "BootNotification.h"
#include "DataTransfer.h"
#include "DiagnosticsStatusNotification.h"
#include "FirmwareStatusNotification.h"
#include "GenericMessageHandler.h"
#include "Heartbeat.h"
#include "ICentralSystemProxy.h"
#include "IRpc.h"
#include "LogStatusNotification.h"
#include "Logger.h"
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
class ILocalControllerConfig;
} // namespace config
namespace messages
{
class MessageDispatcher;
namespace ocpp16
{
class MessagesConverter;
} // namespace ocpp16
} // namespace messages

namespace localcontroller
{

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
                       ICentralSystemProxy&                             central_system);

    /** @brief Destructor */
    virtual ~ChargePointHandler();

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
    /** @brief Proxy to forward requests to the central system */
    ICentralSystemProxy& m_central_system;

    /**
     * @brief Forward a call request to the Central System
     * @param action RPC action for the request
     * @param request Request payload
     * @param response Response payload
     * @param error Error code to return to the Charge Point, empty if no error
     * @param message Error message to return to the Charge Point, empty if no error
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    template <typename RequestType, typename ResponseType>
    bool forward(const std::string& action, const RequestType& request, ResponseType& response, std::string& error, std::string& message)
    {
        bool ret = false;

        // Try to forward message
        std::string cs_error;
        std::string cs_message;
        ret = m_central_system.call(request, response, cs_error, cs_message);
        if (!ret)
        {
            // Check timeout or error response
            if (cs_error.empty())
            {
                LOG_WARNING << "[" << m_identifier << "] - Unable to forward [" << action << "] request";
            }
            else
            {
                LOG_WARNING << "[" << m_identifier << "] - CallError [" << cs_error << "] received on [" << action << "] request";
                error   = cs_error.c_str();
                message = cs_message;
            }
        }

        return ret;
    }
};

} // namespace localcontroller
} // namespace ocpp

#endif // OPENOCPP_LC_CHARGEPOINTHANDLER_H
