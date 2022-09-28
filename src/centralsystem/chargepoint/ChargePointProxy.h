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

#ifndef CHARGEPOINTPROXY_H
#define CHARGEPOINTPROXY_H

#include "ChargePointHandler.h"
#include "GenericMessageSender.h"
#include "ICentralSystem.h"
#include "MessageDispatcher.h"
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
     * @param schemas_path Path to the JSON schemas needed to validate payloads
     * @param messages_converter Converter from/to OCPP to/from JSON messages
     * @param stack_config Stack configuration
     */
    ChargePointProxy(ICentralSystem&                               central_system,
                     const std::string&                            identifier,
                     std::shared_ptr<ocpp::rpc::RpcServer::Client> rpc,
                     const std::string&                            schemas_path,
                     ocpp::messages::MessagesConverter&            messages_converter,
                     const ocpp::config::ICentralSystemConfig&     stack_config);
    /** @brief Destructor */
    virtual ~ChargePointProxy();

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

    /** @copydoc ocpp::types::AvailabilityStatus ICentralSystem::IChargePoint::changeAvailability(int, ocpp::types::AvailabilityType) */
    ocpp::types::AvailabilityStatus changeAvailability(int connector_id, ocpp::types::AvailabilityType availability) override;

    /** @copydoc ocpp::types::ConfigurationStatus ICentralSystem::IChargePoint::changeConfiguration(const std::string&, const std::string&) */
    ocpp::types::ConfigurationStatus changeConfiguration(const std::string& key, const std::string& value) override;

    /** @copydoc bool ICentralSystem::IChargePoint::clearCache() */
    bool clearCache() override;

    /** @copydoc bool ICentralSystem::IChargePoint::clearChargingProfile(const ocpp::types::Optional<int>&,
                                                                         const ocpp::types::Optional<unsigned int>&,
                                                                         const ocpp::types::Optional<ocpp::types::ChargingProfilePurposeType>&,
                                                                         const ocpp::types::Optional<unsigned int>&) */
    bool clearChargingProfile(const ocpp::types::Optional<int>&                                     profile_id,
                              const ocpp::types::Optional<unsigned int>&                            connector_id,
                              const ocpp::types::Optional<ocpp::types::ChargingProfilePurposeType>& purpose,
                              const ocpp::types::Optional<unsigned int>&                            stack_level) override;

    /** @copydoc bool ICentralSystem::IChargePoint::dataTransfer(const std::string&,
                                                                 const std::string&,
                                                                 const std::string&,
                                                                 ocpp::types::DataTransferStatus&,
                                                                 std::string&) */
    bool dataTransfer(const std::string&               vendor_id,
                      const std::string&               message_id,
                      const std::string&               request_data,
                      ocpp::types::DataTransferStatus& status,
                      std::string&                     response_data) override;

    /** @copydoc bool ICentralSystem::IChargePoint::getCompositeSchedule(unsigned int,
                                                                         std::chrono::seconds,
                                                                         const ocpp::types::Optional<ocpp::types::ChargingRateUnitType>&,
                                                                         ocpp::types::Optional<unsigned int>&,
                                                                         ocpp::types::Optional<ocpp::types::DateTime>&,
                                                                         ocpp::types::Optional<ocpp::types::ChargingSchedule>&) */
    bool getCompositeSchedule(unsigned int                                                    connector_id,
                              std::chrono::seconds                                            duration,
                              const ocpp::types::Optional<ocpp::types::ChargingRateUnitType>& unit,
                              ocpp::types::Optional<unsigned int>&                            schedule_connector_id,
                              ocpp::types::Optional<ocpp::types::DateTime>&                   schedule_start,
                              ocpp::types::Optional<ocpp::types::ChargingSchedule>&           schedule) override;

    /** @copydoc bool ICentralSystem::IChargePoint::getConfiguration(const std::vector<std::string>&,
                                                                     std::vector<ocpp::types::KeyValue>&,
                                                                     std::vector<std::string>&) */
    bool getConfiguration(const std::vector<std::string>&     keys,
                          std::vector<ocpp::types::KeyValue>& config_keys,
                          std::vector<std::string>&           unknown_keys) override;

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
                                                                           const ocpp::types::Optional<ocpp::types::ChargingProfile>&) */
    bool remoteStartTransaction(const ocpp::types::Optional<unsigned int>&                 connector_id,
                                const std::string&                                         id_tag,
                                const ocpp::types::Optional<ocpp::types::ChargingProfile>& profile) override;

    /** @copydoc bool ICentralSystem::IChargePoint::remoteStopTransaction(int) */
    bool remoteStopTransaction(int transaction_id) override;

    /** @copydoc ocpp::types::ReservationStatus ICentralSystem::IChargePoint::reserveNow(unsigned int,
                                                                                         const ocpp::types::DateTime&,
                                                                                         const std::string&,
                                                                                         const std::string&,
                                                                                         int)*/
    ocpp::types::ReservationStatus reserveNow(unsigned int                 connector_id,
                                              const ocpp::types::DateTime& expiry_date,
                                              const std::string&           id_tag,
                                              const std::string&           parent_id_tag,
                                              int                          reservation_id) override;

    /** @copydoc bool ICentralSystem::IChargePoint::reset(ocpp::types::ResetType) */
    bool reset(ocpp::types::ResetType type) override;

    /** @copydoc ocpp::types::UpdateStatus ICentralSystem::IChargePoint::sendLocalList(int,
                                                                                       const std::vector<ocpp::types::AuthorizationData>&,
                                                                                       ocpp::types::UpdateType) */
    ocpp::types::UpdateStatus sendLocalList(int                                                version,
                                            const std::vector<ocpp::types::AuthorizationData>& authorization_list,
                                            ocpp::types::UpdateType                            update_type) override;

    /** @copydoc ocpp::types::ChargingProfileStatus ICentralSystem::IChargePoint::setChargingProfile(unsigned int,
                                                                                                     const ocpp::types::ChargingProfile&) */
    ocpp::types::ChargingProfileStatus setChargingProfile(unsigned int connector_id, const ocpp::types::ChargingProfile& profile) override;

    /** @copydoc ocpp::types::TriggerMessageStatus ICentralSystem::IChargePoint::triggerMessage(ocpp::types::MessageTrigger,
                                                                                                const ocpp::types::Optional<unsigned int>) */
    ocpp::types::TriggerMessageStatus triggerMessage(ocpp::types::MessageTrigger               message,
                                                     const ocpp::types::Optional<unsigned int> connector_id) override;

    /** @copydoc ocpp::types::UnlockStatus ICentralSystem::IChargePoint::unlockConnector(unsigned int) */
    ocpp::types::UnlockStatus unlockConnector(unsigned int connector_id) override;

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

    /** @copydoc ocpp::types::DeleteCertificateStatusEnumType ICentralSystem::IChargePoint::deleteCertificate(const ocpp::types::CertificateHashDataType&) */
    ocpp::types::DeleteCertificateStatusEnumType deleteCertificate(const ocpp::types::CertificateHashDataType& certificate) override;

    /** @copydoc ocpp::types::TriggerMessageStatusEnumType ICentralSystem::IChargePoint::extendedTriggerMessage(ocpp::types::MessageTriggerEnumType,
                                                                                                                const ocpp::types::Optional<unsigned int>) */
    ocpp::types::TriggerMessageStatusEnumType extendedTriggerMessage(ocpp::types::MessageTriggerEnumType       message,
                                                                     const ocpp::types::Optional<unsigned int> connector_id) override;

    /** @copydoc bool ICentralSystem::IChargePoint::getInstalledCertificateIds(ocpp::types::CertificateUseEnumType,
                                                                               std::vector<ocpp::types::CertificateHashDataType>&) */
    bool getInstalledCertificateIds(ocpp::types::CertificateUseEnumType                type,
                                    std::vector<ocpp::types::CertificateHashDataType>& certificates) override;

    /** @copydoc bool ICentralSystem::IChargePoint::getLog(ocpp::types::LogEnumType,
                                                           int,
                                                           const std::string&,
                                                           const ocpp::types::Optional<unsigned int>&,
                                                           const ocpp::types::Optional<std::chrono::seconds>&,
                                                           const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                           const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                           std::string&) */
    bool getLog(ocpp::types::LogEnumType                            type,
                int                                                 request_id,
                const std::string&                                  uri,
                const ocpp::types::Optional<unsigned int>&          retries,
                const ocpp::types::Optional<std::chrono::seconds>&  retry_interval,
                const ocpp::types::Optional<ocpp::types::DateTime>& start,
                const ocpp::types::Optional<ocpp::types::DateTime>& stop,
                std::string&                                        log_filename) override;

    /** @copydoc ocpp::types::CertificateStatusEnumType ICentralSystem::installCertificate(ocpp::types::CertificateUseEnumType,
                                                                                           const ocpp::x509::Certificate&) */
    ocpp::types::CertificateStatusEnumType installCertificate(ocpp::types::CertificateUseEnumType type,
                                                              const ocpp::x509::Certificate&      certificate) override;

    /** @copydoc ocpp::types::UpdateFirmwareStatusEnumType ICentralSystem::signedUpdateFirmware(
                                                                  int,
                                                                  const std::string&,
                                                                  const ocpp::types::Optional<unsigned int>&,
                                                                  const ocpp::types::DateTime&,
                                                                  const ocpp::types::Optional<std::chrono::seconds>&,
                                                                  const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                                  const ocpp::x509::Certificate&,
                                                                  const std::string&) */
    ocpp::types::UpdateFirmwareStatusEnumType signedUpdateFirmware(int                                                 request_id,
                                                                   const std::string&                                  uri,
                                                                   const ocpp::types::Optional<unsigned int>&          retries,
                                                                   const ocpp::types::DateTime&                        retrieve_date,
                                                                   const ocpp::types::Optional<std::chrono::seconds>&  retry_interval,
                                                                   const ocpp::types::Optional<ocpp::types::DateTime>& install_date,
                                                                   const ocpp::x509::Certificate&                      signing_certificate,
                                                                   const std::string&                                  signature) override;

    // IRpc::IListener interface

    /** @copydoc void IRpc::IListener::rpcDisconnected() */
    void rpcDisconnected() override;

    /** @copydoc void IRpc::IListener::rpcError() */
    void rpcError() override;

    /** @copydoc bool IRpc::IListener::rpcCallReceived(const std::string&,
                                                       const rapidjson::Value&,
                                                       rapidjson::Document&,
                                                       const char*&,
                                                       std::string&) */
    bool rpcCallReceived(const std::string&      action,
                         const rapidjson::Value& payload,
                         rapidjson::Document&    response,
                         const char*&            error_code,
                         std::string&            error_message) override;

    // IRpc::ISpy interface

    /** @copydoc void IRpc::ISpy::rcpMessageReceived(const std::string& msg) */
    void rcpMessageReceived(const std::string& msg) override;

    /** @copydoc void IRpc::ISpy::rcpMessageSent(const std::string& msg) */
    void rcpMessageSent(const std::string& msg) override;

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
    /** @brief User request handler */
    IChargePointRequestHandler* m_user_handler;
};

} // namespace centralsystem
} // namespace ocpp

#endif // CHARGEPOINTPROXY_H
