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

#ifndef OPENOCPP_ICHARGEPOINTPROXY_H
#define OPENOCPP_ICHARGEPOINTPROXY_H

#include "CancelReservation.h"
#include "CertificateSigned.h"
#include "ChangeAvailability.h"
#include "ChangeConfiguration.h"
#include "ClearCache.h"
#include "ClearChargingProfile.h"
#include "DataTransfer.h"
#include "DeleteCertificate.h"
#include "ExtendedTriggerMessage.h"
#include "GetCompositeSchedule.h"
#include "GetConfiguration.h"
#include "GetDiagnostics.h"
#include "GetInstalledCertificateIds.h"
#include "GetLocalListVersion.h"
#include "GetLog.h"
#include "ICentralSystemProxy.h"
#include "InstallCertificate.h"
#include "RemoteStartTransaction.h"
#include "RemoteStopTransaction.h"
#include "ReserveNow.h"
#include "Reset.h"
#include "SendLocalList.h"
#include "SetChargingProfile.h"
#include "SignedUpdateFirmware.h"
#include "TriggerMessage.h"
#include "UnlockConnector.h"
#include "UpdateFirmware.h"

#include <functional>
#include <memory>

namespace ocpp
{
namespace localcontroller
{

/** @brief Interface for the charge point proxy implementations */
class IChargePointProxy
{
  public:
    /** @brief Destructor */
    virtual ~IChargePointProxy() { }

    /**
     * @brief Get the IP address of the charge point
     * @return IP address of the charge point
     */
    virtual const std::string& ipAddress() const = 0;

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

    /** @brief Disconnect the charge point */
    virtual void disconnect() = 0;

    /** 
     * @brief Get the associated central system proxy 
     * @return Associated central system proxy 
     */
    virtual std::shared_ptr<ICentralSystemProxy> centralSystemProxy() = 0;

    /** 
     * @brief Register a listener to the proxy events 
     * @param listener Listener to the proxy events
     */
    virtual void registerListener(ILocalControllerProxyEventsHandler& listener) = 0;

    // OCPP operations

    /**
     * @brief Cancel a reservation
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::CancelReservationReq& request,
                      ocpp::messages::CancelReservationConf&      response,
                      std::string&                                error,
                      std::string&                                message) = 0;

    /**
     * @brief Change the availability state of a connector
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ChangeAvailabilityReq& request,
                      ocpp::messages::ChangeAvailabilityConf&      response,
                      std::string&                                 error,
                      std::string&                                 message) = 0;

    /**
     * @brief Change the value of a configuration key
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ChangeConfigurationReq& request,
                      ocpp::messages::ChangeConfigurationConf&      response,
                      std::string&                                  error,
                      std::string&                                  message) = 0;

    /**
     * @brief Clear the authentication cache
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ClearCacheReq& request,
                      ocpp::messages::ClearCacheConf&      response,
                      std::string&                         error,
                      std::string&                         message) = 0;

    /**
     * @brief Clear 1 or more charging profiles
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ClearChargingProfileReq& request,
                      ocpp::messages::ClearChargingProfileConf&      response,
                      std::string&                                   error,
                      std::string&                                   message) = 0;

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
     * @brief Get a smart charging composite schedule
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::GetCompositeScheduleReq& request,
                      ocpp::messages::GetCompositeScheduleConf&      response,
                      std::string&                                   error,
                      std::string&                                   message) = 0;

    /**
     * @brief Get the value of the configuration keys
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::GetConfigurationReq& request,
                      ocpp::messages::GetConfigurationConf&      response,
                      std::string&                               error,
                      std::string&                               message) = 0;

    /**
     * @brief Get the diagnostic file
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::GetDiagnosticsReq& request,
                      ocpp::messages::GetDiagnosticsConf&      response,
                      std::string&                             error,
                      std::string&                             message) = 0;

    /**
     * @brief Get the version of the local authorization list
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::GetLocalListVersionReq& request,
                      ocpp::messages::GetLocalListVersionConf&      response,
                      std::string&                                  error,
                      std::string&                                  message) = 0;

    /**
     * @brief Send a remote start transaction request
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::RemoteStartTransactionReq& request,
                      ocpp::messages::RemoteStartTransactionConf&      response,
                      std::string&                                     error,
                      std::string&                                     message) = 0;

    /**
     * @brief Send a remote stop transaction request
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::RemoteStopTransactionReq& request,
                      ocpp::messages::RemoteStopTransactionConf&      response,
                      std::string&                                    error,
                      std::string&                                    message) = 0;

    /**
     * @brief Reserve a connector
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ReserveNowReq& request,
                      ocpp::messages::ReserveNowConf&      response,
                      std::string&                         error,
                      std::string&                         message) = 0;

    /**
     * @brief Reset the charge point
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ResetReq& request,
                      ocpp::messages::ResetConf&      response,
                      std::string&                    error,
                      std::string&                    message) = 0;

    /**
     * @brief Send or upgrade a local authorization list
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::SendLocalListReq& request,
                      ocpp::messages::SendLocalListConf&      response,
                      std::string&                            error,
                      std::string&                            message) = 0;

    /**
     * @brief Set a charging profile in the charge point
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::SetChargingProfileReq& request,
                      ocpp::messages::SetChargingProfileConf&      response,
                      std::string&                                 error,
                      std::string&                                 message) = 0;

    /**
     * @brief Request the send of a specific message
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::TriggerMessageReq& request,
                      ocpp::messages::TriggerMessageConf&      response,
                      std::string&                             error,
                      std::string&                             message) = 0;

    /**
     * @brief Unlock a connector
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::UnlockConnectorReq& request,
                      ocpp::messages::UnlockConnectorConf&      response,
                      std::string&                              error,
                      std::string&                              message) = 0;

    /**
     * @brief Update the firmware of the charge point
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::UpdateFirmwareReq& request,
                      ocpp::messages::UpdateFirmwareConf&      response,
                      std::string&                             error,
                      std::string&                             message) = 0;

    // Security extensions

    /**
     * @brief Send a generated certificate chain after a SignCertificate request from the charge point
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::CertificateSignedReq& request,
                      ocpp::messages::CertificateSignedConf&      response,
                      std::string&                                error,
                      std::string&                                message) = 0;

    /**
     * @brief Delete an installed CA certificate
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::DeleteCertificateReq& request,
                      ocpp::messages::DeleteCertificateConf&      response,
                      std::string&                                error,
                      std::string&                                message) = 0;

    /**
     * @brief Request the send of a specific message
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ExtendedTriggerMessageReq& request,
                      ocpp::messages::ExtendedTriggerMessageConf&      response,
                      std::string&                                     error,
                      std::string&                                     message) = 0;

    /**
     * @brief Get the list of installed CA certificates
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::GetInstalledCertificateIdsReq& request,
                      ocpp::messages::GetInstalledCertificateIdsConf&      response,
                      std::string&                                         error,
                      std::string&                                         message) = 0;

    /**
     * @brief Get the log file
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::GetLogReq& request,
                      ocpp::messages::GetLogConf&      response,
                      std::string&                     error,
                      std::string&                     message) = 0;

    /**
     * @brief Install a CA certificate
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::InstallCertificateReq& request,
                      ocpp::messages::InstallCertificateConf&      response,
                      std::string&                                 error,
                      std::string&                                 message) = 0;

    /**
     * @brief Update the firmware of the charge point
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::SignedUpdateFirmwareReq& request,
                      ocpp::messages::SignedUpdateFirmwareConf&      response,
                      std::string&                                   error,
                      std::string&                                   message) = 0;

    /**
     * @brief Register a handler for the Authorize request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::AuthorizeReq&, ocpp::messages::AuthorizeConf&, const char*&, std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the BootNotification request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::BootNotificationReq&, ocpp::messages::BootNotificationConf&, const char*&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the DataTransfer request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::DataTransferReq&, ocpp::messages::DataTransferConf&, const char*&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the DiagnosticsStatusNotification request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::DiagnosticsStatusNotificationReq&,
                                                    ocpp::messages::DiagnosticsStatusNotificationConf&,
                                                    const char*&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the FirmwareStatusNotification request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::FirmwareStatusNotificationReq&,
                                                    ocpp::messages::FirmwareStatusNotificationConf&,
                                                    const char*&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the Heartbeat request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::HeartbeatReq&, ocpp::messages::HeartbeatConf&, const char*&, std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the MeterValues request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::MeterValuesReq&, ocpp::messages::MeterValuesConf&, const char*&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the StartTransaction request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::StartTransactionReq&, ocpp::messages::StartTransactionConf&, const char*&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the StatusNotification request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::StatusNotificationReq&, ocpp::messages::StatusNotificationConf&, const char*&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the StopTransaction request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::StopTransactionReq&, ocpp::messages::StopTransactionConf&, const char*&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the LogStatusNotification request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::LogStatusNotificationReq&, ocpp::messages::LogStatusNotificationConf&, const char*&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the SecurityEventNotification request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::SecurityEventNotificationReq&,
                                                    ocpp::messages::SecurityEventNotificationConf&,
                                                    const char*&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the SignCertificate request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::SignCertificateReq&, ocpp::messages::SignCertificateConf&, const char*&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the SignedFirmwareStatusNotification request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::SignedFirmwareStatusNotificationReq&,
                                                    ocpp::messages::SignedFirmwareStatusNotificationConf&,
                                                    const char*&,
                                                    std::string&)> handler) = 0;
};

} // namespace localcontroller
} // namespace ocpp

#endif // OPENOCPP_ICHARGEPOINTPROXY_H
