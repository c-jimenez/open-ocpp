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

#ifndef MAINTENANCEMANAGER_H
#define MAINTENANCEMANAGER_H

#include "Enums.h"
#include "GenericMessageHandler.h"
#include "GetDiagnostics.h"
#include "GetLog.h"
#include "ITriggerMessageManager.h"
#include "Reset.h"
#include "UnlockConnector.h"
#include "UpdateFirmware.h"

#include <thread>

namespace ocpp
{
// Forward declarations
namespace config
{
class IChargePointConfig;
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
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ResetReq, ocpp::messages::ResetConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::UnlockConnectorReq, ocpp::messages::UnlockConnectorConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::GetDiagnosticsReq, ocpp::messages::GetDiagnosticsConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::UpdateFirmwareReq, ocpp::messages::UpdateFirmwareConf>,
      // Security extensions
      public ocpp::messages::GenericMessageHandler<ocpp::messages::GetLogReq, ocpp::messages::GetLogConf>
{
  public:
    /** @brief Constructor */
    MaintenanceManager(const ocpp::config::IChargePointConfig&         stack_config,
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

    // ITriggerMessageManager::ITriggerMessageHandler interface

    /** @copydoc bool ITriggerMessageHandler::onTriggerMessage(ocpp::types::MessageTrigger, unsigned int) */
    bool onTriggerMessage(ocpp::types::MessageTrigger message, unsigned int connector_id) override;

    /** @copydoc bool ITriggerMessageHandler::onTriggerMessage(ocpp::types::MessageTriggerEnumType, unsigned int) */
    bool onTriggerMessage(ocpp::types::MessageTriggerEnumType message, unsigned int connector_id) override;

    // GenericMessageHandler interface

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ResetReq& request,
                       ocpp::messages::ResetConf&      response,
                       const char*&                    error_code,
                       std::string&                    error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::UnlockConnectorReq& request,
                       ocpp::messages::UnlockConnectorConf&      response,
                       const char*&                              error_code,
                       std::string&                              error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::GetDiagnosticsReq& request,
                       ocpp::messages::GetDiagnosticsConf&      response,
                       const char*&                             error_code,
                       std::string&                             error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::UpdateFirmwareReq& request,
                       ocpp::messages::UpdateFirmwareConf&      response,
                       const char*&                             error_code,
                       std::string&                             error_message) override;

    // Security extensions

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::GetLogReq& request,
                       ocpp::messages::GetLogConf&      response,
                       const char*&                     error_code,
                       std::string&                     error_message) override;

  private:
    /** @brief Stack configuration */
    const ocpp::config::IChargePointConfig& m_stack_config;
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
    ocpp::types::DiagnosticsStatus m_diagnostics_status;
    /** @brief Logs status */
    ocpp::types::UploadLogStatusEnumType m_logs_status;
    /** @brief Logs request id */
    ocpp::types::Optional<int> m_logs_request_id;

    /** @brief Firmware update thread */
    std::thread* m_firmware_thread;
    /** @brief Firmware update status */
    ocpp::types::FirmwareStatus m_firmware_status;

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
    void processGetLog(ocpp::types::LogEnumType            type,
                       std::string                         location,
                       ocpp::types::Optional<unsigned int> retries,
                       ocpp::types::Optional<unsigned int> retry_interval,
                       std::string                         local_log_file);

    /** @brief Send a log status notification */
    void sendLogStatusNotification();
};

} // namespace chargepoint
} // namespace ocpp

#endif // MAINTENANCEMANAGER_H
