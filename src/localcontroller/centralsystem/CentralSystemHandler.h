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

#ifndef OPENOCPP_CENTRALSYSTEMHANDLER_H
#define OPENOCPP_CENTRALSYSTEMHANDLER_H

#include "CancelReservation.h"
#include "CertificateSigned.h"
#include "ChangeAvailability.h"
#include "ChangeConfiguration.h"
#include "ClearCache.h"
#include "ClearChargingProfile.h"
#include "DataTransfer.h"
#include "DeleteCertificate.h"
#include "ExtendedTriggerMessage.h"
#include "GenericMessageHandler.h"
#include "GetCompositeSchedule.h"
#include "GetConfiguration.h"
#include "GetDiagnostics.h"
#include "GetInstalledCertificateIds.h"
#include "GetLocalListVersion.h"
#include "GetLog.h"
#include "IChargePointProxy.h"
#include "IRpc.h"
#include "InstallCertificate.h"
#include "Logger.h"
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

namespace ocpp
{
namespace config
{
class ILocalControllerConfig;
} // namespace config
namespace messages
{
class MessageDispatcher;
class MessagesConverter;
} // namespace messages

namespace localcontroller
{

/** @brief Handler for central system requests */
class CentralSystemHandler
    : public ocpp::messages::GenericMessageHandler<ocpp::messages::CancelReservationReq, ocpp::messages::CancelReservationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ChangeAvailabilityReq, ocpp::messages::ChangeAvailabilityConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ChangeConfigurationReq, ocpp::messages::ChangeConfigurationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ClearCacheReq, ocpp::messages::ClearCacheConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ClearChargingProfileReq, ocpp::messages::ClearChargingProfileConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::DataTransferReq, ocpp::messages::DataTransferConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::GetCompositeScheduleReq, ocpp::messages::GetCompositeScheduleConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::GetConfigurationReq, ocpp::messages::GetConfigurationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::GetDiagnosticsReq, ocpp::messages::GetDiagnosticsConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::GetLocalListVersionReq, ocpp::messages::GetLocalListVersionConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::RemoteStartTransactionReq, ocpp::messages::RemoteStartTransactionConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::RemoteStopTransactionReq, ocpp::messages::RemoteStopTransactionConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ReserveNowReq, ocpp::messages::ReserveNowConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ResetReq, ocpp::messages::ResetConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::SendLocalListReq, ocpp::messages::SendLocalListConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::SetChargingProfileReq, ocpp::messages::SetChargingProfileConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::TriggerMessageReq, ocpp::messages::TriggerMessageConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::UnlockConnectorReq, ocpp::messages::UnlockConnectorConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::UpdateFirmwareReq, ocpp::messages::UpdateFirmwareConf>,
      // Security extensions
      public ocpp::messages::GenericMessageHandler<ocpp::messages::CertificateSignedReq, ocpp::messages::CertificateSignedConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::DeleteCertificateReq, ocpp::messages::DeleteCertificateConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ExtendedTriggerMessageReq, ocpp::messages::ExtendedTriggerMessageConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::GetInstalledCertificateIdsReq,
                                                   ocpp::messages::GetInstalledCertificateIdsConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::GetLogReq, ocpp::messages::GetLogConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::InstallCertificateReq, ocpp::messages::InstallCertificateConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::SignedUpdateFirmwareReq, ocpp::messages::SignedUpdateFirmwareConf>
{
  public:
    /**
     * @brief Constructor
     * @param identifier Charge point's identifier
     * @param messages_converter Converter from/to OCPP to/from JSON messages
     * @param msg_dispatcher Message dispatcher
     * @param stack_config Stack configuration
     */
    CentralSystemHandler(const std::string&                       identifier,
                         const ocpp::messages::MessagesConverter& messages_converter,
                         ocpp::messages::MessageDispatcher&       msg_dispatcher);

    /** @brief Destructor */
    virtual ~CentralSystemHandler();

    /**
     * @brief Set the proxy to forward requests to the charge point
     * @param central_system Proxy to forward requests to the charge point
     */
    void setChargePointProxy(std::weak_ptr<IChargePointProxy> charge_point) { m_charge_point = charge_point; }

    // OCPP handlers

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::CancelReservationReq& request,
                       ocpp::messages::CancelReservationConf&      response,
                       std::string&                                error_code,
                       std::string&                                error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ChangeAvailabilityReq& request,
                       ocpp::messages::ChangeAvailabilityConf&      response,
                       std::string&                                 error_code,
                       std::string&                                 error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ChangeConfigurationReq& request,
                       ocpp::messages::ChangeConfigurationConf&      response,
                       std::string&                                  error_code,
                       std::string&                                  error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ClearCacheReq& request,
                       ocpp::messages::ClearCacheConf&      response,
                       std::string&                         error_code,
                       std::string&                         error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ClearChargingProfileReq& request,
                       ocpp::messages::ClearChargingProfileConf&      response,
                       std::string&                                   error_code,
                       std::string&                                   error_message) override;

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
    bool handleMessage(const ocpp::messages::GetCompositeScheduleReq& request,
                       ocpp::messages::GetCompositeScheduleConf&      response,
                       std::string&                                   error_code,
                       std::string&                                   error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::GetConfigurationReq& request,
                       ocpp::messages::GetConfigurationConf&      response,
                       std::string&                               error_code,
                       std::string&                               error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::GetDiagnosticsReq& request,
                       ocpp::messages::GetDiagnosticsConf&      response,
                       std::string&                             error_code,
                       std::string&                             error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::GetLocalListVersionReq& request,
                       ocpp::messages::GetLocalListVersionConf&      response,
                       std::string&                                  error_code,
                       std::string&                                  error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::RemoteStartTransactionReq& request,
                       ocpp::messages::RemoteStartTransactionConf&      response,
                       std::string&                                     error_code,
                       std::string&                                     error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::RemoteStopTransactionReq& request,
                       ocpp::messages::RemoteStopTransactionConf&      response,
                       std::string&                                    error_code,
                       std::string&                                    error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ReserveNowReq& request,
                       ocpp::messages::ReserveNowConf&      response,
                       std::string&                         error_code,
                       std::string&                         error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ResetReq& request,
                       ocpp::messages::ResetConf&      response,
                       std::string&                    error_code,
                       std::string&                    error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::SendLocalListReq& request,
                       ocpp::messages::SendLocalListConf&      response,
                       std::string&                            error_code,
                       std::string&                            error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::SetChargingProfileReq& request,
                       ocpp::messages::SetChargingProfileConf&      response,
                       std::string&                                 error_code,
                       std::string&                                 error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::TriggerMessageReq& request,
                       ocpp::messages::TriggerMessageConf&      response,
                       std::string&                             error_code,
                       std::string&                             error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::UnlockConnectorReq& request,
                       ocpp::messages::UnlockConnectorConf&      response,
                       std::string&                              error_code,
                       std::string&                              error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::UpdateFirmwareReq& request,
                       ocpp::messages::UpdateFirmwareConf&      response,
                       std::string&                             error_code,
                       std::string&                             error_message) override;

    // Security extensions

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::CertificateSignedReq& request,
                       ocpp::messages::CertificateSignedConf&      response,
                       std::string&                                error_code,
                       std::string&                                error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::DeleteCertificateReq& request,
                       ocpp::messages::DeleteCertificateConf&      response,
                       std::string&                                error_code,
                       std::string&                                error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ExtendedTriggerMessageReq& request,
                       ocpp::messages::ExtendedTriggerMessageConf&      response,
                       std::string&                                     error_code,
                       std::string&                                     error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::GetInstalledCertificateIdsReq& request,
                       ocpp::messages::GetInstalledCertificateIdsConf&      response,
                       std::string&                                         error_code,
                       std::string&                                         error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::GetLogReq& request,
                       ocpp::messages::GetLogConf&      response,
                       std::string&                     error_code,
                       std::string&                     error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::InstallCertificateReq& request,
                       ocpp::messages::InstallCertificateConf&      response,
                       std::string&                                 error_code,
                       std::string&                                 error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::SignedUpdateFirmwareReq& request,
                       ocpp::messages::SignedUpdateFirmwareConf&      response,
                       std::string&                                   error_code,
                       std::string&                                   error_message) override;

  private:
    /** @brief Charge point's identifier */
    const std::string m_identifier;
    /** @brief Proxy to forward requests to the charge point */
    std::weak_ptr<IChargePointProxy> m_charge_point;

    /**
     * @brief Forward a call request to the Charge Point
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

        // Get charge point proxy
        auto charge_point = m_charge_point.lock();
        if (charge_point)
        {
            // Try to forward message
            std::string cs_error;
            std::string cs_message;
            ret = charge_point->call(request, response, cs_error, cs_message);
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
        }
        else
        {
            LOG_WARNING << "[" << m_identifier << "] - Unable to forward [" << action << "] request";
        }

        return ret;
    }
};

} // namespace localcontroller
} // namespace ocpp

#endif // OPENOCPP_CENTRALSYSTEMHANDLER_H
