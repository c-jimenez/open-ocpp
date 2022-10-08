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

#ifndef OPENOCPP_ICENTRALSYSTEMPROXY_H
#define OPENOCPP_ICENTRALSYSTEMPROXY_H

#include "Authorize.h"
#include "BootNotification.h"
#include "DataTransfer.h"
#include "DiagnosticsStatusNotification.h"
#include "FirmwareStatusNotification.h"
#include "Heartbeat.h"
#include "ILocalControllerProxyEventsHandler.h"
#include "IWebsocketClient.h"
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
namespace localcontroller
{

/** @brief Interface for central system proxy implementations */
class ICentralSystemProxy
{
  public:
    /** @brief Destructor */
    virtual ~ICentralSystemProxy() { }

    /**
     * @brief Get the charge point identifier
     * @return charge point identifier
     */
    virtual const std::string& identifier() const = 0;

    /**
     * @brief Set the call request timeout
     * @param timeout New timeout value
     */
    virtual void setTimeout(std::chrono::milliseconds timeout) = 0;

    /**
     * @brief Connect to the central system
     * @param url URL to connect to
     * @param credentials Credentials to use
     * @param connect_timeout Connection timeout in ms
     * @param retry_interval Retry interval in ms when connection cannot be established (0 = no retry)
     * @param ping_interval Interval between 2 websocket PING messages when the socket is idle
     * @return true if the client has been started, false otherwise
     */
    virtual bool connect(const std::string&                                     url,
                         const ocpp::websockets::IWebsocketClient::Credentials& credentials,
                         std::chrono::milliseconds                              connect_timeout = std::chrono::seconds(5),
                         std::chrono::milliseconds                              retry_interval  = std::chrono::seconds(5),
                         std::chrono::milliseconds                              ping_interval   = std::chrono::seconds(5)) = 0;

    /** @brief Disconnect from the central system */
    virtual void disconnect() = 0;

    /** 
     * @brief Register a listener to the proxy events 
     * @param listener Listener to the proxy events
     */
    virtual void registerListener(ILocalControllerProxyEventsHandler& listener) = 0;

    /**
     * @brief Notify the startup of the device
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::BootNotificationReq& request,
                      ocpp::messages::BootNotificationConf&      response,
                      std::string&                               error,
                      std::string&                               message) = 0;

    /**
     * @brief Notify a new status for a connector
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::StatusNotificationReq& request,
                      ocpp::messages::StatusNotificationConf&      response,
                      std::string&                                 error,
                      std::string&                                 message) = 0;

    /**
     * @brief Ask for authorization of an operation on a connector
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::AuthorizeReq& request,
                      ocpp::messages::AuthorizeConf&      response,
                      std::string&                        error,
                      std::string&                        message) = 0;

    /**
     * @brief Start a transaction
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::StartTransactionReq& request,
                      ocpp::messages::StartTransactionConf&      response,
                      std::string&                               error,
                      std::string&                               message) = 0;

    /**
     * @brief Stop a transaction
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::StopTransactionReq& request,
                      ocpp::messages::StopTransactionConf&      response,
                      std::string&                              error,
                      std::string&                              message) = 0;

    /**
     * @brief Send a data transfer request
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::DataTransferReq& request,
                      ocpp::messages::DataTransferConf&      response,
                      std::string&                           error,
                      std::string&                           message) = 0;

    /**
     * @brief Send meter values to Central System for a given connector
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::MeterValuesReq& request,
                      ocpp::messages::MeterValuesConf&      response,
                      std::string&                          error,
                      std::string&                          message) = 0;

    /**
     * @brief Notify the status of a get diagnostics operation
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::DiagnosticsStatusNotificationReq& request,
                      ocpp::messages::DiagnosticsStatusNotificationConf&      response,
                      std::string&                                            error,
                      std::string&                                            message) = 0;

    /**
     * @brief Notify the status of a firmware update operation
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::FirmwareStatusNotificationReq& request,
                      ocpp::messages::FirmwareStatusNotificationConf&      response,
                      std::string&                                         error,
                      std::string&                                         message) = 0;

    /**
     * @brief Send a heartbeat
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::HeartbeatReq& request,
                      ocpp::messages::HeartbeatConf&      response,
                      std::string&                        error,
                      std::string&                        message) = 0;

    // Security extensions

    /**
     * @brief Log a security event
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::SecurityEventNotificationReq& request,
                      ocpp::messages::SecurityEventNotificationConf&      response,
                      std::string&                                        error,
                      std::string&                                        message) = 0;

    /**
     * @brief Send a CSR request to sign a certificate
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::SignCertificateReq& request,
                      ocpp::messages::SignCertificateConf&      response,
                      std::string&                              error,
                      std::string&                              message) = 0;

    /**
     * @brief Notify the status of a get logs operation
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::LogStatusNotificationReq& request,
                      ocpp::messages::LogStatusNotificationConf&      response,
                      std::string&                                    error,
                      std::string&                                    message) = 0;

    /**
     * @brief Notify the status of a signed firmware update operation
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::SignedFirmwareStatusNotificationReq& request,
                      ocpp::messages::SignedFirmwareStatusNotificationConf&      response,
                      std::string&                                               error,
                      std::string&                                               message) = 0;
};

} // namespace localcontroller
} // namespace ocpp

#endif // OPENOCPP_ICENTRALSYSTEMPROXY_H
