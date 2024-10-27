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

#ifndef OPENOCPP_CS_CHARGEPOINTPROXY_H
#define OPENOCPP_CS_CHARGEPOINTPROXY_H

#include "ChargePointHandler.h"
#include "GenericMessageSender.h"
#include "ICentralSystem.h"
#include "Iso15118.h"
#include "Logger.h"
#include "MessageDispatcher.h"
#include "MessagesConverter.h"
#include "MessagesValidator.h"
#include "RpcServer.h"

#include <memory>

namespace ocpp
{
namespace centralsystem
{

/** @brief Charge point proxy */
class ChargePointProxy : public ICentralSystem::IChargePoint, public ocpp::rpc::IRpc::IListener, public ocpp::rpc::IRpc::ISpy
{
  public:
    /**
     * @brief Constructor
     * @param central_system Central System instance associated to the charge point
     * @param identifier Charge point's identifier
     * @param rpc RPC connection with the charge point
     * @param messages_validator JSON schemas needed to validate payloads
     * @param messages_converter Converter from/to OCPP to/from JSON messages
     * @param stack_config Stack configuration
     */
    ChargePointProxy(ICentralSystem&                                  central_system,
                     const std::string&                               identifier,
                     std::shared_ptr<ocpp::rpc::RpcServer::Client>    rpc,
                     const ocpp::messages::ocpp16::MessagesValidator& messages_validator,
                     ocpp::messages::ocpp16::MessagesConverter&       messages_converter,
                     const ocpp::config::ICentralSystemConfig&        stack_config);
    /** @brief Destructor */
    virtual ~ChargePointProxy();

    /** @brief Unregister to the IRpc::ISpy interface messages */
    void unregisterFromRpcSpy();

    // ICentralSystem::IChargePoint interface

    /** @copydoc ICentralSystem&& ICentralSystem::IChargePoint::centralSystem() */
    ICentralSystem& centralSystem() override { return m_central_system; }

    /** @copydoc const std::string& ICentralSystem::IChargePoint::ipAddress() const */
    const std::string& ipAddress() const override;

    /** @copydoc const std::string& ICentralSystem::IChargePoint::identifier() const */
    const std::string& identifier() const override { return m_identifier; }

    /** @copydoc void ICentralSystem::IChargePoint::setTimeout(std::chrono::milliseconds) */
    void setTimeout(std::chrono::milliseconds timeout) override;

    /** @copydoc void ICentralSystem::IChargePoint::disconnect() */
    void disconnect() override;

    /** @copydoc void ICentralSystem::IChargePoint::registerHandler(IChargePointRequestHandler&) */
    void registerHandler(IChargePointRequestHandler& handler) override;

    // OCPP operations

    /** @copydoc bool ICentralSystem::IChargePoint::cancelReservation(int) */
    bool cancelReservation(int reservation_id) override;

    /** @copydoc ocpp::types::ocpp16::AvailabilityStatus ICentralSystem::IChargePoint::changeAvailability(unsigned int, ocpp::types::ocpp16::AvailabilityType) */
    ocpp::types::ocpp16::AvailabilityStatus changeAvailability(unsigned int                          connector_id,
                                                               ocpp::types::ocpp16::AvailabilityType availability) override;

    /** @copydoc ocpp::types::ocpp16::ConfigurationStatus ICentralSystem::IChargePoint::changeConfiguration(const std::string&, const std::string&) */
    ocpp::types::ocpp16::ConfigurationStatus changeConfiguration(const std::string& key, const std::string& value) override;

    /** @copydoc bool ICentralSystem::IChargePoint::clearCache() */
    bool clearCache() override;

    /** @copydoc bool ICentralSystem::IChargePoint::clearChargingProfile(const ocpp::types::Optional<int>&,
                                                                         const ocpp::types::Optional<unsigned int>&,
                                                                         const ocpp::types::Optional<ocpp::types::ocpp16::ChargingProfilePurposeType>&,
                                                                         const ocpp::types::Optional<unsigned int>&) */
    bool clearChargingProfile(const ocpp::types::Optional<int>&                                             profile_id,
                              const ocpp::types::Optional<unsigned int>&                                    connector_id,
                              const ocpp::types::Optional<ocpp::types::ocpp16::ChargingProfilePurposeType>& purpose,
                              const ocpp::types::Optional<unsigned int>&                                    stack_level) override;

    /** @copydoc bool ICentralSystem::IChargePoint::dataTransfer(const std::string&,
                                                                 const std::string&,
                                                                 const std::string&,
                                                                 ocpp::types::ocpp16::DataTransferStatus&,
                                                                 std::string&) */
    bool dataTransfer(const std::string&                       vendor_id,
                      const std::string&                       message_id,
                      const std::string&                       request_data,
                      ocpp::types::ocpp16::DataTransferStatus& status,
                      std::string&                             response_data) override;

    /** @copydoc bool ICentralSystem::IChargePoint::getCompositeSchedule(unsigned int,
                                                                         std::chrono::seconds,
                                                                         const ocpp::types::Optional<ocpp::types::ocpp16::ChargingRateUnitType>&,
                                                                         ocpp::types::Optional<unsigned int>&,
                                                                         ocpp::types::Optional<ocpp::types::DateTime>&,
                                                                         ocpp::types::Optional<ocpp::types::ocpp16::ChargingSchedule>&) */
    bool getCompositeSchedule(unsigned int                                                            connector_id,
                              std::chrono::seconds                                                    duration,
                              const ocpp::types::Optional<ocpp::types::ocpp16::ChargingRateUnitType>& unit,
                              ocpp::types::Optional<unsigned int>&                                    schedule_connector_id,
                              ocpp::types::Optional<ocpp::types::DateTime>&                           schedule_start,
                              ocpp::types::Optional<ocpp::types::ocpp16::ChargingSchedule>&           schedule) override;

    /** @copydoc bool ICentralSystem::IChargePoint::getConfiguration(const std::vector<std::string>&,
                                                                     std::vector<ocpp::types::ocpp16::KeyValue>&,
                                                                     std::vector<std::string>&) */
    bool getConfiguration(const std::vector<std::string>&             keys,
                          std::vector<ocpp::types::ocpp16::KeyValue>& config_keys,
                          std::vector<std::string>&                   unknown_keys) override;

    /** @copydoc bool ICentralSystem::IChargePoint::getDiagnostics(const std::string&,
                                                                   const ocpp::types::Optional<unsigned int>&,
                                                                   const ocpp::types::Optional<std::chrono::seconds>&,
                                                                   const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                                   const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                                   std::string&) */
    bool getDiagnostics(const std::string&                                  uri,
                        const ocpp::types::Optional<unsigned int>&          retries,
                        const ocpp::types::Optional<std::chrono::seconds>&  retry_interval,
                        const ocpp::types::Optional<ocpp::types::DateTime>& start,
                        const ocpp::types::Optional<ocpp::types::DateTime>& stop,
                        std::string&                                        diagnotic_filename) override;

    /** @copydoc bool ICentralSystem::IChargePoint::getLocalListVersion(int&) */
    bool getLocalListVersion(int& version) override;

    /** @copydoc bool ICentralSystem::IChargePoint::remoteStartTransaction(const ocpp::types::Optional<unsigned int>&,
                                                                           const std::string&,
                                                                           const ocpp::types::Optional<ocpp::types::ocpp16::ChargingProfile>&) */
    bool remoteStartTransaction(const ocpp::types::Optional<unsigned int>&                         connector_id,
                                const std::string&                                                 id_tag,
                                const ocpp::types::Optional<ocpp::types::ocpp16::ChargingProfile>& profile) override;

    /** @copydoc bool ICentralSystem::IChargePoint::remoteStopTransaction(int) */
    bool remoteStopTransaction(int transaction_id) override;

    /** @copydoc ocpp::types::ocpp16::ReservationStatus ICentralSystem::IChargePoint::reserveNow(unsigned int,
                                                                                         const ocpp::types::DateTime&,
                                                                                         const std::string&,
                                                                                         const std::string&,
                                                                                         int)*/
    ocpp::types::ocpp16::ReservationStatus reserveNow(unsigned int                 connector_id,
                                                      const ocpp::types::DateTime& expiry_date,
                                                      const std::string&           id_tag,
                                                      const std::string&           parent_id_tag,
                                                      int                          reservation_id) override;

    /** @copydoc bool ICentralSystem::IChargePoint::reset(ocpp::types::ocpp16::ResetType) */
    bool reset(ocpp::types::ocpp16::ResetType type) override;

    /** @copydoc ocpp::types::ocpp16::UpdateStatus ICentralSystem::IChargePoint::sendLocalList(int,
                                                                                       const std::vector<ocpp::types::ocpp16::AuthorizationData>&,
                                                                                       ocpp::types::ocpp16::UpdateType) */
    ocpp::types::ocpp16::UpdateStatus sendLocalList(int                                                        version,
                                                    const std::vector<ocpp::types::ocpp16::AuthorizationData>& authorization_list,
                                                    ocpp::types::ocpp16::UpdateType                            update_type) override;

    /** @copydoc ocpp::types::ocpp16::ChargingProfileStatus ICentralSystem::IChargePoint::setChargingProfile(unsigned int,
                                                                                                     const ocpp::types::ocpp16::ChargingProfile&) */
    ocpp::types::ocpp16::ChargingProfileStatus setChargingProfile(unsigned int                                connector_id,
                                                                  const ocpp::types::ocpp16::ChargingProfile& profile) override;

    /** @copydoc ocpp::types::ocpp16::TriggerMessageStatus ICentralSystem::IChargePoint::triggerMessage(ocpp::types::ocpp16::MessageTrigger,
                                                                                                const ocpp::types::Optional<unsigned int>) */
    ocpp::types::ocpp16::TriggerMessageStatus triggerMessage(ocpp::types::ocpp16::MessageTrigger       message,
                                                             const ocpp::types::Optional<unsigned int> connector_id) override;

    /** @copydoc ocpp::types::ocpp16::UnlockStatus ICentralSystem::IChargePoint::unlockConnector(unsigned int) */
    ocpp::types::ocpp16::UnlockStatus unlockConnector(unsigned int connector_id) override;

    /** @copydoc bool ICentralSystem::IChargePoint::updateFirmware(const std::string&,
                                                                   const ocpp::types::Optional<unsigned int>&,
                                                                   const ocpp::types::DateTime&,
                                                                   const ocpp::types::Optional<std::chrono::seconds>&) */
    bool updateFirmware(const std::string&                                 uri,
                        const ocpp::types::Optional<unsigned int>&         retries,
                        const ocpp::types::DateTime&                       retrieve_date,
                        const ocpp::types::Optional<std::chrono::seconds>& retry_interval) override;

    // Security extensions

    /** @copydoc bool ICentralSystem::IChargePoint::certificateSigned(const ocpp::x509::Certificate&) */
    bool certificateSigned(const ocpp::x509::Certificate& certificate_chain) override;

    /** @copydoc ocpp::types::ocpp16::DeleteCertificateStatusEnumType ICentralSystem::IChargePoint::deleteCertificate(const ocpp::types::ocpp16::CertificateHashDataType&) */
    ocpp::types::ocpp16::DeleteCertificateStatusEnumType deleteCertificate(
        const ocpp::types::ocpp16::CertificateHashDataType& certificate) override;

    /** @copydoc ocpp::types::ocpp16::TriggerMessageStatusEnumType ICentralSystem::IChargePoint::extendedTriggerMessage(ocpp::types::ocpp16::MessageTriggerEnumType,
                                                                                                                const ocpp::types::Optional<unsigned int>) */
    ocpp::types::ocpp16::TriggerMessageStatusEnumType extendedTriggerMessage(
        ocpp::types::ocpp16::MessageTriggerEnumType message, const ocpp::types::Optional<unsigned int> connector_id) override;

    /** @copydoc bool ICentralSystem::IChargePoint::getInstalledCertificateIds(ocpp::types::ocpp16::CertificateUseEnumType,
                                                                               std::vector<ocpp::types::ocpp16::CertificateHashDataType>&) */
    bool getInstalledCertificateIds(ocpp::types::ocpp16::CertificateUseEnumType                type,
                                    std::vector<ocpp::types::ocpp16::CertificateHashDataType>& certificates) override;

    /** @copydoc bool ICentralSystem::IChargePoint::getLog(ocpp::types::ocpp16::LogEnumType,
                                                           int,
                                                           const std::string&,
                                                           const ocpp::types::Optional<unsigned int>&,
                                                           const ocpp::types::Optional<std::chrono::seconds>&,
                                                           const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                           const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                           std::string&) */
    bool getLog(ocpp::types::ocpp16::LogEnumType                    type,
                int                                                 request_id,
                const std::string&                                  uri,
                const ocpp::types::Optional<unsigned int>&          retries,
                const ocpp::types::Optional<std::chrono::seconds>&  retry_interval,
                const ocpp::types::Optional<ocpp::types::DateTime>& start,
                const ocpp::types::Optional<ocpp::types::DateTime>& stop,
                std::string&                                        log_filename) override;

    /** @copydoc ocpp::types::ocpp16::CertificateStatusEnumType ICentralSystem::installCertificate(ocpp::types::ocpp16::CertificateUseEnumType,
                                                                                           const ocpp::x509::Certificate&) */
    ocpp::types::ocpp16::CertificateStatusEnumType installCertificate(ocpp::types::ocpp16::CertificateUseEnumType type,
                                                                      const ocpp::x509::Certificate&              certificate) override;

    /** @copydoc ocpp::types::ocpp16::UpdateFirmwareStatusEnumType ICentralSystem::signedUpdateFirmware(
                                                                  int,
                                                                  const std::string&,
                                                                  const ocpp::types::Optional<unsigned int>&,
                                                                  const ocpp::types::DateTime&,
                                                                  const ocpp::types::Optional<std::chrono::seconds>&,
                                                                  const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                                  const ocpp::x509::Certificate&,
                                                                  const std::string&) */
    ocpp::types::ocpp16::UpdateFirmwareStatusEnumType signedUpdateFirmware(
        int                                                 request_id,
        const std::string&                                  uri,
        const ocpp::types::Optional<unsigned int>&          retries,
        const ocpp::types::DateTime&                        retrieve_date,
        const ocpp::types::Optional<std::chrono::seconds>&  retry_interval,
        const ocpp::types::Optional<ocpp::types::DateTime>& install_date,
        const ocpp::x509::Certificate&                      signing_certificate,
        const std::string&                                  signature) override;

    // ISO 15118 PnC extensions

    /** @copydoc bool ICentralSystem::IChargePoint::iso15118CertificateSigned(const ocpp::x509::Certificate&) */
    bool iso15118CertificateSigned(const ocpp::x509::Certificate& certificate_chain) override;

    /** @copydoc ocpp::types::ocpp16::DeleteCertificateStatusEnumType ICentralSystem::IChargePoint::iso15118DeleteCertificate(
                                const ocpp::types::ocpp16::CertificateHashDataType&) */
    ocpp::types::ocpp16::DeleteCertificateStatusEnumType iso15118DeleteCertificate(
        const ocpp::types::ocpp16::CertificateHashDataType& certificate) override;

    /** @copydoc bool ICentralSystem::IChargePoint::iso15118GetInstalledCertificateIds(const std::vector<ocpp::types::ocpp16::GetCertificateIdUseEnumType>&,
                                                                                       std::vector<ocpp::types::ocpp16::CertificateHashDataChainType>&) */
    bool iso15118GetInstalledCertificateIds(const std::vector<ocpp::types::ocpp16::GetCertificateIdUseEnumType>& types,
                                            std::vector<ocpp::types::ocpp16::CertificateHashDataChainType>&      certificates) override;

    /** @copydoc ocpp::types::ocpp16::InstallCertificateStatusEnumType ICentralSystem::IChargePoint::iso15118CertificateSigned(
                                                                ocpp::types::ocpp16::InstallCertificateUseEnumType,
                                                                const ocpp::x509::Certificate&) */
    ocpp::types::ocpp16::InstallCertificateStatusEnumType iso15118InstallCertificate(
        ocpp::types::ocpp16::InstallCertificateUseEnumType type, const ocpp::x509::Certificate& certificate) override;

    /** @copydoc bool ICentralSystem::IChargePoint::iso15118TriggerSignCertificate() */
    bool iso15118TriggerSignCertificate() override;

    // IRpc::IListener interface

    /** @copydoc void IRpc::IListener::rpcDisconnected() */
    void rpcDisconnected() override;

    /** @copydoc void IRpc::IListener::rpcError() */
    void rpcError() override;

    /** @copydoc bool IRpc::IListener::rpcCallReceived(const std::string&,
                                                       const rapidjson::Value&,
                                                       rapidjson::Document&,
                                                       std::string&,
                                                       std::string&) */
    bool rpcCallReceived(const std::string&      action,
                         const rapidjson::Value& payload,
                         rapidjson::Document&    response,
                         std::string&            error_code,
                         std::string&            error_message) override;

    // IRpc::ISpy interface

    /** @copydoc void IRpc::ISpy::rcpMessageReceived(const std::string& msg) */
    void rcpMessageReceived(const std::string& msg) override;

    /** @copydoc void IRpc::ISpy::rcpMessageSent(const std::string& msg) */
    void rcpMessageSent(const std::string& msg) override;

    // Accessors

    /** @brief RPC connection */
    std::shared_ptr<ocpp::rpc::RpcServer::Client>& rpcClient() { return m_rpc; }

    /** @brief Messages validator */
    const ocpp::messages::ocpp16::MessagesValidator& messagesValidator() { return m_messages_validator; }

    /** @brief Messages converters */
    ocpp::messages::ocpp16::MessagesConverter& messagesConverter() { return m_messages_converter; }

  private:
    /** @brief Central System instance associated to the charge point */
    ICentralSystem& m_central_system;
    /** @brief Charge point's identifier */
    std::string m_identifier;
    /** @brief RPC connection */
    std::shared_ptr<ocpp::rpc::RpcServer::Client> m_rpc;
    /** @brief Message dispatcher */
    ocpp::messages::MessageDispatcher m_msg_dispatcher;
    /** @brief Message sender */
    ocpp::messages::GenericMessageSender m_msg_sender;
    /** @brief Request handler */
    ChargePointHandler m_handler;
    /** @brief Messages validator */
    const ocpp::messages::ocpp16::MessagesValidator& m_messages_validator;
    /** @brief Messages converters */
    ocpp::messages::ocpp16::MessagesConverter& m_messages_converter;
    /** @brief User request handler */
    IChargePointRequestHandler* m_user_handler;

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
        auto req_converter  = m_messages_converter.getRequestConverter<RequestType>(type_id);
        auto resp_converter = m_messages_converter.getResponseConverter<ResponseType>(type_id);

        // Prepare request
        ocpp::messages::ocpp16::DataTransferReq req;
        req.vendorId.assign(ocpp::messages::ocpp16::ISO15118_VENDOR_ID);
        req.messageId.value().assign(action);

        // Convert request to JSON
        rapidjson::Document                        json_req(rapidjson::kObjectType);
        rapidjson::StringBuffer                    buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        req_converter->setAllocator(&json_req.GetAllocator());
        req_converter->toJson(request, json_req);
        json_req.Accept(writer);
        req.data.value() = buffer.GetString();

        // Send request
        ocpp::messages::ocpp16::DataTransferConf resp;
        if (m_msg_sender.call(ocpp::messages::ocpp16::DATA_TRANSFER_ACTION, req, resp) == ocpp::messages::CallResult::Ok)
        {
            if (resp.status == ocpp::types::ocpp16::DataTransferStatus::Accepted)
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
                LOG_ERROR << "[ISO15118] Data transfer error : " << ocpp::types::ocpp16::DataTransferStatusHelper.toString(resp.status);
            }
        }

        return ret;
    }
};

} // namespace centralsystem
} // namespace ocpp

#endif // OPENOCPP_CS_CHARGEPOINTPROXY_H
