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

#ifndef OPENOCPP_CHARGEPOINTHANDLER_H
#define OPENOCPP_CHARGEPOINTHANDLER_H

#include "Authorize.h"
#include "BootNotification.h"
#include "DataTransfer.h"
#include "DiagnosticsStatusNotification.h"
#include "FirmwareStatusNotification.h"
#include "GenericMessageHandler.h"
#include "Heartbeat.h"
#include "LogStatusNotification.h"
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
    /** @brief Request handler */
    IChargePointRequestHandler* m_handler;
};

} // namespace centralsystem
} // namespace ocpp

#endif // OPENOCPP_CHARGEPOINTHANDLER_H
