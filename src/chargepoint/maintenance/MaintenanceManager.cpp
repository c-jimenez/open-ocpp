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
#include "Base64.h"
#include "Certificate.h"
#include "Connectors.h"
#include "DiagnosticsStatusNotification.h"
#include "FirmwareStatusNotification.h"
#include "GenericMessageSender.h"
#include "IChargePointConfig.h"
#include "IChargePointEventsHandler.h"
#include "IInternalConfigManager.h"
#include "InternalConfigKeys.h"
#include "LogStatusNotification.h"
#include "Logger.h"
#include "SecurityEvent.h"
#include "SecurityManager.h"
#include "SignedFirmwareStatusNotification.h"
#include "WorkerThreadPool.h"

#include <filesystem>

using namespace ocpp::types;
using namespace ocpp::messages;
using namespace ocpp::x509;

namespace ocpp
{
namespace chargepoint
{

/** @brief Constructor */
MaintenanceManager::MaintenanceManager(const ocpp::config::IChargePointConfig&         stack_config,
                                       ocpp::config::IInternalConfigManager&           internal_config,
                                       IChargePointEventsHandler&                      events_handler,
                                       ocpp::helpers::WorkerThreadPool&                worker_pool,
                                       const ocpp::messages::GenericMessagesConverter& messages_converter,
                                       ocpp::messages::IMessageDispatcher&             msg_dispatcher,
                                       ocpp::messages::GenericMessageSender&           msg_sender,
                                       Connectors&                                     connectors,
                                       ITriggerMessageManager&                         trigger_manager,
                                       ISecurityManager&                               security_manager)
    : GenericMessageHandler<ResetReq, ResetConf>(RESET_ACTION, messages_converter),
      GenericMessageHandler<UnlockConnectorReq, UnlockConnectorConf>(UNLOCK_CONNECTOR_ACTION, messages_converter),
      GenericMessageHandler<GetDiagnosticsReq, GetDiagnosticsConf>(GET_DIAGNOSTICS_ACTION, messages_converter),
      GenericMessageHandler<UpdateFirmwareReq, UpdateFirmwareConf>(UPDATE_FIRMWARE_ACTION, messages_converter),
      GenericMessageHandler<GetLogReq, GetLogConf>(GET_LOG_ACTION, messages_converter),
      GenericMessageHandler<SignedFirmwareUpdateReq, SignedFirmwareUpdateConf>(SIGNED_FIRMWARE_UPDATE_ACTION, messages_converter),
      m_stack_config(stack_config),
      m_internal_config(internal_config),
      m_events_handler(events_handler),
      m_worker_pool(worker_pool),
      m_msg_sender(msg_sender),
      m_connectors(connectors),
      m_security_manager(security_manager),
      m_diagnostics_thread(nullptr),
      m_diagnostics_status(DiagnosticsStatus::Idle),
      m_logs_status(UploadLogStatusEnumType::Idle),
      m_logs_request_id(),
      m_firmware_thread(nullptr),
      m_firmware_status(FirmwareStatus::Idle),
      m_signed_firmware_status(FirmwareStatusEnumType::Idle),
      m_firmware_request_id()
{
    msg_dispatcher.registerHandler(RESET_ACTION, *dynamic_cast<GenericMessageHandler<ResetReq, ResetConf>*>(this));
    msg_dispatcher.registerHandler(UNLOCK_CONNECTOR_ACTION,
                                   *dynamic_cast<GenericMessageHandler<UnlockConnectorReq, UnlockConnectorConf>*>(this));
    msg_dispatcher.registerHandler(GET_DIAGNOSTICS_ACTION,
                                   *dynamic_cast<GenericMessageHandler<GetDiagnosticsReq, GetDiagnosticsConf>*>(this));
    msg_dispatcher.registerHandler(UPDATE_FIRMWARE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<UpdateFirmwareReq, UpdateFirmwareConf>*>(this));
    msg_dispatcher.registerHandler(GET_LOG_ACTION, *dynamic_cast<GenericMessageHandler<GetLogReq, GetLogConf>*>(this));
    msg_dispatcher.registerHandler(SIGNED_FIRMWARE_UPDATE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<SignedFirmwareUpdateReq, SignedFirmwareUpdateConf>*>(this));
    trigger_manager.registerHandler(MessageTrigger::DiagnosticsStatusNotification, *this);
    trigger_manager.registerHandler(MessageTrigger::FirmwareStatusNotification, *this);
    trigger_manager.registerHandler(MessageTriggerEnumType::LogStatusNotification, *this);
    trigger_manager.registerHandler(MessageTriggerEnumType::FirmwareStatusNotification, *this);

    // Get current signed firmware update request id
    if (!m_internal_config.keyExist(SIGNED_FW_UPDATE_ID_KEY))
    {
        m_internal_config.createKey(SIGNED_FW_UPDATE_ID_KEY, "");
    }
    else
    {
        std::string request_id_str;
        if (m_internal_config.getKey(SIGNED_FW_UPDATE_ID_KEY, request_id_str))
        {
            if (!request_id_str.empty())
            {
                m_firmware_request_id = std::atoi(request_id_str.c_str());
            }
            LOG_DEBUG << "Signed firmare update request id : "
                      << (m_firmware_request_id.isSet() ? std::to_string(m_firmware_request_id) : "No signed firmware update in progress");
        }
        else
        {
            LOG_ERROR << "Unable to retrieve current signed firmare update request id";
        }
    }
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

/** @brief Notify the end of a signed firmware update operation */
bool MaintenanceManager::notifySignedFirmwareUpdateStatus(ocpp::types::FirmwareStatusEnumType status)
{
    // Update status
    m_signed_firmware_status = status;

    // Send status
    bool ret = sendSignedFirmwareStatusNotification();

    // Reset status
    m_signed_firmware_status = FirmwareStatusEnumType::Idle;
    m_firmware_request_id.clear();
    m_internal_config.setKey(SIGNED_FW_UPDATE_ID_KEY, "");

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

/** @copydoc bool ITriggerMessageHandler::onTriggerMessage(ocpp::types::MessageTriggerEnumType, unsigned int) */
bool MaintenanceManager::onTriggerMessage(ocpp::types::MessageTriggerEnumType message, unsigned int connector_id)
{
    bool ret = true;
    (void)connector_id;
    switch (message)
    {
        case MessageTriggerEnumType::LogStatusNotification:
        {
            m_worker_pool.run<void>(
                [this]
                {
                    // To let some time for the trigger message reply
                    std::this_thread::sleep_for(std::chrono::milliseconds(250u));
                    sendLogStatusNotification();
                });
        }
        break;

        case MessageTriggerEnumType::FirmwareStatusNotification:
        {
            m_worker_pool.run<void>(
                [this]
                {
                    // To let some time for the trigger message reply
                    std::this_thread::sleep_for(std::chrono::milliseconds(250u));
                    sendSignedFirmwareStatusNotification();
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

    LOG_INFO << "Firmare update requested : location = " << request.location << " - retrieveDate = " << request.retrieveDate.str();

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
    else
    {
        LOG_ERROR << "Firmware update already in progress";
    }

    return true;
}

// Security extensions

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
bool MaintenanceManager::handleMessage(const ocpp::messages::GetLogReq& request,
                                       ocpp::messages::GetLogConf&      response,
                                       const char*&                     error_code,
                                       std::string&                     error_message)
{
    (void)error_code;
    (void)error_message;

    // Prepare response
    response.status = LogStatusEnumType::Rejected;

    // Check if a request is already in progress
    if (!m_diagnostics_thread)
    {
        // Notify request
        std::string local_log_file = m_events_handler.getLog(request.logType, request.log.oldestTimestamp, request.log.latestTimestamp);
        if (!local_log_file.empty())
        {
            std::filesystem::path log_file(local_log_file);

            // Generate the log file
            if ((request.logType == LogEnumType::SecurityLog) && (m_stack_config.securityLogMaxEntriesCount() > 0))
            {
                log_file /= "security_logs.csv";
                LOG_INFO << "Generate security logs export : " << log_file;
                if (m_security_manager.exportSecurityEvents(log_file, request.log.oldestTimestamp, request.log.latestTimestamp))
                {
                    local_log_file = log_file;
                }
                else
                {
                    local_log_file = "";
                }
            }
            if (!local_log_file.empty())
            {
                // Extract filename for the response
                response.fileName.assign(log_file.filename().string());
                response.status = LogStatusEnumType::Accepted;

                // Create a separate thread since the operation can be time consuming
                m_logs_request_id    = request.requestId;
                m_diagnostics_thread = new std::thread(std::bind(&MaintenanceManager::processGetLog,
                                                                 this,
                                                                 request.logType,
                                                                 request.log.remoteLocation,
                                                                 request.retries,
                                                                 request.retryInterval,
                                                                 local_log_file));
                m_diagnostics_thread->detach();
            }
        }
        else
        {
            LOG_WARNING << "GetLog : No logs available";
        }
    }
    else
    {
        LOG_ERROR << "GetLog operation already in progress";
    }

    return true;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool MaintenanceManager::handleMessage(const ocpp::messages::SignedFirmwareUpdateReq& request,
                                       ocpp::messages::SignedFirmwareUpdateConf&      response,
                                       const char*&                                   error_code,
                                       std::string&                                   error_message)
{
    (void)error_code;
    (void)error_message;

    LOG_INFO << "Signed firmare update requested : location = " << request.firmware.location.str()
             << " - retrieveDate = " << request.firmware.retrieveDateTime.str() << " - signature = " << request.firmware.signature.str();

    // Prepare response
    response.status = UpdateFirmwareStatusEnumType::Rejected;

    // Check if a request is already in progress
    if (!m_firmware_thread)
    {
        // Check signing certificate
        std::time_t now = DateTime::now().timestamp();
        Certificate signing_certificate(request.firmware.signingCertificate);
        response.status = UpdateFirmwareStatusEnumType::InvalidCertificate;
        if (signing_certificate.isValid() && (signing_certificate.validityFrom() <= now) && (signing_certificate.validityTo() >= now) &&
            !signing_certificate.isSelfSigned())
        {
            // Check the signature of the signing certificate
            if (m_stack_config.internalCertificateManagementEnabled())
            {
                // Get the installed manufacter CAs to verify the signature of the certificat
                Certificate manufacturer_cas(m_security_manager.getCaCertificates(CertificateUseEnumType::ManufacturerRootCertificate));
                if (manufacturer_cas.isValid())
                {
                    // Check signature
                    if (signing_certificate.verify(manufacturer_cas.certificateChain()))
                    {
                        response.status = UpdateFirmwareStatusEnumType::Accepted;
                    }
                }
                else
                {
                    LOG_ERROR << "No valid Manufacturer CA certificates installed";
                }
            }
            else
            {
                // Check certificate signature
                response.status = m_events_handler.checkFirmwareSigningCertificate(signing_certificate);
            }
            if (response.status == UpdateFirmwareStatusEnumType::Accepted)
            {
                // Create a separate thread since the operation can be time consuming
                m_firmware_request_id = request.requestId;
                m_internal_config.setKey(SIGNED_FW_UPDATE_ID_KEY, std::to_string(request.requestId));
                m_firmware_thread = new std::thread(std::bind(&MaintenanceManager::processSignedFirmwareUpdate,
                                                              this,
                                                              request.firmware.location,
                                                              request.retries,
                                                              request.retryInterval,
                                                              request.firmware.retrieveDateTime,
                                                              request.firmware.installDateTime,
                                                              signing_certificate,
                                                              request.firmware.signature));
                m_firmware_thread->detach();
            }
        }
        if (response.status == UpdateFirmwareStatusEnumType::InvalidCertificate)
        {
            // Send a security event
            m_security_manager.logSecurityEvent(SECEVT_INVALID_FIRMWARE_SIGNING_CERT, "");
        }
    }
    else
    {
        LOG_ERROR << "Firmware update already in progress";
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

// Security extensions

/** @brief Process the upload of the logs */
void MaintenanceManager::processGetLog(ocpp::types::LogEnumType            type,
                                       std::string                         location,
                                       ocpp::types::Optional<unsigned int> retries,
                                       ocpp::types::Optional<unsigned int> retry_interval,
                                       std::string                         local_log_file)
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
    std::filesystem::path log_file(local_log_file);
    url += log_file.filename();

    LOG_INFO << "GetLog : type = " << LogEnumTypeHelper.toString(type) << " - URL = " << url << " - retries = " << nb_retries
             << " - retryInterval = " << retry_interval_s.count() << " - log file = " << local_log_file
             << " - requestId = " << (m_logs_request_id.isSet() ? std::to_string(m_logs_request_id.value()) : "not set");

    // Notify start of operation
    m_logs_status = UploadLogStatusEnumType::Uploading;
    sendLogStatusNotification();

    // Upload loop
    bool success = true;
    do
    {
        success = m_events_handler.uploadFile(local_log_file, url);
        if (!success)
        {
            // Next retry
            nb_retries--;
            if (nb_retries != 0)
            {
                LOG_WARNING << "GetLog : upload failed (" << nb_retries << " retrie(s) left - next retry in " << retry_interval_s.count()
                            << "s)";
                std::this_thread::sleep_for(retry_interval_s);
            }
        }

    } while (!success && (nb_retries != 0));

    // Notify end of operation
    if (success)
    {
        m_logs_status = UploadLogStatusEnumType::Uploaded;
        LOG_INFO << "GetLog : success";
    }
    else
    {
        m_logs_status = UploadLogStatusEnumType::UploadFailure;
        LOG_ERROR << "GetLog : failed";
    }
    sendLogStatusNotification();

    // Reset status
    m_logs_status = UploadLogStatusEnumType::Idle;
    m_logs_request_id.clear();

    // Release thread to allow new diagnostics requests
    delete m_diagnostics_thread;
    m_diagnostics_thread = nullptr;
}

/** @brief Send a log status notification */
void MaintenanceManager::sendLogStatusNotification()
{
    LOG_INFO << "GetLogs status : " << UploadLogStatusEnumTypeHelper.toString(m_logs_status);

    LogStatusNotificationReq status_req;
    LogStatusNotificationReq status_conf;
    status_req.status    = m_logs_status;
    status_req.requestId = m_logs_request_id;
    m_msg_sender.call(LOG_STATUS_NOTIFICATION_ACTION, status_req, status_conf);
}

/** @brief Process the signed firmware update */
void MaintenanceManager::processSignedFirmwareUpdate(std::string                                  location,
                                                     ocpp::types::Optional<unsigned int>          retries,
                                                     ocpp::types::Optional<unsigned int>          retry_interval,
                                                     ocpp::types::DateTime                        retrieve_date,
                                                     ocpp::types::Optional<ocpp::types::DateTime> install_date,
                                                     ocpp::x509::Certificate                      signing_certificate,
                                                     std::string                                  signature)
{
    // Check retrieve date
    if (retrieve_date > DateTime::now())
    {
        LOG_INFO << "SignedUpdateFirmware : Waiting until retrieve date";
        m_signed_firmware_status = FirmwareStatusEnumType::DownloadScheduled;
        sendSignedFirmwareStatusNotification();
        std::this_thread::sleep_until(std::chrono::system_clock::from_time_t(retrieve_date.timestamp()));
    }

    // Notify start of download
    std::string local_firmware_file = m_events_handler.updateFirmwareRequested();
    m_signed_firmware_status        = FirmwareStatusEnumType::Downloading;
    sendSignedFirmwareStatusNotification();

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
                LOG_WARNING << "SignedFirmwareUpdate : download failed (" << nb_retries << " retrie(s) left - next retry in "
                            << retry_interval_s.count() << "s)";
                std::this_thread::sleep_for(retry_interval_s);
            }
        }

    } while (!success && (nb_retries != 0));

    // Notify end of operation
    if (success)
    {
        m_signed_firmware_status = FirmwareStatusEnumType::Downloaded;
        LOG_INFO << "SignedFirmwareUpdate download : success";
    }
    else
    {
        m_signed_firmware_status = FirmwareStatusEnumType::DownloadFailed;
        LOG_ERROR << "SignedFirmwareUpdate download : failed";
    }
    sendSignedFirmwareStatusNotification();

    if (success)
    {
        // Verify signature
        std::vector<uint8_t> decoded_signature = base64::decode(signature);
        success                                = signing_certificate.verify(decoded_signature, local_firmware_file, Sha2::Type::SHA256);

        // Notify end of operation
        if (success)
        {
            m_signed_firmware_status = FirmwareStatusEnumType::SignatureVerified;
            LOG_INFO << "SignedFirmwareUpdate verify : success";
        }
        else
        {
            m_signed_firmware_status = FirmwareStatusEnumType::InvalidSignature;
            LOG_ERROR << "SignedFirmwareUpdate verify : failed";
        }
        sendSignedFirmwareStatusNotification();

        if (success)
        {
            // Check install date
            if (install_date.isSet() && (install_date.value() > DateTime::now()))
            {
                LOG_INFO << "SignedUpdateFirmware : Waiting until install date";
                m_signed_firmware_status = FirmwareStatusEnumType::InstallScheduled;
                sendSignedFirmwareStatusNotification();
                std::this_thread::sleep_until(std::chrono::system_clock::from_time_t(install_date.value().timestamp()));
            }

            // Notify that firmware is ready to be installed
            m_signed_firmware_status = FirmwareStatusEnumType::Installing;
            sendSignedFirmwareStatusNotification();
            m_events_handler.installFirmware(local_firmware_file);
        }
    }
    if (!success)
    {
        // Reset status
        m_signed_firmware_status = FirmwareStatusEnumType::Idle;
        m_firmware_request_id.clear();
        m_internal_config.setKey(SIGNED_FW_UPDATE_ID_KEY, "");
    }

    // Release thread to allow new firmware update requests
    delete m_firmware_thread;
    m_firmware_thread = nullptr;
}

/** @brief Send a signed firmware status notification */
bool MaintenanceManager::sendSignedFirmwareStatusNotification()
{
    LOG_INFO << "SignedFirmwareUpdate status : " << FirmwareStatusEnumTypeHelper.toString(m_signed_firmware_status);

    SignedFirmwareStatusNotificationReq  status_req;
    SignedFirmwareStatusNotificationConf status_conf;
    status_req.status    = m_signed_firmware_status;
    status_req.requestId = m_firmware_request_id;
    CallResult ret       = m_msg_sender.call(SIGNED_FIRMWARE_STATUS_NOTIFICATION_ACTION, status_req, status_conf);
    return (ret == CallResult::Ok);
}

} // namespace chargepoint
} // namespace ocpp
