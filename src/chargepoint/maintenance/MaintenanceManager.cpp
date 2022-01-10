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

#include "MaintenanceManager.h"
#include "Connectors.h"
#include "DiagnosticsStatusNotification.h"
#include "FirmwareStatusNotification.h"
#include "GenericMessageSender.h"
#include "IChargePointEventsHandler.h"
#include "Logger.h"
#include "WorkerThreadPool.h"

#include <filesystem>

using namespace ocpp::types;
using namespace ocpp::messages;

namespace ocpp
{
namespace chargepoint
{

/** @brief Constructor */
MaintenanceManager::MaintenanceManager(IChargePointEventsHandler&                      events_handler,
                                       ocpp::helpers::WorkerThreadPool&                worker_pool,
                                       const ocpp::messages::GenericMessagesConverter& messages_converter,
                                       ocpp::messages::IMessageDispatcher&             msg_dispatcher,
                                       ocpp::messages::GenericMessageSender&           msg_sender,
                                       Connectors&                                     connectors,
                                       ITriggerMessageManager&                         trigger_manager)
    : GenericMessageHandler<ResetReq, ResetConf>(RESET_ACTION, messages_converter),
      GenericMessageHandler<UnlockConnectorReq, UnlockConnectorConf>(UNLOCK_CONNECTOR_ACTION, messages_converter),
      GenericMessageHandler<GetDiagnosticsReq, GetDiagnosticsConf>(GET_DIAGNOSTICS_ACTION, messages_converter),
      GenericMessageHandler<UpdateFirmwareReq, UpdateFirmwareConf>(UPDATE_FIRMWARE_ACTION, messages_converter),
      m_events_handler(events_handler),
      m_worker_pool(worker_pool),
      m_msg_sender(msg_sender),
      m_connectors(connectors),
      m_diagnostics_thread(nullptr),
      m_diagnostics_status(DiagnosticsStatus::Idle),
      m_firmware_thread(nullptr),
      m_firmware_status(FirmwareStatus::Idle)
{
    msg_dispatcher.registerHandler(RESET_ACTION, *dynamic_cast<GenericMessageHandler<ResetReq, ResetConf>*>(this));
    msg_dispatcher.registerHandler(UNLOCK_CONNECTOR_ACTION,
                                   *dynamic_cast<GenericMessageHandler<UnlockConnectorReq, UnlockConnectorConf>*>(this));
    msg_dispatcher.registerHandler(GET_DIAGNOSTICS_ACTION,
                                   *dynamic_cast<GenericMessageHandler<GetDiagnosticsReq, GetDiagnosticsConf>*>(this));
    msg_dispatcher.registerHandler(UPDATE_FIRMWARE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<UpdateFirmwareReq, UpdateFirmwareConf>*>(this));
    trigger_manager.registerHandler(MessageTrigger::DiagnosticsStatusNotification, *this);
    trigger_manager.registerHandler(MessageTrigger::FirmwareStatusNotification, *this);
}

/** @brief Destructor */
MaintenanceManager::~MaintenanceManager() { }

/**
     * @brief Notify the end of a firmware update operation
     * @param success Set to true if the firmware has been installed,
     *                otherwise set to false if the installation failed
     * @return true if the notification has been sent, false otherwise
     */
bool MaintenanceManager::notifyFirmwareUpdateStatus(bool success)
{
    // Update status
    if (success)
    {
        m_firmware_status = FirmwareStatus::Installed;
    }
    else
    {
        m_firmware_status = FirmwareStatus::InstallationFailed;
    }
    bool ret = sendFirmwareStatusNotification();

    // Reset status
    m_firmware_status = FirmwareStatus::Idle;

    return ret;
}

/** @copydoc bool ITriggerMessageHandler::onTriggerMessage(ocpp::types::MessageTrigger, unsigned int) */
bool MaintenanceManager::onTriggerMessage(ocpp::types::MessageTrigger message, unsigned int connector_id)
{
    bool ret = true;
    (void)connector_id;
    switch (message)
    {
        case MessageTrigger::DiagnosticsStatusNotification:
        {
            m_worker_pool.run<void>(
                [this]
                {
                    // To let some time for the trigger message reply
                    std::this_thread::sleep_for(std::chrono::milliseconds(250u));
                    sendDiagnosticStatusNotification();
                });
        }
        break;

        case MessageTrigger::FirmwareStatusNotification:
        {
            m_worker_pool.run<void>(
                [this]
                {
                    // To let some time for the trigger message reply
                    std::this_thread::sleep_for(std::chrono::milliseconds(250u));
                    sendFirmwareStatusNotification();
                });
        }
        break;

        default:
        {
            // Unknown message
            ret = false;
            break;
        }
    }
    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool MaintenanceManager::handleMessage(const ocpp::messages::ResetReq& request,
                                       ocpp::messages::ResetConf&      response,
                                       const char*&                    error_code,
                                       std::string&                    error_message)
{
    (void)error_code;
    (void)error_message;

    LOG_INFO << "Reset request received : type = " << ResetTypeHelper.toString(request.type);

    // Notify reset request
    if (m_events_handler.resetRequested(request.type))
    {
        response.status = ResetStatus::Accepted;
    }
    else
    {
        response.status = ResetStatus::Rejected;
    }

    LOG_INFO << "Reset request " << ResetStatusHelper.toString(response.status);

    return true;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool MaintenanceManager::handleMessage(const ocpp::messages::UnlockConnectorReq& request,
                                       ocpp::messages::UnlockConnectorConf&      response,
                                       const char*&                              error_code,
                                       std::string&                              error_message)
{
    (void)error_code;
    (void)error_message;

    LOG_INFO << "Unlock connector request received : connectorId = " << request.connectorId;

    // Check connector id
    if (m_connectors.isValid(request.connectorId))
    {
        // Notify reset request
        response.status = m_events_handler.unlockConnectorRequested(request.connectorId);
    }
    else
    {
        LOG_ERROR << "Unlock connector : unknown connector id";
        response.status = UnlockStatus::NotSupported;
    }

    LOG_INFO << "Unlock connector request " << UnlockStatusHelper.toString(response.status);

    return true;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool MaintenanceManager::handleMessage(const ocpp::messages::GetDiagnosticsReq& request,
                                       ocpp::messages::GetDiagnosticsConf&      response,
                                       const char*&                             error_code,
                                       std::string&                             error_message)
{
    (void)error_code;
    (void)error_message;

    // Check if a request is already in progress
    if (!m_diagnostics_thread)
    {
        // Notify request
        std::string local_diagnostic_file = m_events_handler.getDiagnostics(request.startTime, request.stopTime);
        if (!local_diagnostic_file.empty())
        {
            // Extract filename for the response
            std::filesystem::path diag_file(local_diagnostic_file);
            response.fileName.assign(diag_file.filename().string());

            // Create a separate thread since the operation can be time consuming
            m_diagnostics_thread = new std::thread(std::bind(&MaintenanceManager::processGetDiagnostics,
                                                             this,
                                                             request.location,
                                                             request.retries,
                                                             request.retryInterval,
                                                             local_diagnostic_file));
            m_diagnostics_thread->detach();
        }
        else
        {
            LOG_WARNING << "GetDiagnostics : No diagnostics available";
        }
    }
    else
    {
        LOG_ERROR << "GetDiagnostics operation already in progress";
    }

    return true;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool MaintenanceManager::handleMessage(const ocpp::messages::UpdateFirmwareReq& request,
                                       ocpp::messages::UpdateFirmwareConf&      response,
                                       const char*&                             error_code,
                                       std::string&                             error_message)
{
    (void)error_code;
    (void)error_message;
    (void)response;

    LOG_INFO << "Firmare update requested : location = " << request.location << request.retrieveDate
             << " - retrieveDate = " << request.retrieveDate.str();

    // Check if a request is already in progress
    if (!m_firmware_thread)
    {
        // Create a separate thread since the operation can be time consuming
        m_firmware_thread = new std::thread(std::bind(&MaintenanceManager::processUpdateFirmware,
                                                      this,
                                                      request.location,
                                                      request.retries,
                                                      request.retryInterval,
                                                      request.retrieveDate));
        m_firmware_thread->detach();
    }

    return true;
}

/** @brief Process the upload of the diagnostics */
void MaintenanceManager::processGetDiagnostics(std::string                         location,
                                               ocpp::types::Optional<unsigned int> retries,
                                               ocpp::types::Optional<unsigned int> retry_interval,
                                               std::string                         local_diagnostic_file)
{
    // Configure retries
    unsigned int nb_retries = 1u;
    if (retries.isSet())
    {
        nb_retries = retries;
    }
    std::chrono::seconds retry_interval_s(1u);
    if (retry_interval.isSet())
    {
        retry_interval_s = std::chrono::seconds(retry_interval.value());
    }

    // Compute URL
    std::string url = location;
    if (url.back() != '/')
    {
        url += "/";
    }
    std::filesystem::path diag_file(local_diagnostic_file);
    url += diag_file.filename();

    LOG_INFO << "GetDiagnostics : URL = " << url << " - retries = " << nb_retries << " - retryInterval = " << retry_interval_s.count()
             << " - diagnostic file = " << local_diagnostic_file;

    // Notify start of operation
    m_diagnostics_status = DiagnosticsStatus::Uploading;
    sendDiagnosticStatusNotification();

    // Upload loop
    bool success = true;
    do
    {
        success = m_events_handler.uploadFile(local_diagnostic_file, url);
        if (!success)
        {
            // Next retry
            nb_retries--;
            if (nb_retries != 0)
            {
                LOG_WARNING << "GetDiagnostics : upload failed (" << nb_retries << " retrie(s) left - next retry in "
                            << retry_interval_s.count() << "s)";
                std::this_thread::sleep_for(retry_interval_s);
            }
        }

    } while (!success && (nb_retries != 0));

    // Notify end of operation
    if (success)
    {
        m_diagnostics_status = DiagnosticsStatus::Uploaded;
        LOG_INFO << "GetDiagnostics : success";
    }
    else
    {
        m_diagnostics_status = DiagnosticsStatus::UploadFailed;
        LOG_ERROR << "GetDiagnostics : failed";
    }
    sendDiagnosticStatusNotification();

    // Reset status
    m_diagnostics_status = DiagnosticsStatus::Idle;

    // Release thread to allow new diagnostics requests
    delete m_diagnostics_thread;
    m_diagnostics_thread = nullptr;
}

/** @brief Send a diagnostic status notification */
void MaintenanceManager::sendDiagnosticStatusNotification()
{
    LOG_INFO << "GetDiagnostics status : " << DiagnosticsStatusHelper.toString(m_diagnostics_status);

    DiagnosticsStatusNotificationReq  status_req;
    DiagnosticsStatusNotificationConf status_conf;
    status_req.status = m_diagnostics_status;
    m_msg_sender.call(DIAGNOSTIC_STATUS_NOTIFICATION_ACTION, status_req, status_conf);
}

/** @brief Process the firmware update */
void MaintenanceManager::processUpdateFirmware(std::string                         location,
                                               ocpp::types::Optional<unsigned int> retries,
                                               ocpp::types::Optional<unsigned int> retry_interval,
                                               ocpp::types::DateTime               retrieve_date)
{
    // Check retrieve date
    if (retrieve_date > DateTime::now())
    {
        LOG_INFO << "UpdateFirmware : Waiting until retrieve date";
        std::this_thread::sleep_until(std::chrono::system_clock::from_time_t(retrieve_date.timestamp()));
    }

    // Notify start of download
    std::string local_firmware_file = m_events_handler.updateFirmwareRequested();
    m_firmware_status               = FirmwareStatus::Downloading;
    sendFirmwareStatusNotification();

    // Configure retries
    unsigned int nb_retries = 1u;
    if (retries.isSet())
    {
        nb_retries = retries;
    }
    std::chrono::seconds retry_interval_s(1u);
    if (retry_interval.isSet())
    {
        retry_interval_s = std::chrono::seconds(retry_interval.value());
    }

    // Download loop
    bool success = true;
    do
    {
        success = m_events_handler.downloadFile(location, local_firmware_file);
        if (!success)
        {
            // Next retry
            nb_retries--;
            if (nb_retries != 0)
            {
                LOG_WARNING << "FirmwareUpdate : download failed (" << nb_retries << " retrie(s) left - next retry in "
                            << retry_interval_s.count() << "s)";
                std::this_thread::sleep_for(retry_interval_s);
            }
        }

    } while (!success && (nb_retries != 0));

    // Notify end of operation
    if (success)
    {
        m_firmware_status = FirmwareStatus::Downloaded;
        LOG_INFO << "FirmwareUpdate download : success";
    }
    else
    {
        m_firmware_status = FirmwareStatus::DownloadFailed;
        LOG_ERROR << "FirmwareUpdate download : failed";
    }
    sendFirmwareStatusNotification();

    if (success)
    {
        // Notify that firmware is ready to be installed
        m_firmware_status = FirmwareStatus::Installing;
        sendFirmwareStatusNotification();
        m_events_handler.installFirmware(local_firmware_file);
    }
    else
    {
        // Reset status
        m_firmware_status = FirmwareStatus::Idle;
    }

    // Release thread to allow new firmware update requests
    delete m_firmware_thread;
    m_firmware_thread = nullptr;
}

/** @brief Send a firmware status notification */
bool MaintenanceManager::sendFirmwareStatusNotification()
{
    LOG_INFO << "FirmwareUpdate status : " << FirmwareStatusHelper.toString(m_firmware_status);

    FirmwareStatusNotificationReq  status_req;
    FirmwareStatusNotificationConf status_conf;
    status_req.status = m_firmware_status;
    CallResult ret    = m_msg_sender.call(FIRMWARE_STATUS_NOTIFICATION_ACTION, status_req, status_conf);
    return (ret == CallResult::Ok);
}

} // namespace chargepoint
} // namespace ocpp
