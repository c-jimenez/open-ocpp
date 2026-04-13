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

#ifndef OPENOCPP_MAINTENANCEMANAGER_H
#define OPENOCPP_MAINTENANCEMANAGER_H

#include "Enums.h"
#include "GenericMessageHandler.h"
#include "GetDiagnostics.h"
#include "GetLog.h"
#include "ITriggerMessageManager.h"
#include "Reset.h"
#include "SignedUpdateFirmware.h"
#include "UnlockConnector.h"
#include "UpdateFirmware.h"

#include <thread>

namespace ocpp
{
// Forward declarations
namespace config
{
class IChargePointConfig;
class IInternalConfigManager;
} // namespace config
namespace messages
{
class IMessageDispatcher;
class GenericMessagesConverter;
class GenericMessageSender;
} // namespace messages
namespace helpers
{
class WorkerThreadPool;
} // namespace helpers
namespace x509
{
class Certificate;
} // namespace x509

// Main namespace
namespace chargepoint
{

class Connectors;
class ISecurityManager;
class IChargePointEventsHandler;

/** @brief Handle maintenance requests for the charge point */
class MaintenanceManager
    : public ITriggerMessageManager::ITriggerMessageHandler,
      public ITriggerMessageManager::IExtendedTriggerMessageHandler,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::ResetReq, ocpp::messages::ocpp16::ResetConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::UnlockConnectorReq, ocpp::messages::ocpp16::UnlockConnectorConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::GetDiagnosticsReq, ocpp::messages::ocpp16::GetDiagnosticsConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::UpdateFirmwareReq, ocpp::messages::ocpp16::UpdateFirmwareConf>,
      // Security extensions
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::GetLogReq, ocpp::messages::ocpp16::GetLogConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::SignedUpdateFirmwareReq,
                                                   ocpp::messages::ocpp16::SignedUpdateFirmwareConf>
{
  public:
    /** @brief Constructor */
    MaintenanceManager(const ocpp::config::IChargePointConfig&         stack_config,
                       ocpp::config::IInternalConfigManager&           internal_config,
                       IChargePointEventsHandler&                      events_handler,
                       ocpp::helpers::WorkerThreadPool&                worker_pool,
                       const ocpp::messages::GenericMessagesConverter& messages_converter,
                       ocpp::messages::IMessageDispatcher&             msg_dispatcher,
                       ocpp::messages::GenericMessageSender&           msg_sender,
                       Connectors&                                     connectors,
                       ITriggerMessageManager&                         trigger_manager,
                       ISecurityManager&                               security_manager);

    /** @brief Destructor */
    virtual ~MaintenanceManager();

    /**
     * @brief Notify the end of a firmware update operation
     * @param success Set to true if the firmware has been installed,
     *                otherwise set to false if the installation failed
     * @return true if the notification has been sent, false otherwise
     */
    bool notifyFirmwareUpdateStatus(bool success);

    /**
     * @brief Notify the end of a signed firmware update operation
     * @param status Installation status (see FirmwareStatusEnumType documentation)
     * @return true if the notification has been sent, false otherwise
     */
    bool notifySignedUpdateFirmwareStatus(ocpp::types::ocpp16::FirmwareStatusEnumType status);

    // ITriggerMessageManager::ITriggerMessageHandler interface

    /** @copydoc bool ITriggerMessageHandler::onTriggerMessage(ocpp::types::ocpp16::MessageTrigger, const ocpp::types::Optional<unsigned int>&) */
    bool onTriggerMessage(ocpp::types::ocpp16::MessageTrigger message, const ocpp::types::Optional<unsigned int>& connector_id) override;

    /** @copydoc bool ITriggerMessageHandler::onTriggerMessage(ocpp::types::ocpp16::MessageTriggerEnumType, const ocpp::types::Optional<unsigned int>&) */
    bool onTriggerMessage(ocpp::types::ocpp16::MessageTriggerEnumType message,
                          const ocpp::types::Optional<unsigned int>&  connector_id) override;

    // GenericMessageHandler interface

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
    bool handleMessage(const ocpp::messages::ocpp16::UnlockConnectorReq& request,
                       ocpp::messages::ocpp16::UnlockConnectorConf&      response,
                       std::string&                                      error_code,
                       std::string&                                      error_message) override;

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
    bool handleMessage(const ocpp::messages::ocpp16::GetLogReq& request,
                       ocpp::messages::ocpp16::GetLogConf&      response,
                       std::string&                             error_code,
                       std::string&                             error_message) override;

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
    /** @brief Stack configuration */
    const ocpp::config::IChargePointConfig& m_stack_config;
    /** @brief Charge point's internal configuration */
    ocpp::config::IInternalConfigManager& m_internal_config;
    /** @brief User defined events handler */
    IChargePointEventsHandler& m_events_handler;
    /** @brief Worker thread pool */
    ocpp::helpers::WorkerThreadPool& m_worker_pool;
    /** @brief Message sender */
    ocpp::messages::GenericMessageSender& m_msg_sender;
    /** @brief Connectors */
    Connectors& m_connectors;
    /** @brief Security manager */
    ISecurityManager& m_security_manager;

    /** @brief Diagnostics thread */
    std::thread* m_diagnostics_thread;
    /** @brief Diagnostics status */
    ocpp::types::ocpp16::DiagnosticsStatus m_diagnostics_status;
    /** @brief Logs status */
    ocpp::types::ocpp16::UploadLogStatusEnumType m_logs_status;
    /** @brief Logs request id */
    ocpp::types::Optional<int> m_logs_request_id;

    /** @brief Firmware update thread */
    std::thread* m_firmware_thread;
    /** @brief Firmware update status */
    ocpp::types::ocpp16::FirmwareStatus m_firmware_status;
    /** @brief Signed firmware update status */
    ocpp::types::ocpp16::FirmwareStatusEnumType m_signed_firmware_status;
    /** @brief Signed firmware update request id */
    ocpp::types::Optional<int> m_firmware_request_id;

    /** @brief Process the upload of the diagnostics */
    void processGetDiagnostics(std::string                         location,
                               ocpp::types::Optional<unsigned int> retries,
                               ocpp::types::Optional<unsigned int> retry_interval,
                               std::string                         local_diagnostic_file);

    /** @brief Send a diagnostic status notification */
    void sendDiagnosticStatusNotification();

    /** @brief Process the firmware update */
    void processUpdateFirmware(std::string                         location,
                               ocpp::types::Optional<unsigned int> retries,
                               ocpp::types::Optional<unsigned int> retry_interval,
                               ocpp::types::DateTime               retrieve_date);

    /** @brief Send a firmware status notification */
    bool sendFirmwareStatusNotification();

    // Security extensions

    /** @brief Process the upload of the logs */
    void processGetLog(ocpp::types::ocpp16::LogEnumType    type,
                       std::string                         location,
                       ocpp::types::Optional<unsigned int> retries,
                       ocpp::types::Optional<unsigned int> retry_interval,
                       std::string                         local_log_file);

    /** @brief Send a log status notification */
    void sendLogStatusNotification();

    /** @brief Process the signed firmware update */
    void processSignedUpdateFirmware(std::string                                  location,
                                     ocpp::types::Optional<unsigned int>          retries,
                                     ocpp::types::Optional<unsigned int>          retry_interval,
                                     ocpp::types::DateTime                        retrieve_date,
                                     ocpp::types::Optional<ocpp::types::DateTime> install_date,
                                     ocpp::x509::Certificate                      signing_certificate,
                                     std::string                                  signature);

    /** @brief Send a signed firmware status notification */
    bool sendSignedFirmwareStatusNotification();
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_MAINTENANCEMANAGER_H
