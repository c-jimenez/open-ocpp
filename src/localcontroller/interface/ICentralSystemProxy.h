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
#include "CancelReservation.h"
#include "CertificateSigned.h"
#include "ChangeAvailability.h"
#include "ChangeConfiguration.h"
#include "ClearCache.h"
#include "ClearChargingProfile.h"
#include "DataTransfer.h"
#include "DeleteCertificate.h"
#include "DiagnosticsStatusNotification.h"
#include "ExtendedTriggerMessage.h"
#include "FirmwareStatusNotification.h"
#include "GetCompositeSchedule.h"
#include "GetConfiguration.h"
#include "GetDiagnostics.h"
#include "GetInstalledCertificateIds.h"
#include "GetLocalListVersion.h"
#include "GetLog.h"
#include "Heartbeat.h"
#include "ILocalControllerProxyEventsHandler.h"
#include "IWebsocketClient.h"
#include "InstallCertificate.h"
#include "LogStatusNotification.h"
#include "MeterValues.h"
#include "RemoteStartTransaction.h"
#include "RemoteStopTransaction.h"
#include "ReserveNow.h"
#include "Reset.h"
#include "SecurityEventNotification.h"
#include "SendLocalList.h"
#include "SetChargingProfile.h"
#include "SignCertificate.h"
#include "SignedFirmwareStatusNotification.h"
#include "SignedUpdateFirmware.h"
#include "StartTransaction.h"
#include "StatusNotification.h"
#include "StopTransaction.h"
#include "TriggerMessage.h"
#include "UnlockConnector.h"
#include "UpdateFirmware.h"

#include <functional>

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

    /**
     * @brief Register a handler for the CancelReservation request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::CancelReservationReq&, ocpp::messages::CancelReservationConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the ChangeAvailability request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::ChangeAvailabilityReq&, ocpp::messages::ChangeAvailabilityConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the ChangeConfiguration request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::ChangeConfigurationReq&, ocpp::messages::ChangeConfigurationConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the ClearCache request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::ClearCacheReq&, ocpp::messages::ClearCacheConf&, std::string&, std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the ClearChargingProfile request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::ClearChargingProfileReq&, ocpp::messages::ClearChargingProfileConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the DataTransfer request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::DataTransferReq&, ocpp::messages::DataTransferConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the GetCompositeSchedule request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::GetCompositeScheduleReq&, ocpp::messages::GetCompositeScheduleConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the GetConfiguration request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::GetConfigurationReq&, ocpp::messages::GetConfigurationConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the GetDiagnostics request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::GetDiagnosticsReq&, ocpp::messages::GetDiagnosticsConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the GetLocalListVersion request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::GetLocalListVersionReq&, ocpp::messages::GetLocalListVersionConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the RemoteStartTransaction request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::RemoteStartTransactionReq&, ocpp::messages::RemoteStartTransactionConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the RemoteStopTransaction request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::RemoteStopTransactionReq&, ocpp::messages::RemoteStopTransactionConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the ReserveNow request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::ReserveNowReq&, ocpp::messages::ReserveNowConf&, std::string&, std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the Reset request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::ResetReq&, ocpp::messages::ResetConf&, std::string&, std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the SendLocalList request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::SendLocalListReq&, ocpp::messages::SendLocalListConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the SetChargingProfile request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::SetChargingProfileReq&, ocpp::messages::SetChargingProfileConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the TriggerMessage request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::TriggerMessageReq&, ocpp::messages::TriggerMessageConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the UnlockConnector request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::UnlockConnectorReq&, ocpp::messages::UnlockConnectorConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the UpdateFirmware request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::UpdateFirmwareReq&, ocpp::messages::UpdateFirmwareConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the CertificateSigned request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::CertificateSignedReq&, ocpp::messages::CertificateSignedConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the DeleteCertificate request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::DeleteCertificateReq&, ocpp::messages::DeleteCertificateConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the ExtendedTriggerMessage request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::ExtendedTriggerMessageReq&, ocpp::messages::ExtendedTriggerMessageConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the GetInstalledCertificateIds request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::GetInstalledCertificateIdsReq&,
                                                    ocpp::messages::GetInstalledCertificateIdsConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the GetLog request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::GetLogReq&, ocpp::messages::GetLogConf&, std::string&, std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the InstallCertificate request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::InstallCertificateReq&, ocpp::messages::InstallCertificateConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the SignedUpdateFirmware request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::SignedUpdateFirmwareReq&, ocpp::messages::SignedUpdateFirmwareConf&, std::string&, std::string&)>
            handler) = 0;
};

} // namespace localcontroller
} // namespace ocpp

#endif // OPENOCPP_ICENTRALSYSTEMPROXY_H
