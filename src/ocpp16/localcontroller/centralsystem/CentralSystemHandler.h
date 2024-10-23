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
namespace ocpp16
{
class MessagesConverter;
} // namespace ocpp16
} // namespace messages

namespace localcontroller
{

/** @brief Handler for central system requests */
class CentralSystemHandler
    : public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::CancelReservationReq,
                                                   ocpp::messages::ocpp16::CancelReservationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::ChangeAvailabilityReq,
                                                   ocpp::messages::ocpp16::ChangeAvailabilityConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::ChangeConfigurationReq,
                                                   ocpp::messages::ocpp16::ChangeConfigurationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::ClearCacheReq, ocpp::messages::ocpp16::ClearCacheConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::ClearChargingProfileReq,
                                                   ocpp::messages::ocpp16::ClearChargingProfileConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::DataTransferReq, ocpp::messages::ocpp16::DataTransferConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::GetCompositeScheduleReq,
                                                   ocpp::messages::ocpp16::GetCompositeScheduleConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::GetConfigurationReq,
                                                   ocpp::messages::ocpp16::GetConfigurationConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::GetDiagnosticsReq, ocpp::messages::ocpp16::GetDiagnosticsConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::GetLocalListVersionReq,
                                                   ocpp::messages::ocpp16::GetLocalListVersionConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::RemoteStartTransactionReq,
                                                   ocpp::messages::ocpp16::RemoteStartTransactionConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::RemoteStopTransactionReq,
                                                   ocpp::messages::ocpp16::RemoteStopTransactionConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::ReserveNowReq, ocpp::messages::ocpp16::ReserveNowConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::ResetReq, ocpp::messages::ocpp16::ResetConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::SendLocalListReq, ocpp::messages::ocpp16::SendLocalListConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::SetChargingProfileReq,
                                                   ocpp::messages::ocpp16::SetChargingProfileConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::TriggerMessageReq, ocpp::messages::ocpp16::TriggerMessageConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::UnlockConnectorReq, ocpp::messages::ocpp16::UnlockConnectorConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::UpdateFirmwareReq, ocpp::messages::ocpp16::UpdateFirmwareConf>,
      // Security extensions
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::CertificateSignedReq,
                                                   ocpp::messages::ocpp16::CertificateSignedConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::DeleteCertificateReq,
                                                   ocpp::messages::ocpp16::DeleteCertificateConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::ExtendedTriggerMessageReq,
                                                   ocpp::messages::ocpp16::ExtendedTriggerMessageConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::GetInstalledCertificateIdsReq,
                                                   ocpp::messages::ocpp16::GetInstalledCertificateIdsConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::GetLogReq, ocpp::messages::ocpp16::GetLogConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::InstallCertificateReq,
                                                   ocpp::messages::ocpp16::InstallCertificateConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::SignedUpdateFirmwareReq,
                                                   ocpp::messages::ocpp16::SignedUpdateFirmwareConf>
{
  public:
    /**
     * @brief Constructor
     * @param identifier Charge point's identifier
     * @param messages_converter Converter from/to OCPP to/from JSON messages
     * @param msg_dispatcher Message dispatcher
     * @param stack_config Stack configuration
     */
    CentralSystemHandler(const std::string&                               identifier,
                         const ocpp::messages::ocpp16::MessagesConverter& messages_converter,
                         ocpp::messages::MessageDispatcher&               msg_dispatcher);

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
    bool handleMessage(const ocpp::messages::ocpp16::CancelReservationReq& request,
                       ocpp::messages::ocpp16::CancelReservationConf&      response,
                       std::string&                                        error_code,
                       std::string&                                        error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::ChangeAvailabilityReq& request,
                       ocpp::messages::ocpp16::ChangeAvailabilityConf&      response,
                       std::string&                                         error_code,
                       std::string&                                         error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::ChangeConfigurationReq& request,
                       ocpp::messages::ocpp16::ChangeConfigurationConf&      response,
                       std::string&                                          error_code,
                       std::string&                                          error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::ClearCacheReq& request,
                       ocpp::messages::ocpp16::ClearCacheConf&      response,
                       std::string&                                 error_code,
                       std::string&                                 error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::ClearChargingProfileReq& request,
                       ocpp::messages::ocpp16::ClearChargingProfileConf&      response,
                       std::string&                                           error_code,
                       std::string&                                           error_message) override;

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
    bool handleMessage(const ocpp::messages::ocpp16::GetCompositeScheduleReq& request,
                       ocpp::messages::ocpp16::GetCompositeScheduleConf&      response,
                       std::string&                                           error_code,
                       std::string&                                           error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::GetConfigurationReq& request,
                       ocpp::messages::ocpp16::GetConfigurationConf&      response,
                       std::string&                                       error_code,
                       std::string&                                       error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::GetDiagnosticsReq& request,
                       ocpp::messages::ocpp16::GetDiagnosticsConf&      response,
                       std::string&                                     error_code,
                       std::string&                                     error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::GetLocalListVersionReq& request,
                       ocpp::messages::ocpp16::GetLocalListVersionConf&      response,
                       std::string&                                          error_code,
                       std::string&                                          error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::RemoteStartTransactionReq& request,
                       ocpp::messages::ocpp16::RemoteStartTransactionConf&      response,
                       std::string&                                             error_code,
                       std::string&                                             error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::RemoteStopTransactionReq& request,
                       ocpp::messages::ocpp16::RemoteStopTransactionConf&      response,
                       std::string&                                            error_code,
                       std::string&                                            error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::ReserveNowReq& request,
                       ocpp::messages::ocpp16::ReserveNowConf&      response,
                       std::string&                                 error_code,
                       std::string&                                 error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::ResetReq& request,
                       ocpp::messages::ocpp16::ResetConf&      response,
                       std::string&                            error_code,
                       std::string&                            error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::SendLocalListReq& request,
                       ocpp::messages::ocpp16::SendLocalListConf&      response,
                       std::string&                                    error_code,
                       std::string&                                    error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::SetChargingProfileReq& request,
                       ocpp::messages::ocpp16::SetChargingProfileConf&      response,
                       std::string&                                         error_code,
                       std::string&                                         error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::TriggerMessageReq& request,
                       ocpp::messages::ocpp16::TriggerMessageConf&      response,
                       std::string&                                     error_code,
                       std::string&                                     error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::UnlockConnectorReq& request,
                       ocpp::messages::ocpp16::UnlockConnectorConf&      response,
                       std::string&                                      error_code,
                       std::string&                                      error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::UpdateFirmwareReq& request,
                       ocpp::messages::ocpp16::UpdateFirmwareConf&      response,
                       std::string&                                     error_code,
                       std::string&                                     error_message) override;

    // Security extensions

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::CertificateSignedReq& request,
                       ocpp::messages::ocpp16::CertificateSignedConf&      response,
                       std::string&                                        error_code,
                       std::string&                                        error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::DeleteCertificateReq& request,
                       ocpp::messages::ocpp16::DeleteCertificateConf&      response,
                       std::string&                                        error_code,
                       std::string&                                        error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::ExtendedTriggerMessageReq& request,
                       ocpp::messages::ocpp16::ExtendedTriggerMessageConf&      response,
                       std::string&                                             error_code,
                       std::string&                                             error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::GetInstalledCertificateIdsReq& request,
                       ocpp::messages::ocpp16::GetInstalledCertificateIdsConf&      response,
                       std::string&                                                 error_code,
                       std::string&                                                 error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::GetLogReq& request,
                       ocpp::messages::ocpp16::GetLogConf&      response,
                       std::string&                             error_code,
                       std::string&                             error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::InstallCertificateReq& request,
                       ocpp::messages::ocpp16::InstallCertificateConf&      response,
                       std::string&                                         error_code,
                       std::string&                                         error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::SignedUpdateFirmwareReq& request,
                       ocpp::messages::ocpp16::SignedUpdateFirmwareConf&      response,
                       std::string&                                           error_code,
                       std::string&                                           error_message) override;

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
