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

#ifndef SECURITYMANAGER_H
#define SECURITYMANAGER_H

#include "CertificateSigned.h"
#include "DeleteCertificate.h"
#include "GenericMessageHandler.h"
#include "GetInstalledCertificateIds.h"
#include "ISecurityManager.h"
#include "ITriggerMessageManager.h"
#include "InstallCertificate.h"
#include "SecurityLogsDatabase.h"

namespace ocpp
{
// Forward declarations
namespace config
{
class IChargePointConfig;
} // namespace config
namespace messages
{
class IRequestFifo;
class GenericMessageSender;
struct SecurityEventNotificationReq;
} // namespace messages
namespace helpers
{
class WorkerThreadPool;
} // namespace helpers

// Main namespace
namespace chargepoint
{

class IChargePointEventsHandler;

/** @brief Handle security operations for the charge point */
class SecurityManager
    : public ISecurityManager,
      public ITriggerMessageManager::IExtendedTriggerMessageHandler,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::CertificateSignedReq, ocpp::messages::CertificateSignedConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::DeleteCertificateReq, ocpp::messages::DeleteCertificateConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::GetInstalledCertificateIdsReq,
                                                   ocpp::messages::GetInstalledCertificateIdsConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::InstallCertificateReq, ocpp::messages::InstallCertificateConf>
{
  public:
    /** @brief Constructor */
    SecurityManager(const ocpp::config::IChargePointConfig&         stack_config,
                    ocpp::database::Database&                       database,
                    IChargePointEventsHandler&                      events_handler,
                    ocpp::helpers::WorkerThreadPool&                worker_pool,
                    const ocpp::messages::GenericMessagesConverter& messages_converter,
                    ocpp::messages::IRequestFifo&                   requests_fifo);

    /** @brief Destructor */
    virtual ~SecurityManager();

    /** @brief Initialize the database table */
    void initDatabaseTable();

    /** @brief Start the security manager */
    bool start(ocpp::messages::GenericMessageSender& msg_sender,
               ocpp::messages::IMessageDispatcher&   msg_dispatcher,
               ITriggerMessageManager&               trigger_manager);

    /** @brief Stop the security manager */
    bool stop();

    /**
     * @brief Send a CSR request to sign a certificate
     * @param csr CSR request in PEM format
     * @return true if the request has been sent and accepted, false otherwise
     */
    bool signCertificate(const std::string& csr);

    // ISecurityManager interface

    /** @copydoc bool ISecurityManager::logSecurityEvent(const std::string&, const std::string&, bool) */
    bool logSecurityEvent(const std::string& type, const std::string& message, bool critical = false) override;

    /** @copydoc bool ISecurityManager::clearSecurityEvents() */
    bool clearSecurityEvents() override;

    /** bool ISecurityManager::exportSecurityEvents(const std::string&,
                                                    const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                    const ocpp::types::Optional<ocpp::types::DateTime>&) */
    bool exportSecurityEvents(const std::string&                                  filepath,
                              const ocpp::types::Optional<ocpp::types::DateTime>& start_time,
                              const ocpp::types::Optional<ocpp::types::DateTime>& stop_time) override;

    // ITriggerMessageManager::ITriggerMessageHandler interface

    /** @copydoc bool ITriggerMessageHandler::onTriggerMessage(ocpp::types::MessageTriggerEnumType, unsigned int) */
    bool onTriggerMessage(ocpp::types::MessageTriggerEnumType message, unsigned int connector_id) override;

    // GenericMessageHandler interface

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::CertificateSignedReq& request,
                       ocpp::messages::CertificateSignedConf&      response,
                       const char*&                                error_code,
                       std::string&                                error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::DeleteCertificateReq& request,
                       ocpp::messages::DeleteCertificateConf&      response,
                       const char*&                                error_code,
                       std::string&                                error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::GetInstalledCertificateIdsReq& request,
                       ocpp::messages::GetInstalledCertificateIdsConf&      response,
                       const char*&                                         error_code,
                       std::string&                                         error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::InstallCertificateReq& request,
                       ocpp::messages::InstallCertificateConf&      response,
                       const char*&                                 error_code,
                       std::string&                                 error_message) override;

  private:
    /** @brief User defined events handler */
    IChargePointEventsHandler& m_events_handler;
    /** @brief Worker thread pool */
    ocpp::helpers::WorkerThreadPool& m_worker_pool;
    /** @brief Transaction related requests FIFO */
    ocpp::messages::IRequestFifo& m_requests_fifo;
    /** @brief Message converter for SecurityEventNotificationReq */
    ocpp::messages::IMessageConverter<ocpp::messages::SecurityEventNotificationReq>& m_security_event_req_converter;

    /** @brief Security logs database */
    SecurityLogsDatabase m_security_logs_db;

    /** @brief Message sender */
    ocpp::messages::GenericMessageSender* m_msg_sender;
};

} // namespace chargepoint
} // namespace ocpp

#endif // SECURITYMANAGER_H
