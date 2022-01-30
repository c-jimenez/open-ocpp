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

#include "SecurityManager.h"
#include "GenericMessageSender.h"
#include "IChargePointConfig.h"
#include "IChargePointEventsHandler.h"
#include "Logger.h"
#include "MessagesConverter.h"
#include "SecurityEvent.h"
#include "SecurityEventNotification.h"
#include "SignCertificate.h"
#include "WorkerThreadPool.h"

#include <map>

using namespace ocpp::types;
using namespace ocpp::messages;

namespace ocpp
{
namespace chargepoint
{

/** @brief Criticity for each standard OCPP security event */
static const std::map<std::string, bool> s_security_events = {{SECEVT_FIRMWARE_UPDATED, true},
                                                              {SECEVT_FAILED_AUTHENT_AT_CENTRAL_SYSTEM, false},
                                                              {SECEVT_CENTRAL_SYSTEM_FAILED_TO_AUTHENT, false},
                                                              {SECEVT_SETTING_SYSTEM_TIME, true},
                                                              {SECEVT_STARTUP_OF_DEVICE, true},
                                                              {SECEVT_RESET_REBOOT, true},
                                                              {SECEVT_SECURITY_LOG_CLEARED, true},
                                                              {SECEVT_RECONFIG_SECURITY_PARAMETER, false},
                                                              {SECEVT_MEMORY_EXHAUSTION, true},
                                                              {SECEVT_INVALID_MESSAGES, false},
                                                              {SECEVT_ATTEMPTED_REPLAY_ATTACKS, false},
                                                              {SECEVT_TAMPER_DETECTION_ACTIVATED, true},
                                                              {SECEVT_INVALID_FIRMWARE_SIGNATURE, false},
                                                              {SECEVT_INVALID_FIRMWARE_SIGNING_CERT, false},
                                                              {SECEVT_INVALID_CENTRAL_SYSTEM_CERT, false},
                                                              {SECEVT_INVALID_CHARGE_POINT_CERT, false},
                                                              {SECEVT_INVALID_TLS_VERSION, false},
                                                              {SECEVT_INVALID_TLS_CIPHER_SUITE, false}};

/** @brief Constructor */
SecurityManager::SecurityManager(const ocpp::config::IChargePointConfig&         stack_config,
                                 ocpp::database::Database&                       database,
                                 IChargePointEventsHandler&                      events_handler,
                                 ocpp::helpers::WorkerThreadPool&                worker_pool,
                                 const ocpp::messages::GenericMessagesConverter& messages_converter,
                                 ocpp::messages::IRequestFifo&                   requests_fifo)
    : GenericMessageHandler<CertificateSignedReq, CertificateSignedConf>(CERTIFICATE_SIGNED_ACTION, messages_converter),
      GenericMessageHandler<DeleteCertificateReq, DeleteCertificateConf>(DELETE_CERTIFICATE_ACTION, messages_converter),
      GenericMessageHandler<GetInstalledCertificateIdsReq, GetInstalledCertificateIdsConf>(GET_INSTALLED_CERTIFICATE_IDS_ACTION,
                                                                                           messages_converter),
      GenericMessageHandler<InstallCertificateReq, InstallCertificateConf>(INSTALL_CERTIFICATE_ACTION, messages_converter),
      m_events_handler(events_handler),
      m_worker_pool(worker_pool),
      m_requests_fifo(requests_fifo),
      m_security_event_req_converter(
          *messages_converter.getRequestConverter<SecurityEventNotificationReq>(SECURITY_EVENT_NOTIFICATION_ACTION)),
      m_security_logs_db(stack_config, database),
      m_msg_sender(nullptr)
{
}

/** @brief Destructor */
SecurityManager::~SecurityManager() { }

/** @brief Initialize the database table */
void SecurityManager::initDatabaseTable()
{
    m_security_logs_db.initDatabaseTable();
}

/** @brief Start the security manager */
bool SecurityManager::start(ocpp::messages::GenericMessageSender& msg_sender,
                            ocpp::messages::IMessageDispatcher&   msg_dispatcher,
                            ITriggerMessageManager&               trigger_manager)
{
    bool ret = false;

    // Check if already started
    if (!m_msg_sender)
    {
        // Register to Central System requests
        m_msg_sender = &msg_sender;
        msg_dispatcher.registerHandler(CERTIFICATE_SIGNED_ACTION,
                                       *dynamic_cast<GenericMessageHandler<CertificateSignedReq, CertificateSignedConf>*>(this));
        msg_dispatcher.registerHandler(DELETE_CERTIFICATE_ACTION,
                                       *dynamic_cast<GenericMessageHandler<DeleteCertificateReq, DeleteCertificateConf>*>(this));
        msg_dispatcher.registerHandler(
            GET_INSTALLED_CERTIFICATE_IDS_ACTION,
            *dynamic_cast<GenericMessageHandler<GetInstalledCertificateIdsReq, GetInstalledCertificateIdsConf>*>(this));
        msg_dispatcher.registerHandler(INSTALL_CERTIFICATE_ACTION,
                                       *dynamic_cast<GenericMessageHandler<InstallCertificateReq, InstallCertificateConf>*>(this));

        // Register to trigger messages
        trigger_manager.registerHandler(MessageTriggerEnumType::SignChargePointCertificate, *this);

        ret = true;
    }

    return ret;
}

/** @brief Stop the security manager */
bool SecurityManager::stop()
{
    bool ret = false;

    // Check if already started
    if (m_msg_sender)
    {
        // Reset communication objects
        m_msg_sender = nullptr;

        ret = true;
    }

    return ret;
}

/** @brief Send a CSR request to sign a certificate */
bool SecurityManager::signCertificate(const std::string& csr)
{
    bool ret = false;

    LOG_INFO << "Sign certificate : csr size = " << csr.size();

    if (m_msg_sender)
    {
        SignCertificateReq sign_certificate_req;
        sign_certificate_req.csr.assign(csr);

        SignCertificateConf sign_certificate_conf;
        if (m_msg_sender->call(SIGN_CERTIFICATE_ACTION, sign_certificate_req, sign_certificate_conf) == CallResult::Ok)
        {
            LOG_INFO << "Sign certificate : " << GenericStatusEnumTypeHelper.toString(sign_certificate_conf.status);
            ret = true;
        }
    }
    else
    {
        LOG_ERROR << "Stack is not started";
    }

    return ret;
}

// ISecurityManager interface

/** @copydoc bool ISecurityManager::logSecurityEvent(const std::string&, const std::string&, bool) */
bool SecurityManager::logSecurityEvent(const std::string& type, const std::string& message, bool critical)
{
    bool ret = true;

    // Generate timestamp
    DateTime timestamp = DateTime::now();

    // Send notification for critical events only
    const auto& iter_type = s_security_events.find(type);
    if (iter_type != s_security_events.cend())
    {
        critical = iter_type->second;
    }
    if (critical)
    {
        LOG_WARNING << "Security Event : type = " << type << ", message = " << message;

        SecurityEventNotificationReq request;
        request.type.assign(type);
        request.timestamp = timestamp;
        if (!message.empty())
        {
            request.techInfo.value().assign(message);
        }

        if (m_msg_sender)
        {
            // Stack is started, try to send the notification
            SecurityEventNotificationConf response;
            if (m_msg_sender->call(SECURITY_EVENT_NOTIFICATION_ACTION, request, response, &m_requests_fifo) == CallResult::Failed)
            {
                ret = false;
            }
        }
        else
        {
            // Stack is not started, queue the notification
            rapidjson::Document payload;
            payload.Parse("{}");
            m_security_event_req_converter.setAllocator(&payload.GetAllocator());
            if (m_security_event_req_converter.toJson(request, payload))
            {
                m_requests_fifo.push(0, SECURITY_EVENT_NOTIFICATION_ACTION, payload);
            }
        }
    }
    else
    {
        LOG_INFO << "Security Event : type = " << type << ", message = " << message;
    }

    // Store event if logs are enabled
    ret = m_security_logs_db.log(type, message, critical, timestamp) && ret;

    return ret;
}

/** @copydoc bool ISecurityManager::clearSecurityEvents() */
bool SecurityManager::clearSecurityEvents()
{
    bool ret = m_security_logs_db.clear();
    if (ret)
    {
        LOG_INFO << "Security logs cleared";
        logSecurityEvent(SECEVT_SECURITY_LOG_CLEARED, "");
    }
    return ret;
}

/** bool ISecurityManager::exportSecurityEvents(const std::string&,
                                                const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                const ocpp::types::Optional<ocpp::types::DateTime>&) */
bool SecurityManager::exportSecurityEvents(const std::string&                                  filepath,
                                           const ocpp::types::Optional<ocpp::types::DateTime>& start_time,
                                           const ocpp::types::Optional<ocpp::types::DateTime>& stop_time)
{
    return m_security_logs_db.exportSecurityEvents(filepath, start_time, stop_time);
}

// ITriggerMessageManager::ITriggerMessageHandler interface

/** @copydoc bool ITriggerMessageHandler::onTriggerMessage(ocpp::types::MessageTriggerEnumType, unsigned int) */
bool SecurityManager::onTriggerMessage(ocpp::types::MessageTriggerEnumType message, unsigned int connector_id)
{
    bool ret = false;
    (void)connector_id;

    if (message == MessageTriggerEnumType::SignChargePointCertificate)
    {
        m_worker_pool.run<void>(
            [this]
            {
                // To let some time for the trigger message reply
                std::this_thread::sleep_for(std::chrono::milliseconds(100u));

                // Notify application to generate a CSR
                std::string csr;
                m_events_handler.generateCsr(csr);

                // Send the request
                signCertificate(csr);
            });

        ret = true;
    }

    return ret;
}

// GenericMessageHandler interface

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool SecurityManager::handleMessage(const ocpp::messages::CertificateSignedReq& request,
                                    ocpp::messages::CertificateSignedConf&      response,
                                    const char*&                                error_code,
                                    std::string&                                error_message)
{
    bool ret = true;

    LOG_INFO << "Certificate signed message received : certificate size = " << request.certificateChain.size();

    (void)response;
    (void)error_code;
    (void)error_message;

    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool SecurityManager::handleMessage(const ocpp::messages::DeleteCertificateReq& request,
                                    ocpp::messages::DeleteCertificateConf&      response,
                                    const char*&                                error_code,
                                    std::string&                                error_message)
{
    bool ret = true;

    LOG_INFO << "Delete certificate request received : hashAlgorithm = "
             << HashAlgorithmEnumTypeHelper.toString(request.certificateHashData.hashAlgorithm)
             << " - issuerKeyHash = " << request.certificateHashData.issuerKeyHash.str()
             << " - issuerNameHash = " << request.certificateHashData.issuerNameHash.str();

    (void)response;
    (void)error_code;
    (void)error_message;

    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool SecurityManager::handleMessage(const ocpp::messages::GetInstalledCertificateIdsReq& request,
                                    ocpp::messages::GetInstalledCertificateIdsConf&      response,
                                    const char*&                                         error_code,
                                    std::string&                                         error_message)
{
    bool ret = true;

    LOG_INFO << "Get installed certificate ids request received : certificateType = "
             << CertificateUseEnumTypeHelper.toString(request.certificateType);

    (void)response;
    (void)error_code;
    (void)error_message;

    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool SecurityManager::handleMessage(const ocpp::messages::InstallCertificateReq& request,
                                    ocpp::messages::InstallCertificateConf&      response,
                                    const char*&                                 error_code,
                                    std::string&                                 error_message)
{
    bool ret = true;

    LOG_INFO << "Install certificate request received : certificateType = "
             << CertificateUseEnumTypeHelper.toString(request.certificateType) << " - certificate size = " << request.certificate.size();

    (void)response;
    (void)error_code;
    (void)error_message;

    return ret;
}

} // namespace chargepoint
} // namespace ocpp