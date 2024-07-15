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
     * @brief Indicate if the connection with the central system is active
     * @return true if the connection is active, false otherwise
     */
    virtual bool isConnected() const = 0;

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
    virtual bool call(const ocpp::messages::ocpp16::BootNotificationReq& request,
                      ocpp::messages::ocpp16::BootNotificationConf&      response,
                      std::string&                                       error,
                      std::string&                                       message) = 0;

    /**
     * @brief Notify a new status for a connector
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::StatusNotificationReq& request,
                      ocpp::messages::ocpp16::StatusNotificationConf&      response,
                      std::string&                                         error,
                      std::string&                                         message) = 0;

    /**
     * @brief Ask for authorization of an operation on a connector
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::AuthorizeReq& request,
                      ocpp::messages::ocpp16::AuthorizeConf&      response,
                      std::string&                                error,
                      std::string&                                message) = 0;

    /**
     * @brief Start a transaction
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::StartTransactionReq& request,
                      ocpp::messages::ocpp16::StartTransactionConf&      response,
                      std::string&                                       error,
                      std::string&                                       message) = 0;

    /**
     * @brief Stop a transaction
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::StopTransactionReq& request,
                      ocpp::messages::ocpp16::StopTransactionConf&      response,
                      std::string&                                      error,
                      std::string&                                      message) = 0;

    /**
     * @brief Send a data transfer request
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::DataTransferReq& request,
                      ocpp::messages::ocpp16::DataTransferConf&      response,
                      std::string&                                   error,
                      std::string&                                   message) = 0;

    /**
     * @brief Send meter values to Central System for a given connector
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::MeterValuesReq& request,
                      ocpp::messages::ocpp16::MeterValuesConf&      response,
                      std::string&                                  error,
                      std::string&                                  message) = 0;

    /**
     * @brief Notify the status of a get diagnostics operation
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::DiagnosticsStatusNotificationReq& request,
                      ocpp::messages::ocpp16::DiagnosticsStatusNotificationConf&      response,
                      std::string&                                                    error,
                      std::string&                                                    message) = 0;

    /**
     * @brief Notify the status of a firmware update operation
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::FirmwareStatusNotificationReq& request,
                      ocpp::messages::ocpp16::FirmwareStatusNotificationConf&      response,
                      std::string&                                                 error,
                      std::string&                                                 message) = 0;

    /**
     * @brief Send a heartbeat
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::HeartbeatReq& request,
                      ocpp::messages::ocpp16::HeartbeatConf&      response,
                      std::string&                                error,
                      std::string&                                message) = 0;

    // Security extensions

    /**
     * @brief Log a security event
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::SecurityEventNotificationReq& request,
                      ocpp::messages::ocpp16::SecurityEventNotificationConf&      response,
                      std::string&                                                error,
                      std::string&                                                message) = 0;

    /**
     * @brief Send a CSR request to sign a certificate
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::SignCertificateReq& request,
                      ocpp::messages::ocpp16::SignCertificateConf&      response,
                      std::string&                                      error,
                      std::string&                                      message) = 0;

    /**
     * @brief Notify the status of a get logs operation
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::LogStatusNotificationReq& request,
                      ocpp::messages::ocpp16::LogStatusNotificationConf&      response,
                      std::string&                                            error,
                      std::string&                                            message) = 0;

    /**
     * @brief Notify the status of a signed firmware update operation
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::SignedFirmwareStatusNotificationReq& request,
                      ocpp::messages::ocpp16::SignedFirmwareStatusNotificationConf&      response,
                      std::string&                                                       error,
                      std::string&                                                       message) = 0;

    /**
     * @brief Register a handler for the CancelReservation request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::CancelReservationReq&,
                                                    ocpp::messages::ocpp16::CancelReservationConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the ChangeAvailability request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::ChangeAvailabilityReq&,
                                                    ocpp::messages::ocpp16::ChangeAvailabilityConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the ChangeConfiguration request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::ChangeConfigurationReq&,
                                                    ocpp::messages::ocpp16::ChangeConfigurationConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the ClearCache request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::ocpp16::ClearCacheReq&, ocpp::messages::ocpp16::ClearCacheConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the ClearChargingProfile request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::ClearChargingProfileReq&,
                                                    ocpp::messages::ocpp16::ClearChargingProfileConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the DataTransfer request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::ocpp16::DataTransferReq&, ocpp::messages::ocpp16::DataTransferConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the GetCompositeSchedule request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::GetCompositeScheduleReq&,
                                                    ocpp::messages::ocpp16::GetCompositeScheduleConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the GetConfiguration request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(
            const ocpp::messages::ocpp16::GetConfigurationReq&, ocpp::messages::ocpp16::GetConfigurationConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the GetDiagnostics request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::ocpp16::GetDiagnosticsReq&, ocpp::messages::ocpp16::GetDiagnosticsConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the GetLocalListVersion request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::GetLocalListVersionReq&,
                                                    ocpp::messages::ocpp16::GetLocalListVersionConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the RemoteStartTransaction request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::RemoteStartTransactionReq&,
                                                    ocpp::messages::ocpp16::RemoteStartTransactionConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the RemoteStopTransaction request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::RemoteStopTransactionReq&,
                                                    ocpp::messages::ocpp16::RemoteStopTransactionConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the ReserveNow request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::ocpp16::ReserveNowReq&, ocpp::messages::ocpp16::ReserveNowConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the Reset request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::ocpp16::ResetReq&, ocpp::messages::ocpp16::ResetConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the SendLocalList request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::ocpp16::SendLocalListReq&, ocpp::messages::ocpp16::SendLocalListConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the SetChargingProfile request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::SetChargingProfileReq&,
                                                    ocpp::messages::ocpp16::SetChargingProfileConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the TriggerMessage request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::ocpp16::TriggerMessageReq&, ocpp::messages::ocpp16::TriggerMessageConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the UnlockConnector request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(
            const ocpp::messages::ocpp16::UnlockConnectorReq&, ocpp::messages::ocpp16::UnlockConnectorConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the UpdateFirmware request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::ocpp16::UpdateFirmwareReq&, ocpp::messages::ocpp16::UpdateFirmwareConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the CertificateSigned request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::CertificateSignedReq&,
                                                    ocpp::messages::ocpp16::CertificateSignedConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the DeleteCertificate request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::DeleteCertificateReq&,
                                                    ocpp::messages::ocpp16::DeleteCertificateConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the ExtendedTriggerMessage request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::ExtendedTriggerMessageReq&,
                                                    ocpp::messages::ocpp16::ExtendedTriggerMessageConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the GetInstalledCertificateIds request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::GetInstalledCertificateIdsReq&,
                                                    ocpp::messages::ocpp16::GetInstalledCertificateIdsConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the GetLog request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::ocpp16::GetLogReq&, ocpp::messages::ocpp16::GetLogConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the InstallCertificate request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::InstallCertificateReq&,
                                                    ocpp::messages::ocpp16::InstallCertificateConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the SignedUpdateFirmware request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::SignedUpdateFirmwareReq&,
                                                    ocpp::messages::ocpp16::SignedUpdateFirmwareConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;
};

} // namespace localcontroller
} // namespace ocpp

#endif // OPENOCPP_ICENTRALSYSTEMPROXY_H
