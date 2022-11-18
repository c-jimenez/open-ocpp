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

#ifndef OPENOCPP_ISO15118MANAGER_H
#define OPENOCPP_ISO15118MANAGER_H

#include "Certificate.h"
#include "CertificateHashDataType.h"
#include "DataTransfer.h"
#include "GenericMessageSender.h"
#include "GenericMessagesConverter.h"
#include "IDataTransferManager.h"
#include "IdTokenInfoType.h"
#include "Logger.h"
#include "OcspRequestDataType.h"
#include "Timer.h"
#include "json.h"

#include <unordered_map>

namespace ocpp
{
// Forward declarations
namespace config
{
class IOcppConfig;
} // namespace config
namespace messages
{
struct CertificateSignedReq;
struct CertificateSignedConf;
struct DeleteCertificateReq;
struct DeleteCertificateConf;
struct Iso15118GetInstalledCertificateIdsReq;
struct Iso15118GetInstalledCertificateIdsConf;
struct Iso15118InstallCertificateReq;
struct Iso15118InstallCertificateConf;
struct Iso15118TriggerMessageReq;
struct Iso15118TriggerMessageConf;
} // namespace messages
namespace helpers
{
class WorkerThreadPool;
} // namespace helpers

// Main namespace
namespace chargepoint
{

class IAuthentManager;
class IChargePointEventsHandler;

/** @brief Handle charge point ISO15118 requests */
class Iso15118Manager : public IDataTransferManager::IDataTransferHandler
{
  public:
    /** @brief Constructor */
    Iso15118Manager(ocpp::config::IOcppConfig&                      ocpp_config,
                    IChargePointEventsHandler&                      events_handler,
                    ocpp::helpers::ITimerPool&                      timer_pool,
                    ocpp::helpers::WorkerThreadPool&                worker_pool,
                    const ocpp::messages::GenericMessagesConverter& messages_converter,
                    ocpp::messages::GenericMessageSender&           msg_sender,
                    IAuthentManager&                                authent_manager,
                    IDataTransferManager&                           datatransfer_manager);

    /** @brief Destructor */
    virtual ~Iso15118Manager();

    /**
     * @brief Authorize an ISO15118 transaction
     * @param certificate The X.509 certificated presented by EV
     * @param id_token This contains the identifier that needs to be authorized
     * @param cert_hash_data Contains the information needed to verify the EV Contract Certificate via OCSP
     * @param cert_status Certificate status information. - if all certificates are 
     *                    valid: return 'Accepted'. - if one of the certificates was revoked,
     *                    return 'CertificateRevoked
     * @return Authorization status (see AuthorizationStatus type)
    */
    ocpp::types::AuthorizationStatus authorize(const ocpp::x509::Certificate&                                          certificate,
                                               const std::string&                                                      id_token,
                                               const std::vector<ocpp::types::OcspRequestDataType>&                    cert_hash_data,
                                               ocpp::types::Optional<ocpp::types::AuthorizeCertificateStatusEnumType>& cert_status);
    /** 
     * @brief Get or update an ISO15118 EV certificate 
     * @param iso15118_schema_version Schema version currently used for the 15118 session between EV and Charge Point
     * @param action Defines whether certificate needs to be installed or updated
     * @param exi_request Raw CertificateInstallationReq request from EV, Base64 encoded
     * @param exi_response Raw CertificateInstallationRes response for the EV, Base64 encoded
     * @return Operation status (see Iso15118EVCertificateStatusEnumType enum) 
     */
    ocpp::types::Iso15118EVCertificateStatusEnumType get15118EVCertificate(const std::string&                     iso15118_schema_version,
                                                                           ocpp::types::CertificateActionEnumType action,
                                                                           const std::string&                     exi_request,
                                                                           std::string&                           exi_response);

    /**
     * @brief Get the status of an ISO15118 certificate
     * @param ocsp_request Indicates the certificate of which the status is requested
     * @param ocsp_result OCSPResponse class as defined in IETF RFC 6960. DER encoded (as defined in IETF RFC 6960), and then base64 encoded
     * @return Operation status (see GetCertificateStatusEnumType enum)
     */
    ocpp::types::GetCertificateStatusEnumType getCertificateStatus(const ocpp::types::OcspRequestDataType& ocsp_request,
                                                                   std::string&                            ocsp_result);

    /**
     * @brief Send a CSR request to sign an ISO15118 certificate
     * @param csr CSR request
     * @return true if the request has been sent and accepted, false otherwise
     */
    bool signCertificate(const ocpp::x509::CertificateRequest& csr);

    // IDataTransferManager::IDataTransferHandler interface

    /** @copydoc ocpp::types::DataTransferStatus IDataTransferHandler::onDataTransferRequest(const std::string&,
                                                                                             const std::string&,
                                                                                             const std::string&,
                                                                                             std::string&) */
    ocpp::types::DataTransferStatus onDataTransferRequest(const std::string& vendor_id,
                                                          const std::string& message_id,
                                                          const std::string& request_data,
                                                          std::string&       response_data) override;

  private:
    /** @brief Standard OCPP configuration */
    ocpp::config::IOcppConfig& m_ocpp_config;
    /** @brief User defined events handler */
    IChargePointEventsHandler& m_events_handler;
    /** @brief Worker thread pool */
    ocpp::helpers::WorkerThreadPool& m_worker_pool;
    /** @brief Messages converters */
    const ocpp::messages::GenericMessagesConverter& m_messages_converter;
    /** @brief Message sender */
    ocpp::messages::GenericMessageSender& m_msg_sender;
    /** @brief Authentication manager */
    IAuthentManager& m_authent_manager;
    /** @brief Last CSR request to sign */
    std::string m_last_csr;
    /** @brief Number of retries to sign a CSR request */
    unsigned int m_csr_sign_retries;
    /** @brief Timer for sign certificate operations */
    ocpp::helpers::Timer m_csr_timer;

    /** @brief Vendor id for ISO 15118 PnC extensions messages */
    static constexpr const char* ISO15118_VENDOR_ID = "org.openchargealliance.iso15118pnc";

    /**
     * @brief Generic ISO15118 request sender
     * @param type_id Type of message
     * @param action Action correspondin to the message
     * @param request Request to send
     * @param response Received response
     * @return true if the message has been sent and a response has been received, false otherwise
     */
    template <typename RequestType, typename ResponseType>
    bool send(const std::string& type_id, const std::string& action, const RequestType& request, ResponseType& response)
    {
        bool ret = false;

        // Get converters
        ocpp::messages::IMessageConverter<RequestType>*  req_converter  = m_messages_converter.getRequestConverter<RequestType>(type_id);
        ocpp::messages::IMessageConverter<ResponseType>* resp_converter = m_messages_converter.getResponseConverter<ResponseType>(type_id);

        // Prepare request
        ocpp::messages::DataTransferReq req;
        req.vendorId.assign(ISO15118_VENDOR_ID);
        req.messageId.value().assign(action);

        // Convert request to JSON
        rapidjson::Document                        json_req;
        rapidjson::StringBuffer                    buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        req_converter->toJson(request, json_req);
        json_req.Accept(writer);
        req.data.value() = buffer.GetString();

        // Send request
        ocpp::messages::DataTransferConf resp;
        if (m_msg_sender.call(ocpp::messages::DATA_TRANSFER_ACTION, req, resp) == ocpp::messages::CallResult::Ok)
        {
            if (resp.status == ocpp::types::DataTransferStatus::Accepted)
            {
                try
                {
                    // Parse JSON
                    rapidjson::Document json_resp;
                    json_resp.Parse(resp.data.value().c_str());
                    if (!json_resp.HasParseError())
                    {
                        // Convert response from JSON
                        std::string error_code;
                        std::string error_message;
                        ret = resp_converter->fromJson(json_resp, response, error_code, error_message);
                    }
                    else
                    {
                        LOG_ERROR << "[ISO15118] << " << action << " : Invalid JSON received";
                    }
                }
                catch (const std::exception&)
                {
                    LOG_ERROR << "[ISO15118] << " << action << " : Invalid JSON received";
                }
            }
            else
            {
                LOG_ERROR << "[ISO15118] Data transfer error : " << ocpp::types::DataTransferStatusHelper.toString(resp.status);
            }
        }

        return ret;
    }

    /**
     * @brief Generic ISO15118 request handler
     * @param type_id Type of message
     * @param request_data Data associated to the request
     * @param response_data Data associated with the response
     * @return Response status (see DataTransferStatus enum)
     */
    template <typename RequestType, typename ResponseType>
    ocpp::types::DataTransferStatus handle(const std::string& type_id, const std::string& request_data, std::string& response_data)
    {
        ocpp::types::DataTransferStatus                  status         = ocpp::types::DataTransferStatus::Rejected;
        ocpp::messages::IMessageConverter<RequestType>*  req_converter  = m_messages_converter.getRequestConverter<RequestType>(type_id);
        ocpp::messages::IMessageConverter<ResponseType>* resp_converter = m_messages_converter.getResponseConverter<ResponseType>(type_id);
        try
        {
            // Parse JSON
            rapidjson::Document request;
            request.Parse(request_data.c_str());
            if (!request.HasParseError())
            {
                // Convert request from JSON
                RequestType req;
                std::string error_code;
                std::string error_message;
                if (req_converter->fromJson(request, req, error_code, error_message))
                {
                    // Handle message
                    ResponseType resp;
                    handle(req, resp);

                    // Convert response to JSON
                    rapidjson::Document response;
                    response.Parse("{}");
                    if (resp_converter->toJson(resp, response))
                    {

                        // Serialize response
                        rapidjson::StringBuffer                    buffer;
                        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                        response.Accept(writer);
                        response_data = buffer.GetString();

                        status = ocpp::types::DataTransferStatus::Accepted;
                    }
                }
                else
                {
                    LOG_ERROR << "[ISO15118] << " << type_id << " : Invalid JSON received";
                }
            }
            else
            {
                LOG_ERROR << "[ISO15118] << " << type_id << " : Invalid JSON received";
            }
        }
        catch (const std::exception&)
        {
            LOG_ERROR << "[ISO15118] << " << type_id << " : Invalid JSON received";
        }
        return status;
    }

    /** @brief Handle a CertificateSigned request */
    void handle(const ocpp::messages::CertificateSignedReq& request, ocpp::messages::CertificateSignedConf& response);
    /** @brief Handle a DeleteCertificate request */
    void handle(const ocpp::messages::DeleteCertificateReq& request, ocpp::messages::DeleteCertificateConf& response);
    /** @brief Handle a Iso15118GetInstalledCertificateIds request */
    void handle(const ocpp::messages::Iso15118GetInstalledCertificateIdsReq& request,
                ocpp::messages::Iso15118GetInstalledCertificateIdsConf&      response);
    /** @brief Handle a InstallCertificate request */
    void handle(const ocpp::messages::Iso15118InstallCertificateReq& request, ocpp::messages::Iso15118InstallCertificateConf& response);
    /** @brief Handle a TriggerMessage request */
    void handle(const ocpp::messages::Iso15118TriggerMessageReq& request, ocpp::messages::Iso15118TriggerMessageConf& response);

    /** @brief Fill the hash information of a certificat */
    void fillHashInfo(const ocpp::x509::Certificate& certificate, ocpp::types::CertificateHashDataType& info);

    /** @brief Send a CSR request to sign an ISO15118 certificate */
    bool sendSignCertificate();
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_ISO15118MANAGER_H
