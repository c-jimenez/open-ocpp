/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License version 2.1
as published by the Free Software Foundation.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPENOCPP_SECURITYMANAGER_H
#define OPENOCPP_SECURITYMANAGER_H

#include "CaCertificatesDatabase.h"
#include "CertificateSigned.h"
#include "CpCertificatesDatabase.h"
#include "DeleteCertificate.h"
#include "GenericMessageHandler.h"
#include "GetInstalledCertificateIds.h"
#include "IConfigManager.h"
#include "ISecurityManager.h"
#include "ITriggerMessageManager.h"
#include "InstallCertificate.h"
#include "SecurityLogsDatabase.h"

namespace ocpp
{
// Forward declarations
namespace config
{
class IOcppConfig;
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

class IChargePoint;
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
                    ocpp::config::IOcppConfig&                      ocpp_config,
                    ocpp::database::Database&                       database,
                    IChargePointEventsHandler&                      events_handler,
                    ocpp::helpers::WorkerThreadPool&                worker_pool,
                    const ocpp::messages::GenericMessagesConverter& messages_converter,
                    ocpp::messages::IRequestFifo&                   requests_fifo,
                    IChargePoint&                                   charge_point);

    /** @brief Destructor */
    virtual ~SecurityManager();

    /** @brief Initialize the database table */
    void initDatabaseTable();

    /** @brief Start the security manager */
    bool start(ocpp::messages::GenericMessageSender& msg_sender,
               ocpp::messages::IMessageDispatcher&   msg_dispatcher,
               ITriggerMessageManager&               trigger_manager,
               IConfigManager&                       config_manager);

    /** @brief Stop the security manager */
    bool stop();

    /**
     * @brief Send a CSR request to sign a certificate
     * @param csr CSR request
     * @return true if the request has been sent and accepted, false otherwise
     */
    bool signCertificate(const ocpp::x509::CertificateRequest& csr);

    /**
     * @brief Generate a new certificate request
     * @return true if the request has been sent and accepted, false otherwise
     */
    bool generateCertificateRequest();

    /**
     * @brief Get the installed Charge Point certificate as PEM encoded data
     * @param private_key Corresponding private key as PEM encoded data
     * @return Installed Charge Point certificate as PEM encoded data
     */
    std::string getChargePointCertificate(std::string& private_key);

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

    /** @copydoc std::string ISecurityManager::getCaCertificates(ocpp::types::CertificateUseEnumType) */
    std::string getCaCertificates(ocpp::types::CertificateUseEnumType type) override;

    // ITriggerMessageManager::ITriggerMessageHandler interface

    /** @copydoc bool ITriggerMessageHandler::onTriggerMessage(ocpp::types::MessageTriggerEnumType, const ocpp::types::Optional<unsigned int>&) */
    bool onTriggerMessage(ocpp::types::MessageTriggerEnumType message, const ocpp::types::Optional<unsigned int>& connector_id) override;

    // GenericMessageHandler interface

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
    bool handleMessage(const ocpp::messages::GetInstalledCertificateIdsReq& request,
                       ocpp::messages::GetInstalledCertificateIdsConf&      response,
                       std::string&                                         error_code,
                       std::string&                                         error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::InstallCertificateReq& request,
                       ocpp::messages::InstallCertificateConf&      response,
                       std::string&                                 error_code,
                       std::string&                                 error_message) override;

  private:
    /** @brief Stack configuration */
    const ocpp::config::IChargePointConfig& m_stack_config;
    /** @brief Standard OCPP configuration */
    ocpp::config::IOcppConfig& m_ocpp_config;
    /** @brief User defined events handler */
    IChargePointEventsHandler& m_events_handler;
    /** @brief Worker thread pool */
    ocpp::helpers::WorkerThreadPool& m_worker_pool;
    /** @brief Transaction related requests FIFO */
    ocpp::messages::IRequestFifo& m_requests_fifo;
    /** @brief Message converter for SecurityEventNotificationReq */
    std::unique_ptr<ocpp::messages::IMessageConverter<ocpp::messages::SecurityEventNotificationReq>> m_security_event_req_converter;
    /** @brief Charge Point */
    IChargePoint& m_charge_point;

    /** @brief Security logs database */
    SecurityLogsDatabase m_security_logs_db;
    /** @brief CA certificates database */
    CaCertificatesDatabase m_ca_certificates_db;
    /** @brief CP certificates database */
    CpCertificatesDatabase m_cp_certificates_db;

    /** @brief Message sender */
    ocpp::messages::GenericMessageSender* m_msg_sender;

    /** @brief Specific configuration check for parameter : AuthorizationKey */
    ocpp::types::ConfigurationStatus checkAuthorizationKeyParameter(const std::string& key, const std::string& value);
    /** @brief Specific configuration check for parameter : SecurityProfile */
    ocpp::types::ConfigurationStatus checkSecurityProfileParameter(const std::string& key, const std::string& value);

    /** @brief Fill the hash information of a certificat */
    void fillHashInfo(const ocpp::x509::Certificate& certificate, ocpp::types::CertificateHashDataType& info);
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_SECURITYMANAGER_H
