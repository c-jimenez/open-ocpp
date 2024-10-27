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

#include "SecurityManager.h"
#include "Certificate.h"
#include "CertificateRequest.h"
#include "GenericMessageSender.h"
#include "IChargePoint.h"
#include "IChargePointConfig.h"
#include "IChargePointEventsHandler.h"
#include "IOcppConfig.h"
#include "Logger.h"
#include "MessagesConverter.h"
#include "PrivateKey.h"
#include "SecurityEvent.h"
#include "SecurityEventNotification.h"
#include "Sha2.h"
#include "SignCertificate.h"
#include "WorkerThreadPool.h"

#include <unordered_map>

using namespace ocpp::messages;
using namespace ocpp::messages::ocpp16;
using namespace ocpp::types;
using namespace ocpp::types::ocpp16;
using namespace ocpp::x509;

namespace ocpp
{
namespace chargepoint
{

/** @brief Criticity for each standard OCPP security event */
static const std::unordered_map<std::string, bool> s_security_events = {{SECEVT_FIRMWARE_UPDATED, true},
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

/** @brief Elliptic curves for certificate request generation */
static const std::unordered_map<std::string, unsigned int> s_ec_curves = {
    {"prime256v1", static_cast<unsigned int>(PrivateKey::Curve::PRIME256_V1)},
    {"secp256k1", static_cast<unsigned int>(PrivateKey::Curve::SECP256_K1)},
    {"secp384r1", static_cast<unsigned int>(PrivateKey::Curve::SECP384_R1)},
    {"secp521r1", static_cast<unsigned int>(PrivateKey::Curve::SECP521_R1)},
    {"brainpoolP256t1", static_cast<unsigned int>(PrivateKey::Curve::BRAINPOOLP256_T1)},
    {"brainpoolP384t1", static_cast<unsigned int>(PrivateKey::Curve::BRAINPOOLP384_T1)},
    {"brainpoolP512t1", static_cast<unsigned int>(PrivateKey::Curve::BRAINPOOLP512_T1)}};

/** @brief Secure hashes for certificate request generation */
static const std::unordered_map<std::string, Sha2::Type> s_shas = {
    {"sha256", Sha2::Type::SHA256}, {"sha384", Sha2::Type::SHA384}, {"sha512", Sha2::Type::SHA512}};

/** @brief Constructor */
SecurityManager::SecurityManager(const ocpp::config::IChargePointConfig&         stack_config,
                                 ocpp::config::IOcppConfig&                      ocpp_config,
                                 ocpp::database::Database&                       database,
                                 IChargePointEventsHandler&                      events_handler,
                                 ocpp::helpers::WorkerThreadPool&                worker_pool,
                                 const ocpp::messages::GenericMessagesConverter& messages_converter,
                                 ocpp::messages::IRequestFifo&                   requests_fifo,
                                 IChargePoint&                                   charge_point)
    : GenericMessageHandler<CertificateSignedReq, CertificateSignedConf>(CERTIFICATE_SIGNED_ACTION, messages_converter),
      GenericMessageHandler<DeleteCertificateReq, DeleteCertificateConf>(DELETE_CERTIFICATE_ACTION, messages_converter),
      GenericMessageHandler<GetInstalledCertificateIdsReq, GetInstalledCertificateIdsConf>(GET_INSTALLED_CERTIFICATE_IDS_ACTION,
                                                                                           messages_converter),
      GenericMessageHandler<InstallCertificateReq, InstallCertificateConf>(INSTALL_CERTIFICATE_ACTION, messages_converter),
      m_stack_config(stack_config),
      m_ocpp_config(ocpp_config),
      m_events_handler(events_handler),
      m_worker_pool(worker_pool),
      m_requests_fifo(requests_fifo),
      m_security_event_req_converter(
          messages_converter.getRequestConverter<SecurityEventNotificationReq>(SECURITY_EVENT_NOTIFICATION_ACTION)),
      m_charge_point(charge_point),
      m_security_logs_db(stack_config, database),
      m_ca_certificates_db(stack_config, database),
      m_cp_certificates_db(stack_config, database),
      m_msg_sender(nullptr)
{
}

/** @brief Destructor */
SecurityManager::~SecurityManager() { }

/** @brief Initialize the database table */
void SecurityManager::initDatabaseTable()
{
    m_security_logs_db.initDatabaseTable();
    m_ca_certificates_db.initDatabaseTable();
    m_cp_certificates_db.initDatabaseTable();
}

/** @brief Start the security manager */
bool SecurityManager::start(ocpp::messages::GenericMessageSender& msg_sender,
                            ocpp::messages::IMessageDispatcher&   msg_dispatcher,
                            ITriggerMessageManager&               trigger_manager,
                            IConfigManager&                       config_manager)
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

        // Register specific configuration checks
        config_manager.registerCheckFunction(
            "AuthorizationKey",
            std::bind(&SecurityManager::checkAuthorizationKeyParameter, this, std::placeholders::_1, std::placeholders::_2));
        config_manager.registerCheckFunction(
            "SecurityProfile",
            std::bind(&SecurityManager::checkSecurityProfileParameter, this, std::placeholders::_1, std::placeholders::_2));

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
bool SecurityManager::signCertificate(const ocpp::x509::CertificateRequest& csr)
{
    bool ret = false;

    LOG_INFO << "Sign certificate : valid = " << csr.isValid() << " - subject = " << csr.subjectString();

    // Check validity
    if (csr.isValid())
    {
        // Send request
        if (m_msg_sender)
        {
            SignCertificateReq sign_certificate_req;
            sign_certificate_req.csr.assign(csr.pem());

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
    }
    else
    {
        LOG_ERROR << "Invalid certificate request";
    }

    return ret;
}

/** @brief Generate a new certificate request */
bool SecurityManager::generateCertificateRequest()
{
    LOG_INFO << "Generating new certificate request";

    // Generate a private key
    unsigned int     param    = 0;
    PrivateKey::Type key_type = PrivateKey::Type::EC;
    if (m_stack_config.clientCertificateRequestKeyType() == "rsa")
    {
        key_type = PrivateKey::Type::RSA;
        param    = m_stack_config.clientCertificateRequestRsaKeyLength();
        if (param < 2048u)
        {
            param = 2048u;
        }
    }
    else
    {
        std::string curve      = m_stack_config.clientCertificateRequestEcCurve();
        const auto& iter_curve = s_ec_curves.find(curve);
        if (iter_curve != s_ec_curves.end())
        {
            param = iter_curve->second;
        }
        else
        {
            param = s_ec_curves.at("prime256v1");
        }
    }
    std::string passphrase = m_stack_config.tlsClientCertificatePrivateKeyPassphrase();
    PrivateKey  private_key(key_type, param, passphrase);

    // Generate a certificate request
    CertificateRequest::Subject subject;
    subject.country           = m_stack_config.clientCertificateRequestSubjectCountry();
    subject.state             = m_stack_config.clientCertificateRequestSubjectState();
    subject.location          = m_stack_config.clientCertificateRequestSubjectLocation();
    subject.organization      = m_ocpp_config.cpoName();
    subject.organization_unit = m_stack_config.clientCertificateRequestSubjectOrganizationUnit();
    subject.common_name       = m_stack_config.chargePointSerialNumber();
    subject.email_address     = m_stack_config.clientCertificateRequestSubjectEmail();
    Sha2::Type  sha_type      = Sha2::Type::SHA256;
    std::string sha           = m_stack_config.clientCertificateRequestHashType();
    const auto& iter_sha      = s_shas.find(sha);
    if (iter_sha != s_shas.end())
    {
        sha_type = iter_sha->second;
    }
    CertificateRequest certificate_request(subject, private_key, sha_type);

    // Save request into database
    m_cp_certificates_db.saveCertificateRequest(certificate_request, private_key);

    // Send to Central System
    return signCertificate(certificate_request);
}

/** @brief Get the installed Charge Point certificate as PEM encoded data */
std::string SecurityManager::getChargePointCertificate(std::string& private_key)
{
    return m_cp_certificates_db.getChargePointCertificate(private_key);
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

        if (m_stack_config.securityEventNotificationEnabled())
        {
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
                rapidjson::Document payload(rapidjson::kObjectType);
                m_security_event_req_converter->setAllocator(&payload.GetAllocator());
                if (m_security_event_req_converter->toJson(request, payload))
                {
                    m_requests_fifo.push(0, SECURITY_EVENT_NOTIFICATION_ACTION, payload);
                }
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

/** @copydoc std::string ISecurityManager::getCaCertificates(ocpp::types::ocpp16::CertificateUseEnumType) */
std::string SecurityManager::getCaCertificates(ocpp::types::ocpp16::CertificateUseEnumType type)
{
    return m_ca_certificates_db.getCertificateListPem(type);
}

// ITriggerMessageManager::ITriggerMessageHandler interface

/** @copydoc bool ITriggerMessageHandler::onTriggerMessage(ocpp::types::ocpp16::MessageTriggerEnumType, const ocpp::types::Optional<unsigned int>&) */
bool SecurityManager::onTriggerMessage(ocpp::types::ocpp16::MessageTriggerEnumType message,
                                       const ocpp::types::Optional<unsigned int>&  connector_id)
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

                std::string csr_pem;
                if (m_stack_config.internalCertificateManagementEnabled())
                {
                    // Generate and send CSR
                    generateCertificateRequest();
                }
                else
                {
                    // Notify application to generate a CSR
                    m_events_handler.generateCsr(csr_pem);

                    // Create request
                    CertificateRequest csr(csr_pem);

                    // Send the request
                    signCertificate(csr);
                }
            });

        ret = true;
    }

    return ret;
}

// GenericMessageHandler interface

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool SecurityManager::handleMessage(const ocpp::messages::ocpp16::CertificateSignedReq& request,
                                    ocpp::messages::ocpp16::CertificateSignedConf&      response,
                                    std::string&                                        error_code,
                                    std::string&                                        error_message)
{
    bool ret                 = true;
    bool send_security_event = true;

    (void)error_code;
    (void)error_message;

    LOG_INFO << "Certificate signed message received : certificate size = " << request.certificateChain.size();

    // Prepare response
    response.status = CertificateSignedStatusEnumType::Rejected;

    // Check certificate's size
    if (request.certificateChain.size() < m_ocpp_config.certificateSignedMaxChainSize())
    {
        // Check certificate's validity
        Certificate certificate(request.certificateChain);
        if (certificate.isValid() && certificate.verify())
        {
            if (m_stack_config.internalCertificateManagementEnabled())
            {
                // Get the corresponding certificate request
                unsigned    request_id              = 0;
                std::string certificate_request_pem = m_cp_certificates_db.getCertificateRequest(request_id);
                if (!certificate_request_pem.empty())
                {
                    // Check if the certificate matches the request
                    CertificateRequest certificate_request(certificate_request_pem);
                    if (certificate_request.isValid() && (certificate_request.publicKey() == certificate.publicKey()))
                    {
                        // Install certificate
                        if (m_cp_certificates_db.installCertificate(request_id, certificate))
                        {

                            // Reconnect using the new certificate
                            if (certificate.validityFrom() <= DateTime::now().timestamp())
                            {
                                m_charge_point.reconnect();
                            }

                            response.status     = CertificateSignedStatusEnumType::Accepted;
                            send_security_event = false;
                        }
                    }
                    else
                    {
                        LOG_ERROR << "No matching certificate request found";
                    }
                }
                else
                {
                    LOG_ERROR << "No certificate request found";
                }
            }
            else
            {
                // Notify new certificate
                if (m_events_handler.chargePointCertificateReceived(certificate))
                {
                    response.status     = CertificateSignedStatusEnumType::Accepted;
                    send_security_event = false;
                }
            }
        }
    }
    if (send_security_event)
    {
        logSecurityEvent(SECEVT_INVALID_CHARGE_POINT_CERT, "");
    }

    LOG_INFO << "Certificate signed message : " << CertificateSignedStatusEnumTypeHelper.toString(response.status);

    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool SecurityManager::handleMessage(const ocpp::messages::ocpp16::DeleteCertificateReq& request,
                                    ocpp::messages::ocpp16::DeleteCertificateConf&      response,
                                    std::string&                                        error_code,
                                    std::string&                                        error_message)
{
    bool ret = true;

    (void)error_code;
    (void)error_message;

    LOG_INFO << "Delete certificate request received : hashAlgorithm = "
             << HashAlgorithmEnumTypeHelper.toString(request.certificateHashData.hashAlgorithm)
             << " - issuerKeyHash = " << request.certificateHashData.issuerKeyHash.str()
             << " - issuerNameHash = " << request.certificateHashData.issuerNameHash.str()
             << " - serialNumber = " << request.certificateHashData.serialNumber.str();

    if (m_stack_config.internalCertificateManagementEnabled())
    {
        // Delete certificate
        response.status = m_ca_certificates_db.deleteCertificate(request.certificateHashData);
    }
    else
    {
        // Notify handler to delete the certificate
        response.status = m_events_handler.deleteCertificate(request.certificateHashData.hashAlgorithm,
                                                             request.certificateHashData.issuerNameHash,
                                                             request.certificateHashData.issuerKeyHash,
                                                             request.certificateHashData.serialNumber);
    }

    LOG_INFO << "Delete certificate : " << DeleteCertificateStatusEnumTypeHelper.toString(response.status);

    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool SecurityManager::handleMessage(const ocpp::messages::ocpp16::GetInstalledCertificateIdsReq& request,
                                    ocpp::messages::ocpp16::GetInstalledCertificateIdsConf&      response,
                                    std::string&                                                 error_code,
                                    std::string&                                                 error_message)
{
    bool ret = true;

    (void)error_code;
    (void)error_message;

    LOG_INFO << "Get installed certificate ids request received : certificateType = "
             << CertificateUseEnumTypeHelper.toString(request.certificateType);

    // Prepare response
    response.status = GetInstalledCertificateStatusEnumType::NotFound;

    if (m_stack_config.internalCertificateManagementEnabled())
    {
        // Get the list of installed certificates
        m_ca_certificates_db.getCertificateList(request.certificateType, response.certificateHashData);
        if (!response.certificateHashData.empty())
        {
            response.status = GetInstalledCertificateStatusEnumType::Accepted;
        }
    }
    else
    {
        // Notify handler to get the list of installed certificates
        std::vector<Certificate> certificates;
        m_events_handler.getInstalledCertificates(request.certificateType, certificates);
        if (!certificates.empty())
        {
            // Compute hashes for each certificate
            for (const auto& certificate : certificates)
            {
                if (certificate.isValid())
                {
                    response.certificateHashData.emplace_back();
                    CertificateHashDataType& hash_data = response.certificateHashData.back();
                    fillHashInfo(certificate, hash_data);
                }
            }
            if (!response.certificateHashData.empty())
            {
                response.status = GetInstalledCertificateStatusEnumType::Accepted;
            }
        }
    }

    LOG_INFO << "Get installed certificate ids : status = " << GetInstalledCertificateStatusEnumTypeHelper.toString(response.status)
             << " - count = " << response.certificateHashData.size();

    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool SecurityManager::handleMessage(const ocpp::messages::ocpp16::InstallCertificateReq& request,
                                    ocpp::messages::ocpp16::InstallCertificateConf&      response,
                                    std::string&                                         error_code,
                                    std::string&                                         error_message)
{
    bool ret = true;

    (void)error_code;
    (void)error_message;

    LOG_INFO << "Install certificate request received : certificateType = "
             << CertificateUseEnumTypeHelper.toString(request.certificateType) << " - certificate size = " << request.certificate.size();

    // Prepare response
    response.status = CertificateStatusEnumType::Rejected;

    // Check certificate
    Certificate certificate(request.certificate.str());
    if (certificate.isValid())
    {
        if (m_stack_config.internalCertificateManagementEnabled())
        {
            // Check the number of installed certificate
            unsigned int certificates_count =
                m_ca_certificates_db.getCertificateCount(CertificateUseEnumType::CentralSystemRootCertificate, false) +
                m_ca_certificates_db.getCertificateCount(CertificateUseEnumType::ManufacturerRootCertificate, false);
            if (certificates_count < m_ocpp_config.certificateStoreMaxLength())
            {
                // Additionnal checks
                if ((request.certificateType == CertificateUseEnumType::ManufacturerRootCertificate) ||
                    !m_ocpp_config.additionalRootCertificateCheck())
                {
                    // Create hash of certificate
                    CertificateHashDataType hash_data;
                    fillHashInfo(certificate, hash_data);

                    // Install certificate
                    if (m_ca_certificates_db.addCertificate(request.certificateType, certificate, hash_data))
                    {
                        response.status = CertificateStatusEnumType::Accepted;

                        // Reconnect using the new certificate
                        if ((request.certificateType == CertificateUseEnumType::CentralSystemRootCertificate) &&
                            (certificate.validityFrom() <= DateTime::now().timestamp()))
                        {
                            m_charge_point.reconnect();
                        }
                    }
                    else
                    {
                        response.status = CertificateStatusEnumType::Failed;
                    }
                }
            }
        }
        else
        {
            // Notify new certificate
            response.status = m_events_handler.caCertificateReceived(request.certificateType, certificate);
        }
    }

    LOG_INFO << "Install certificate : " << CertificateStatusEnumTypeHelper.toString(response.status);

    return ret;
}

/** @brief Specific configuration check for parameter : AuthorizationKey */
ocpp::types::ocpp16::ConfigurationStatus SecurityManager::checkAuthorizationKeyParameter(const std::string& key, const std::string& value)
{
    (void)key;
    ConfigurationStatus ret = ConfigurationStatus::Rejected;

    // Authorization key length for security profiles 1 and 2 must be between 16 and 40 bytes
    // and must be a valid hexadecimal representation
    if ((value.size() >= 16u) && (value.size() <= 40u))
    {
        auto key_bytes = ocpp::helpers::fromHexString(value);
        if (key_bytes.size() == (value.size() / 2u))
        {
            ret = ConfigurationStatus::Accepted;
        }
    }

    return ret;
}

/** @brief Specific configuration check for parameter : SecurityProfile */
ocpp::types::ocpp16::ConfigurationStatus SecurityManager::checkSecurityProfileParameter(const std::string& key, const std::string& value)
{
    (void)key;
    ConfigurationStatus ret = ConfigurationStatus::Rejected;

    // Do not allow to decrease security profile
    unsigned int security_profile     = m_ocpp_config.securityProfile();
    unsigned int new_security_profile = static_cast<unsigned int>(std::atoi(value.c_str()));
    if (new_security_profile > security_profile)
    {
        // Check if new security profile requirements are met
        switch (new_security_profile)
        {
            case 1:
            {
                // Basic authent
                // AuthorizationKey value must no be empty
                if (!m_ocpp_config.authorizationKey().empty())
                {
                    ret = ConfigurationStatus::Accepted;
                }
            }
            break;

            case 2:
            {
                // Basic authent + TLS (server authentication only)
                // AuthorizationKey value must no be empty
                // A Central System root certificate must be installed
                if (!m_ocpp_config.authorizationKey().empty())
                {
                    if (m_stack_config.internalCertificateManagementEnabled())
                    {
                        if (m_ca_certificates_db.getCertificateCount(CertificateUseEnumType::CentralSystemRootCertificate, true) > 0)
                        {
                            ret = ConfigurationStatus::Accepted;
                        }
                    }
                    else
                    {
                        if (m_events_handler.hasCentralSystemCaCertificateInstalled())
                        {
                            ret = ConfigurationStatus::Accepted;
                        }
                    }
                }
            }
            break;

            case 3:
            {
                // TLS with server and client authentication
                // A Central System root certificate must be installed
                // A valid Charge Point certificate must be installed
                if (m_stack_config.internalCertificateManagementEnabled())
                {
                    if ((m_ca_certificates_db.getCertificateCount(CertificateUseEnumType::CentralSystemRootCertificate, true) > 0) &&
                        m_cp_certificates_db.isValidCertificateInstalled())
                    {
                        ret = ConfigurationStatus::Accepted;
                    }
                }
                else
                {
                    if (m_events_handler.hasCentralSystemCaCertificateInstalled() && m_events_handler.hasChargePointCertificateInstalled())
                    {
                        ret = ConfigurationStatus::Accepted;
                    }
                }
            }
            break;

            default:
            {
                // Invalid security profile
                break;
            }
        }
    }

    return ret;
}

/** @brief Fill the hash information of a certificat */
void SecurityManager::fillHashInfo(const ocpp::x509::Certificate& certificate, ocpp::types::ocpp16::CertificateHashDataType& info)
{
    // Compute hashes with SHA-256 algorithm
    Sha2 sha256;
    info.hashAlgorithm = HashAlgorithmEnumType::SHA256;
    sha256.compute(certificate.issuerDer().data(), certificate.issuerDer().size());
    info.issuerNameHash.assign(sha256.resultString());
    sha256.compute(&certificate.publicKey()[0], certificate.publicKey().size());
    info.issuerKeyHash.assign(sha256.resultString());
    info.serialNumber.assign(certificate.serialNumberHexString());
}

} // namespace chargepoint
} // namespace ocpp
