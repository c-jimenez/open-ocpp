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

#ifndef OPENOCPP_ICHARGEPOINTPROXY_H
#define OPENOCPP_ICHARGEPOINTPROXY_H

#include "ICentralSystem.h"
#include "ILocalControllerConfig.h"
#include "RpcPool.h"

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
     * @brief Instanciate local controller's charge point proxy from a central system's charge point proxy
     * @param central_system_proxy Central system's charge point proxy
     * @param stack_config Stack configuration for local controller
     * @param rpc_pool RPC pool
     */
    static std::shared_ptr<IChargePointProxy> createFrom(
        std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint>& central_system_proxy,
        const ocpp::config::ILocalControllerConfig&                         stack_config,
        ocpp::rpc::RpcPool&                                                 rpc_pool);

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
    virtual bool call(const ocpp::messages::ocpp16::CancelReservationReq& request,
                      ocpp::messages::ocpp16::CancelReservationConf&      response,
                      std::string&                                        error,
                      std::string&                                        message) = 0;

    /**
     * @brief Change the availability state of a connector
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::ChangeAvailabilityReq& request,
                      ocpp::messages::ocpp16::ChangeAvailabilityConf&      response,
                      std::string&                                         error,
                      std::string&                                         message) = 0;

    /**
     * @brief Change the value of a configuration key
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::ChangeConfigurationReq& request,
                      ocpp::messages::ocpp16::ChangeConfigurationConf&      response,
                      std::string&                                          error,
                      std::string&                                          message) = 0;

    /**
     * @brief Clear the authentication cache
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::ClearCacheReq& request,
                      ocpp::messages::ocpp16::ClearCacheConf&      response,
                      std::string&                                 error,
                      std::string&                                 message) = 0;

    /**
     * @brief Clear 1 or more charging profiles
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::ClearChargingProfileReq& request,
                      ocpp::messages::ocpp16::ClearChargingProfileConf&      response,
                      std::string&                                           error,
                      std::string&                                           message) = 0;

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
     * @brief Get a smart charging composite schedule
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::GetCompositeScheduleReq& request,
                      ocpp::messages::ocpp16::GetCompositeScheduleConf&      response,
                      std::string&                                           error,
                      std::string&                                           message) = 0;

    /**
     * @brief Get the value of the configuration keys
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::GetConfigurationReq& request,
                      ocpp::messages::ocpp16::GetConfigurationConf&      response,
                      std::string&                                       error,
                      std::string&                                       message) = 0;

    /**
     * @brief Get the diagnostic file
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::GetDiagnosticsReq& request,
                      ocpp::messages::ocpp16::GetDiagnosticsConf&      response,
                      std::string&                                     error,
                      std::string&                                     message) = 0;

    /**
     * @brief Get the version of the local authorization list
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::GetLocalListVersionReq& request,
                      ocpp::messages::ocpp16::GetLocalListVersionConf&      response,
                      std::string&                                          error,
                      std::string&                                          message) = 0;

    /**
     * @brief Send a remote start transaction request
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::RemoteStartTransactionReq& request,
                      ocpp::messages::ocpp16::RemoteStartTransactionConf&      response,
                      std::string&                                             error,
                      std::string&                                             message) = 0;

    /**
     * @brief Send a remote stop transaction request
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::RemoteStopTransactionReq& request,
                      ocpp::messages::ocpp16::RemoteStopTransactionConf&      response,
                      std::string&                                            error,
                      std::string&                                            message) = 0;

    /**
     * @brief Reserve a connector
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::ReserveNowReq& request,
                      ocpp::messages::ocpp16::ReserveNowConf&      response,
                      std::string&                                 error,
                      std::string&                                 message) = 0;

    /**
     * @brief Reset the charge point
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::ResetReq& request,
                      ocpp::messages::ocpp16::ResetConf&      response,
                      std::string&                            error,
                      std::string&                            message) = 0;

    /**
     * @brief Send or upgrade a local authorization list
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::SendLocalListReq& request,
                      ocpp::messages::ocpp16::SendLocalListConf&      response,
                      std::string&                                    error,
                      std::string&                                    message) = 0;

    /**
     * @brief Set a charging profile in the charge point
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::SetChargingProfileReq& request,
                      ocpp::messages::ocpp16::SetChargingProfileConf&      response,
                      std::string&                                         error,
                      std::string&                                         message) = 0;

    /**
     * @brief Request the send of a specific message
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::TriggerMessageReq& request,
                      ocpp::messages::ocpp16::TriggerMessageConf&      response,
                      std::string&                                     error,
                      std::string&                                     message) = 0;

    /**
     * @brief Unlock a connector
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::UnlockConnectorReq& request,
                      ocpp::messages::ocpp16::UnlockConnectorConf&      response,
                      std::string&                                      error,
                      std::string&                                      message) = 0;

    /**
     * @brief Update the firmware of the charge point
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::UpdateFirmwareReq& request,
                      ocpp::messages::ocpp16::UpdateFirmwareConf&      response,
                      std::string&                                     error,
                      std::string&                                     message) = 0;

    // Security extensions

    /**
     * @brief Send a generated certificate chain after a SignCertificate request from the charge point
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::CertificateSignedReq& request,
                      ocpp::messages::ocpp16::CertificateSignedConf&      response,
                      std::string&                                        error,
                      std::string&                                        message) = 0;

    /**
     * @brief Delete an installed CA certificate
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::DeleteCertificateReq& request,
                      ocpp::messages::ocpp16::DeleteCertificateConf&      response,
                      std::string&                                        error,
                      std::string&                                        message) = 0;

    /**
     * @brief Request the send of a specific message
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::ExtendedTriggerMessageReq& request,
                      ocpp::messages::ocpp16::ExtendedTriggerMessageConf&      response,
                      std::string&                                             error,
                      std::string&                                             message) = 0;

    /**
     * @brief Get the list of installed CA certificates
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::GetInstalledCertificateIdsReq& request,
                      ocpp::messages::ocpp16::GetInstalledCertificateIdsConf&      response,
                      std::string&                                                 error,
                      std::string&                                                 message) = 0;

    /**
     * @brief Get the log file
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::GetLogReq& request,
                      ocpp::messages::ocpp16::GetLogConf&      response,
                      std::string&                             error,
                      std::string&                             message) = 0;

    /**
     * @brief Install a CA certificate
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::InstallCertificateReq& request,
                      ocpp::messages::ocpp16::InstallCertificateConf&      response,
                      std::string&                                         error,
                      std::string&                                         message) = 0;

    /**
     * @brief Update the firmware of the charge point
     * @param request Request to send
     * @param response Received response
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    virtual bool call(const ocpp::messages::ocpp16::SignedUpdateFirmwareReq& request,
                      ocpp::messages::ocpp16::SignedUpdateFirmwareConf&      response,
                      std::string&                                           error,
                      std::string&                                           message) = 0;

    /**
     * @brief Register a handler for the Authorize request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::ocpp16::AuthorizeReq&, ocpp::messages::ocpp16::AuthorizeConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the BootNotification request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(
            const ocpp::messages::ocpp16::BootNotificationReq&, ocpp::messages::ocpp16::BootNotificationConf&, std::string&, std::string&)>
            handler) = 0;

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
     * @brief Register a handler for the DiagnosticsStatusNotification request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::DiagnosticsStatusNotificationReq&,
                                                    ocpp::messages::ocpp16::DiagnosticsStatusNotificationConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the FirmwareStatusNotification request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::FirmwareStatusNotificationReq&,
                                                    ocpp::messages::ocpp16::FirmwareStatusNotificationConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the Heartbeat request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(const ocpp::messages::ocpp16::HeartbeatReq&, ocpp::messages::ocpp16::HeartbeatConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the MeterValues request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<
            bool(const ocpp::messages::ocpp16::MeterValuesReq&, ocpp::messages::ocpp16::MeterValuesConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the StartTransaction request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(
            const ocpp::messages::ocpp16::StartTransactionReq&, ocpp::messages::ocpp16::StartTransactionConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the StatusNotification request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::StatusNotificationReq&,
                                                    ocpp::messages::ocpp16::StatusNotificationConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the StopTransaction request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(
            const ocpp::messages::ocpp16::StopTransactionReq&, ocpp::messages::ocpp16::StopTransactionConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the LogStatusNotification request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::LogStatusNotificationReq&,
                                                    ocpp::messages::ocpp16::LogStatusNotificationConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the SecurityEventNotification request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::SecurityEventNotificationReq&,
                                                    ocpp::messages::ocpp16::SecurityEventNotificationConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;

    /**
     * @brief Register a handler for the SignCertificate request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(
        std::function<bool(
            const ocpp::messages::ocpp16::SignCertificateReq&, ocpp::messages::ocpp16::SignCertificateConf&, std::string&, std::string&)>
            handler) = 0;

    /**
     * @brief Register a handler for the SignedFirmwareStatusNotification request
     * @param handler Handler function
     * @return true if the handler has been registered, false otherwise
     */
    virtual bool registerHandler(std::function<bool(const ocpp::messages::ocpp16::SignedFirmwareStatusNotificationReq&,
                                                    ocpp::messages::ocpp16::SignedFirmwareStatusNotificationConf&,
                                                    std::string&,
                                                    std::string&)> handler) = 0;
};

} // namespace localcontroller
} // namespace ocpp

#endif // OPENOCPP_ICHARGEPOINTPROXY_H
