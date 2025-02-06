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

#include "ChargePointProxy.h"
#include "CancelReservation.h"
#include "CertificateSigned.h"
#include "ChangeAvailability.h"
#include "ChangeConfiguration.h"
#include "ClearCache.h"
#include "ClearChargingProfile.h"
#include "DataTransfer.h"
#include "DeleteCertificate.h"
#include "ExtendedTriggerMessage.h"
#include "GetCompositeSchedule.h"
#include "GetConfiguration.h"
#include "GetDiagnostics.h"
#include "GetInstalledCertificateIds.h"
#include "GetLocalListVersion.h"
#include "GetLog.h"
#include "ICentralSystemConfig.h"
#include "InstallCertificate.h"
#include "Iso15118GetInstalledCertificateIds.h"
#include "Iso15118InstallCertificate.h"
#include "Iso15118TriggerMessage.h"
#include "RemoteStartTransaction.h"
#include "RemoteStopTransaction.h"
#include "ReserveNow.h"
#include "Reset.h"
#include "SendLocalList.h"
#include "SetChargingProfile.h"
#include "SignedUpdateFirmware.h"
#include "TriggerMessage.h"
#include "UnlockConnector.h"
#include "UpdateFirmware.h"

using namespace ocpp::messages;
using namespace ocpp::messages::ocpp16;
using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace centralsystem
{

/** @brief Constructor */
ChargePointProxy::ChargePointProxy(ICentralSystem&                                  central_system,
                                   const std::string&                               identifier,
                                   std::shared_ptr<ocpp::rpc::RpcServer::Client>    rpc,
                                   const ocpp::messages::ocpp16::MessagesValidator& messages_validator,
                                   ocpp::messages::ocpp16::MessagesConverter&       messages_converter,
                                   const ocpp::config::ICentralSystemConfig&        stack_config)
    : m_central_system(central_system),
      m_identifier(identifier),
      m_rpc(rpc),
      m_msg_dispatcher(messages_validator),
      m_msg_sender(*m_rpc, messages_converter, messages_validator, stack_config.callRequestTimeout()),
      m_handler(m_identifier, messages_converter, m_msg_dispatcher, stack_config),
      m_messages_validator(messages_validator),
      m_messages_converter(messages_converter)
{
    m_rpc->registerSpy(*this);
    m_rpc->registerListener(*this);
}

/** @brief Destructor */
ChargePointProxy::~ChargePointProxy()
{
    unregisterFromRpcSpy();
}

/** @brief Unregister to the IRpc::ISpy interface messages */
void ChargePointProxy::unregisterFromRpcSpy()
{
    m_rpc->unregisterSpy(*this);
}

// ICentralSystem::IChargePoint interface

/** @copydoc const std::string& ICentralSystem::IChargePoint::ipAddress() const */
const std::string& ChargePointProxy::ipAddress() const
{
    return m_rpc->ipAddress();
}

/** @copydoc void ICentralSystem::IChargePoint::setTimeout(std::chrono::milliseconds) */
void ChargePointProxy::setTimeout(std::chrono::milliseconds timeout)
{
    m_msg_sender.setTimeout(timeout);
}

/** @copydoc void ICentralSystem::IChargePoint::disconnect() */
void ChargePointProxy::disconnect()
{
    m_rpc->disconnect(true);
}

/** @copydoc void ICentralSystem::IChargePoint::registerHandler(IChargePointRequestHandler&) */
void ChargePointProxy::registerHandler(IChargePointRequestHandler& handler)
{
    m_user_handler = &handler;
    m_handler.registerHandler(handler);
}

// OCPP operations

/** @copydoc bool ICentralSystem::IChargePoint::cancelReservation(int) */
bool ChargePointProxy::cancelReservation(int reservation_id)
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier << "] - Cancel reservation : reservationId = " << reservation_id;

    // Prepare request
    CancelReservationReq req;
    req.reservationId = reservation_id;

    // Send request
    CancelReservationConf resp;
    CallResult            res = m_msg_sender.call(CANCEL_RESERVATION_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = (resp.status == CancelReservationStatus::Accepted);
        LOG_INFO << "[" << m_identifier << "] - Cancel reservation : " << CancelReservationStatusHelper.toString(resp.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc ocpp::types::ocpp16::AvailabilityStatus ICentralSystem::IChargePoint::changeAvailability(unsigned int, ocpp::types::ocpp16::AvailabilityType) */
ocpp::types::ocpp16::AvailabilityStatus ChargePointProxy::changeAvailability(unsigned int                          connector_id,
                                                                             ocpp::types::ocpp16::AvailabilityType availability)
{
    AvailabilityStatus ret = AvailabilityStatus::Rejected;

    LOG_INFO << "[" << m_identifier << "] - Change availability : connectorId = " << connector_id
             << " - availability = " << AvailabilityTypeHelper.toString(availability);

    // Prepare request
    ChangeAvailabilityReq req;
    req.connectorId = connector_id;
    req.type        = availability;

    // Send request
    ChangeAvailabilityConf resp;
    CallResult             res = m_msg_sender.call(CHANGE_AVAILABILITY_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = resp.status;
        LOG_INFO << "[" << m_identifier << "] - Change availability : " << AvailabilityStatusHelper.toString(resp.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc ocpp::types::ocpp16::ConfigurationStatus ICentralSystem::IChargePoint::changeConfiguration(const std::string&, const std::string&) */
ocpp::types::ocpp16::ConfigurationStatus ChargePointProxy::changeConfiguration(const std::string& key, const std::string& value)
{
    ConfigurationStatus ret = ConfigurationStatus::Rejected;

    LOG_INFO << "[" << m_identifier << "] - Change configuration : key = " << key << " - value = " << value;

    // Prepare request
    ChangeConfigurationReq req;
    req.key.assign(key);
    req.value.assign(value);

    // Send request
    ChangeConfigurationConf resp;
    CallResult              res = m_msg_sender.call(CHANGE_CONFIGURATION_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = resp.status;
        LOG_INFO << "[" << m_identifier << "] - Change configuration : " << ConfigurationStatusHelper.toString(resp.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc bool ICentralSystem::IChargePoint::clearCache() */
bool ChargePointProxy::clearCache()
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier << "] - Clear cache";

    // Prepare request
    ClearCacheReq req;

    // Send request
    ClearCacheConf resp;
    CallResult     res = m_msg_sender.call(CLEAR_CACHE_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = (resp.status == ClearCacheStatus::Accepted);
        LOG_INFO << "[" << m_identifier << "] - Clear cache : " << ClearCacheStatusHelper.toString(resp.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc bool ICentralSystem::IChargePoint::clearChargingProfile(const ocpp::types::Optional<int>&,
                                                                     const ocpp::types::Optional<unsigned int>&,
                                                                     const ocpp::types::Optional<ocpp::types::ocpp16::ChargingProfilePurposeType>&,
                                                                     const ocpp::types::Optional<unsigned int>&) */
bool ChargePointProxy::clearChargingProfile(const ocpp::types::Optional<int>&                                             profile_id,
                                            const ocpp::types::Optional<unsigned int>&                                    connector_id,
                                            const ocpp::types::Optional<ocpp::types::ocpp16::ChargingProfilePurposeType>& purpose,
                                            const ocpp::types::Optional<unsigned int>&                                    stack_level)
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier << "] - Clear charging profile : id = " << (profile_id.isSet() ? std::to_string(profile_id) : "not set")
             << " - connectorId = " << (connector_id.isSet() ? std::to_string(connector_id) : "not set")
             << " - chargingProfilePurpose = " << (purpose.isSet() ? ChargingProfilePurposeTypeHelper.toString(purpose) : "not set")
             << " - stackLevel = " << (stack_level.isSet() ? std::to_string(stack_level) : "not set");

    // Prepare request
    ClearChargingProfileReq req;
    req.id                     = profile_id;
    req.connectorId            = connector_id;
    req.chargingProfilePurpose = purpose;
    req.stackLevel             = stack_level;

    // Send request
    ClearChargingProfileConf resp;
    CallResult               res = m_msg_sender.call(CLEAR_CHARGING_PROFILE_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = (resp.status == ClearChargingProfileStatus::Accepted);
        LOG_INFO << "[" << m_identifier << "] - Clear charging profile : " << ClearChargingProfileStatusHelper.toString(resp.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc bool ICentralSystem::IChargePoint::dataTransfer(const std::string&,
                                                             const std::string&,
                                                             const std::string&,
                                                             ocpp::types::ocpp16::DataTransferStatus&,
                                                             std::string&) */
bool ChargePointProxy::dataTransfer(const std::string&                       vendor_id,
                                    const std::string&                       message_id,
                                    const std::string&                       request_data,
                                    ocpp::types::ocpp16::DataTransferStatus& status,
                                    std::string&                             response_data)
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier << "] - Data transfer : vendorId = " << vendor_id << " - messageId = " << message_id
             << " - data = " << request_data;

    // Prepare request
    DataTransferReq req;
    req.vendorId.assign(vendor_id);
    if (!message_id.empty())
    {
        req.messageId.value().assign(message_id);
    }
    if (!request_data.empty())
    {
        req.data = request_data;
    }

    // Send request
    DataTransferConf resp;
    CallResult       res = m_msg_sender.call(DATA_TRANSFER_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret           = (resp.status == DataTransferStatus::Accepted);
        status        = resp.status;
        response_data = resp.data;
        LOG_INFO << "[" << m_identifier << "] - Data transfer : status = " << DataTransferStatusHelper.toString(resp.status)
                 << " - data = " << (resp.data.isSet() ? resp.data.value() : "not set");
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc bool ICentralSystem::IChargePoint::getCompositeSchedule(unsigned int,
                                                                     std::chrono::seconds,
                                                                     const ocpp::types::Optional<ocpp::types::ocpp16::ChargingRateUnitType>&,
                                                                     ocpp::types::Optional<unsigned int>&,
                                                                     ocpp::types::Optional<ocpp::types::DateTime>&,
                                                                     ocpp::types::Optional<ocpp::types::ocpp16::ChargingSchedule>&) */
bool ChargePointProxy::getCompositeSchedule(unsigned int                                                            connector_id,
                                            std::chrono::seconds                                                    duration,
                                            const ocpp::types::Optional<ocpp::types::ocpp16::ChargingRateUnitType>& unit,
                                            ocpp::types::Optional<unsigned int>&                                    schedule_connector_id,
                                            ocpp::types::Optional<ocpp::types::DateTime>&                           schedule_start,
                                            ocpp::types::Optional<ocpp::types::ocpp16::ChargingSchedule>&           schedule)
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier << "] - Get composite schedule : connectorId = " << connector_id << " - duration = " << duration.count()
             << " - unit = " << (unit.isSet() ? ChargingRateUnitTypeHelper.toString(unit) : "not set");

    // Prepare request
    GetCompositeScheduleReq req;
    req.connectorId      = connector_id;
    req.duration         = static_cast<unsigned int>(duration.count());
    req.chargingRateUnit = unit;

    // Send request
    GetCompositeScheduleConf resp;
    CallResult               res = m_msg_sender.call(GET_COMPOSITE_SCHEDULE_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret                   = (resp.status == GetCompositeScheduleStatus::Accepted);
        schedule_connector_id = resp.connectorId;
        schedule_start        = resp.scheduleStart;
        schedule              = resp.chargingSchedule;
        LOG_INFO << "[" << m_identifier
                 << "] - Get composite schedule : status = " << GetCompositeScheduleStatusHelper.toString(resp.status)
                 << " - connectorId = " << (resp.connectorId.isSet() ? std::to_string(resp.connectorId) : "not set")
                 << " - scheduleStart = " << (resp.scheduleStart.isSet() ? resp.scheduleStart.value().str() : "not set")
                 << " - chargingSchedule = " << (resp.chargingSchedule.isSet() ? "set" : "not set");
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc bool ICentralSystem::IChargePoint::getConfiguration(const std::vector<std::string>&,
                                                                 std::vector<ocpp::types::ocpp16::KeyValue>&,
                                                                 std::vector<std::string>&) */
bool ChargePointProxy::getConfiguration(const std::vector<std::string>&             keys,
                                        std::vector<ocpp::types::ocpp16::KeyValue>& config_keys,
                                        std::vector<std::string>&                   unknown_keys)
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier << "] - Get configuration : key count = " << keys.size();

    // Prepare request
    GetConfigurationReq req;
    if (!keys.empty())
    {
        for (const std::string& key : keys)
        {
            req.key.value().emplace_back();
            req.key.value().back().assign(key);
        }
    }

    // Send request
    GetConfigurationConf resp;
    CallResult           res = m_msg_sender.call(GET_CONFIGURATION_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = true;
        if (resp.configurationKey.isSet())
        {
            config_keys = resp.configurationKey;
        }
        if (resp.unknownKey.isSet())
        {
            for (const auto& key : resp.unknownKey.value())
            {
                unknown_keys.emplace_back(key.str());
            }
        }
        LOG_INFO << "[" << m_identifier << "] - Get configuration : key count = " << config_keys.size()
                 << " - unknown key count = " << unknown_keys.size();
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc bool ICentralSystem::IChargePoint::getDiagnostics(const std::string&,
                                                               const ocpp::types::Optional<unsigned int>&,
                                                               const ocpp::types::Optional<std::chrono::seconds>&,
                                                               const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                               const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                               std::string&) */
bool ChargePointProxy::getDiagnostics(const std::string&                                  uri,
                                      const ocpp::types::Optional<unsigned int>&          retries,
                                      const ocpp::types::Optional<std::chrono::seconds>&  retry_interval,
                                      const ocpp::types::Optional<ocpp::types::DateTime>& start,
                                      const ocpp::types::Optional<ocpp::types::DateTime>& stop,
                                      std::string&                                        diagnotic_filename)
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier << "] - Get diagnostics : location = " << uri
             << " - retries = " << (retries.isSet() ? std::to_string(retries) : "not set")
             << " - retryInterval = " << (retry_interval.isSet() ? std::to_string(retry_interval.value().count()) : "not set")
             << " - startTime = " << (start.isSet() ? start.value().str() : "not set")
             << " - stopTime = " << (stop.isSet() ? stop.value().str() : "not set");

    // Prepare request
    GetDiagnosticsReq req;
    req.location = uri;
    req.retries  = retries;
    if (retry_interval.isSet())
    {
        req.retryInterval = static_cast<unsigned int>(retry_interval.value().count());
    }
    req.startTime = start;
    req.stopTime  = stop;

    // Send request
    GetDiagnosticsConf resp;
    CallResult         res = m_msg_sender.call(GET_DIAGNOSTICS_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret                = true;
        diagnotic_filename = resp.fileName.value();
        LOG_INFO << "[" << m_identifier << "] - Get diagnostics : filename = "
                 << (resp.fileName.isSet() ? resp.fileName.value().str() : "no diagnostic available");
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc bool ICentralSystem::IChargePoint::getLocalListVersion(int&) */
bool ChargePointProxy::getLocalListVersion(int& version)
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier << "] - Get local list version";

    // Prepare request
    GetLocalListVersionReq req;

    // Send request
    GetLocalListVersionConf resp;
    CallResult              res = m_msg_sender.call(GET_LOCAL_LIST_VERSION_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret     = true;
        version = resp.listVersion;
        LOG_INFO << "[" << m_identifier << "] - Get local list version : " << resp.listVersion;
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc bool ICentralSystem::IChargePoint::remoteStartTransaction(const ocpp::types::Optional<unsigned int>&,
                                                                       const std::string&,
                                                                       const ocpp::types::Optional<ocpp::types::ocpp16::ChargingProfile>&) */
bool ChargePointProxy::remoteStartTransaction(const ocpp::types::Optional<unsigned int>&                         connector_id,
                                              const std::string&                                                 id_tag,
                                              const ocpp::types::Optional<ocpp::types::ocpp16::ChargingProfile>& profile)
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier
             << "] - Remote start transaction : connectorId = " << (connector_id.isSet() ? std::to_string(connector_id) : "not set")
             << " - idTag = " << id_tag << " - chargingProfile = " << (profile.isSet() ? "set" : "not set");

    // Prepare request
    RemoteStartTransactionReq req;
    req.connectorId = connector_id;
    req.idTag.assign(id_tag);
    req.chargingProfile = profile;

    // Send request
    RemoteStartTransactionConf resp;
    CallResult                 res = m_msg_sender.call(REMOTE_START_TRANSACTION_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = (resp.status == RemoteStartStopStatus::Accepted);
        LOG_INFO << "[" << m_identifier << "] - Remote start transaction : " << RemoteStartStopStatusHelper.toString(resp.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc bool ICentralSystem::IChargePoint::remoteStopTransaction(int) */
bool ChargePointProxy::remoteStopTransaction(int transaction_id)
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier << "] - Remote stop transaction : transactionId = " << transaction_id;

    // Prepare request
    RemoteStopTransactionReq req;
    req.transactionId = transaction_id;

    // Send request
    RemoteStopTransactionConf resp;
    CallResult                res = m_msg_sender.call(REMOTE_STOP_TRANSACTION_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = (resp.status == RemoteStartStopStatus::Accepted);
        LOG_INFO << "[" << m_identifier << "] - Remote stop transaction : " << RemoteStartStopStatusHelper.toString(resp.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc ocpp::types::ocpp16::ReservationStatus ICentralSystem::IChargePoint::reserveNow(unsigned int,
                                                                                     const ocpp::types::DateTime&,
                                                                                     const std::string&,
                                                                                     const std::string&,
                                                                                     int)*/
ocpp::types::ocpp16::ReservationStatus ChargePointProxy::reserveNow(unsigned int                 connector_id,
                                                                    const ocpp::types::DateTime& expiry_date,
                                                                    const std::string&           id_tag,
                                                                    const std::string&           parent_id_tag,
                                                                    int                          reservation_id)
{
    ReservationStatus ret = ReservationStatus::Rejected;

    LOG_INFO << "[" << m_identifier << "] - Reserve now : connectorId = " << connector_id << " - expiryDate = " << expiry_date.str()
             << " - idTag = " << id_tag << " - parentIdTag = " << parent_id_tag << " - reservationId = " << reservation_id;

    // Prepare request
    ReserveNowReq req;
    req.connectorId = connector_id;
    req.expiryDate  = expiry_date;
    req.idTag.assign(id_tag);
    if (!parent_id_tag.empty())
    {
        req.parentIdTag.value().assign(parent_id_tag);
    }
    req.reservationId = reservation_id;

    // Send request
    ReserveNowConf resp;
    CallResult     res = m_msg_sender.call(RESERVE_NOW_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = resp.status;
        LOG_INFO << "[" << m_identifier << "] - Reserve now : " << ReservationStatusHelper.toString(resp.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc bool ICentralSystem::IChargePoint::reset(ocpp::types::ocpp16::ResetType) */
bool ChargePointProxy::reset(ocpp::types::ocpp16::ResetType type)
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier << "] - Reset : type = " << ResetTypeHelper.toString(type);

    // Prepare request
    ResetReq req;
    req.type = type;

    // Send request
    ResetConf  resp;
    CallResult res = m_msg_sender.call(RESET_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = (resp.status == ResetStatus::Accepted);
        LOG_INFO << "[" << m_identifier << "] - Reset : " << ResetStatusHelper.toString(resp.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc ocpp::types::ocpp16::UpdateStatus ICentralSystem::IChargePoint::sendLocalList(int,
                                                                                   const std::vector<ocpp::types::ocpp16::AuthorizationData>&,
                                                                                   ocpp::types::ocpp16::UpdateType) */
ocpp::types::ocpp16::UpdateStatus ChargePointProxy::sendLocalList(
    int version, const std::vector<ocpp::types::ocpp16::AuthorizationData>& authorization_list, ocpp::types::ocpp16::UpdateType update_type)
{
    UpdateStatus ret = UpdateStatus::Failed;

    LOG_INFO << "[" << m_identifier << "] - Send local list : listVersion = " << version
             << " - localAuthorizationList count = " << authorization_list.size()
             << " - updateType = " << UpdateTypeHelper.toString(update_type);

    // Prepare request
    SendLocalListReq req;
    req.listVersion            = version;
    req.localAuthorizationList = authorization_list;
    req.updateType             = update_type;

    // Send request
    SendLocalListConf resp;
    CallResult        res = m_msg_sender.call(SEND_LOCAL_LIST_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = resp.status;
        LOG_INFO << "[" << m_identifier << "] - Send local list : " << UpdateStatusHelper.toString(resp.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc ocpp::types::ocpp16::ChargingProfileStatus ICentralSystem::IChargePoint::setChargingProfile(unsigned int,
                                                                                                 const ocpp::types::ocpp16::ChargingProfile&) */
ocpp::types::ocpp16::ChargingProfileStatus ChargePointProxy::setChargingProfile(unsigned int                                connector_id,
                                                                                const ocpp::types::ocpp16::ChargingProfile& profile)
{
    ChargingProfileStatus ret = ChargingProfileStatus::Rejected;

    LOG_INFO << "[" << m_identifier << "] - Set charging profile : connectorId = " << connector_id
             << " - csChargingProfiles : id = " << profile.chargingProfileId
             << " - purpose = " << ChargingProfilePurposeTypeHelper.toString(profile.chargingProfilePurpose);

    // Prepare request
    SetChargingProfileReq req;
    req.connectorId        = connector_id;
    req.csChargingProfiles = profile;

    // Send request
    SetChargingProfileConf resp;
    CallResult             res = m_msg_sender.call(SET_CHARGING_PROFILE_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = resp.status;
        LOG_INFO << "[" << m_identifier << "] - Set charging profile : " << ChargingProfileStatusHelper.toString(resp.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc ocpp::types::ocpp16::TriggerMessageStatus ICentralSystem::IChargePoint::triggerMessage(ocpp::types::ocpp16::MessageTrigger,
                                                                                                const ocpp::types::Optional<unsigned int>) */
ocpp::types::ocpp16::TriggerMessageStatus ChargePointProxy::triggerMessage(ocpp::types::ocpp16::MessageTrigger       message,
                                                                           const ocpp::types::Optional<unsigned int> connector_id)
{
    TriggerMessageStatus ret = TriggerMessageStatus::Rejected;

    LOG_INFO << "[" << m_identifier << "] - Trigger message : requestedMessage = " << MessageTriggerHelper.toString(message)
             << " - connectorId = " << (connector_id.isSet() ? std::to_string(connector_id) : "not set");

    // Prepare request
    TriggerMessageReq req;
    req.requestedMessage = message;
    req.connectorId      = connector_id;

    // Send request
    TriggerMessageConf resp;
    CallResult         res = m_msg_sender.call(TRIGGER_MESSAGE_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = resp.status;
        LOG_INFO << "[" << m_identifier << "] - Trigger message : " << TriggerMessageStatusHelper.toString(resp.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc ocpp::types::ocpp16::UnlockStatus ICentralSystem::IChargePoint::unlockConnector(unsigned int) */
ocpp::types::ocpp16::UnlockStatus ChargePointProxy::unlockConnector(unsigned int connector_id)
{
    UnlockStatus ret = UnlockStatus::UnlockFailed;

    LOG_INFO << "[" << m_identifier << "] - Unlock connector : connectorId = " << connector_id;

    // Prepare request
    UnlockConnectorReq req;
    req.connectorId = connector_id;

    // Send request
    UnlockConnectorConf resp;
    CallResult          res = m_msg_sender.call(UNLOCK_CONNECTOR_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = resp.status;
        LOG_INFO << "[" << m_identifier << "] - Unlock connector : " << UnlockStatusHelper.toString(resp.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc bool ICentralSystem::IChargePoint::updateFirmware(const std::string&,
                                                               const ocpp::types::Optional<unsigned int>&,
                                                               const ocpp::types::DateTime&,
                                                               const ocpp::types::Optional<std::chrono::seconds>&) */
bool ChargePointProxy::updateFirmware(const std::string&                                 uri,
                                      const ocpp::types::Optional<unsigned int>&         retries,
                                      const ocpp::types::DateTime&                       retrieve_date,
                                      const ocpp::types::Optional<std::chrono::seconds>& retry_interval)
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier << "] - Update firmware : location = " << uri
             << " - retries = " << (retries.isSet() ? std::to_string(retries) : "not set") << " - retrieveDate = " << retrieve_date.str()
             << " - retryInterval = " << (retry_interval.isSet() ? std::to_string(retry_interval.value().count()) : "not set");

    // Prepare request
    UpdateFirmwareReq req;
    req.location     = uri;
    req.retries      = retries;
    req.retrieveDate = retrieve_date;
    if (retry_interval.isSet())
    {
        req.retryInterval = static_cast<unsigned int>(retry_interval.value().count());
    }

    // Send request
    UpdateFirmwareConf resp;
    CallResult         res = m_msg_sender.call(UPDATE_FIRMWARE_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = true;
        LOG_INFO << "[" << m_identifier << "] - Update firmware : Accepted";
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

// Security extensions

/** @copydoc bool ICentralSystem::IChargePoint::certificateSigned(const ocpp::x509::Certificate&) */
bool ChargePointProxy::certificateSigned(const ocpp::x509::Certificate& certificate_chain)
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier << "] - Certificate signed : certificate chain size = " << certificate_chain.pemChain().size();

    // Prepare request
    CertificateSignedReq req;
    req.certificateChain.assign(certificate_chain.pem());

    // Send request
    CertificateSignedConf resp;
    CallResult            res = m_msg_sender.call(CERTIFICATE_SIGNED_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = (resp.status == CertificateSignedStatusEnumType::Accepted);
        LOG_INFO << "[" << m_identifier << "] - Certificate signed : " << CertificateSignedStatusEnumTypeHelper.toString(resp.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc ocpp::types::ocpp16::DeleteCertificateStatusEnumType ICentralSystem::IChargePoint::deleteCertificate(const ocpp::types::ocpp16::CertificateHashDataType&) */
ocpp::types::ocpp16::DeleteCertificateStatusEnumType ChargePointProxy::deleteCertificate(
    const ocpp::types::ocpp16::CertificateHashDataType& certificate)

{
    DeleteCertificateStatusEnumType ret = DeleteCertificateStatusEnumType::Failed;

    LOG_INFO << "[" << m_identifier << "] - Delete certificate : serialNumber = " << certificate.serialNumber.str();

    // Prepare request
    DeleteCertificateReq req;
    req.certificateHashData = certificate;

    // Send request
    DeleteCertificateConf resp;
    CallResult            res = m_msg_sender.call(DELETE_CERTIFICATE_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = resp.status;
        LOG_INFO << "[" << m_identifier << "] - Delete certificate : " << DeleteCertificateStatusEnumTypeHelper.toString(resp.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc ocpp::types::ocpp16::TriggerMessageStatusEnumType ICentralSystem::IChargePoint::extendedTriggerMessage(ocpp::types::ocpp16::MessageTriggerEnumType,
                                                                                                            const ocpp::types::Optional<unsigned int>) */
ocpp::types::ocpp16::TriggerMessageStatusEnumType ChargePointProxy::extendedTriggerMessage(
    ocpp::types::ocpp16::MessageTriggerEnumType message, const ocpp::types::Optional<unsigned int> connector_id)
{
    TriggerMessageStatusEnumType ret = TriggerMessageStatusEnumType::Rejected;

    LOG_INFO << "[" << m_identifier
             << "] - Extended trigger message : requestedMessage = " << MessageTriggerEnumTypeHelper.toString(message)
             << " - connectorId = " << (connector_id.isSet() ? std::to_string(connector_id) : "not set");

    // Prepare request
    ExtendedTriggerMessageReq req;
    req.requestedMessage = message;
    req.connectorId      = connector_id;

    // Send request
    ExtendedTriggerMessageConf resp;
    CallResult                 res = m_msg_sender.call(EXTENDED_TRIGGER_MESSAGE_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = resp.status;
        LOG_INFO << "[" << m_identifier << "] - Extended trigger message : " << TriggerMessageStatusEnumTypeHelper.toString(resp.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc bool ICentralSystem::IChargePoint::getInstalledCertificateIds(ocpp::types::ocpp16::CertificateUseEnumType,
                                                                           std::vector<ocpp::types::ocpp16::CertificateHashDataType>&) */
bool ChargePointProxy::getInstalledCertificateIds(ocpp::types::ocpp16::CertificateUseEnumType                type,
                                                  std::vector<ocpp::types::ocpp16::CertificateHashDataType>& certificates)
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier
             << "] - Get installed certificate ids : certificateType = " << CertificateUseEnumTypeHelper.toString(type);

    // Prepare request
    GetInstalledCertificateIdsReq req;
    req.certificateType = type;

    // Send request
    GetInstalledCertificateIdsConf resp;
    CallResult                     res = m_msg_sender.call(GET_INSTALLED_CERTIFICATE_IDS_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        LOG_INFO << "[" << m_identifier
                 << "] - Get installed certificate ids : status = " << GetInstalledCertificateStatusEnumTypeHelper.toString(resp.status)
                 << " - count = " << resp.certificateHashData.size();
        certificates = resp.certificateHashData;
        ret          = true;
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc bool ICentralSystem::IChargePoint::getLog(ocpp::types::ocpp16::LogEnumType,
                                                           int,
                                                           const std::string&,
                                                           const ocpp::types::Optional<unsigned int>&,
                                                           const ocpp::types::Optional<std::chrono::seconds>&,
                                                           const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                           const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                           std::string&) */
bool ChargePointProxy::getLog(ocpp::types::ocpp16::LogEnumType                    type,
                              int                                                 request_id,
                              const std::string&                                  uri,
                              const ocpp::types::Optional<unsigned int>&          retries,
                              const ocpp::types::Optional<std::chrono::seconds>&  retry_interval,
                              const ocpp::types::Optional<ocpp::types::DateTime>& start,
                              const ocpp::types::Optional<ocpp::types::DateTime>& stop,
                              std::string&                                        log_filename)
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier << "] - Get log : type = " << LogEnumTypeHelper.toString(type) << " - request_id = " << request_id
             << " - location = " << uri << " - retries = " << (retries.isSet() ? std::to_string(retries) : "not set")
             << " - retryInterval = " << (retry_interval.isSet() ? std::to_string(retry_interval.value().count()) : "not set")
             << " - startTime = " << (start.isSet() ? start.value().str() : "not set")
             << " - stopTime = " << (stop.isSet() ? stop.value().str() : "not set");

    // Prepare request
    GetLogReq req;
    req.logType   = type;
    req.requestId = request_id;
    req.retries   = retries;
    if (retry_interval.isSet())
    {
        req.retryInterval = static_cast<unsigned int>(retry_interval.value().count());
    }
    req.log.remoteLocation.assign(uri);
    req.log.oldestTimestamp = start;
    req.log.latestTimestamp = stop;

    // Send request
    GetLogConf resp;
    CallResult res = m_msg_sender.call(GET_LOG_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret          = true;
        log_filename = resp.fileName;
        LOG_INFO << "[" << m_identifier << "] - Get log : status = " << LogStatusEnumTypeHelper.toString(resp.status)
                 << " - filename = " << resp.fileName.str();
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc ocpp::types::ocpp16::CertificateStatusEnumType ICentralSystem::installCertificate(ocpp::types::ocpp16::CertificateUseEnumType,
                                                                                           const ocpp::x509::Certificate&) */
ocpp::types::ocpp16::CertificateStatusEnumType ChargePointProxy::installCertificate(ocpp::types::ocpp16::CertificateUseEnumType type,
                                                                                    const ocpp::x509::Certificate&              certificate)
{
    CertificateStatusEnumType ret = CertificateStatusEnumType::Rejected;

    LOG_INFO << "[" << m_identifier << "] - Install certificate : certificateType = " << CertificateUseEnumTypeHelper.toString(type)
             << " - certificate subject = " << certificate.subjectString();

    // Prepare request
    InstallCertificateReq req;
    req.certificateType = type;
    req.certificate.assign(certificate.pem());

    // Send request
    InstallCertificateConf resp;
    CallResult             res = m_msg_sender.call(INSTALL_CERTIFICATE_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = resp.status;
        LOG_INFO << "[" << m_identifier << "] - Install certificate : " << CertificateStatusEnumTypeHelper.toString(resp.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

/** @copydoc ocpp::types::ocpp16::UpdateFirmwareStatusEnumType ICentralSystem::signedUpdateFirmware(
                                                                  int,
                                                                  const std::string&,
                                                                  const ocpp::types::Optional<unsigned int>&,
                                                                  const ocpp::types::DateTime&,
                                                                  const ocpp::types::Optional<std::chrono::seconds>&,
                                                                  const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                                  const ocpp::x509::Certificate&,
                                                                  const std::string&) */
ocpp::types::ocpp16::UpdateFirmwareStatusEnumType ChargePointProxy::signedUpdateFirmware(
    int                                                 request_id,
    const std::string&                                  uri,
    const ocpp::types::Optional<unsigned int>&          retries,
    const ocpp::types::DateTime&                        retrieve_date,
    const ocpp::types::Optional<std::chrono::seconds>&  retry_interval,
    const ocpp::types::Optional<ocpp::types::DateTime>& install_date,
    const ocpp::x509::Certificate&                      signing_certificate,
    const std::string&                                  signature)
{
    UpdateFirmwareStatusEnumType ret = UpdateFirmwareStatusEnumType::Rejected;

    LOG_INFO << "[" << m_identifier << "] - Signed firmware update : requestId = " << request_id << " - location = " << uri
             << " - retries = " << (retries.isSet() ? std::to_string(retries) : "not set")
             << " - retrieveDateTime = " << retrieve_date.str()
             << " - retryInterval = " << (retry_interval.isSet() ? std::to_string(retry_interval.value().count()) : "not set")
             << " - installDateTime = " << (install_date.isSet() ? install_date.value().str() : "not set")
             << " - signingCertificate subject = " << signing_certificate.subjectString() << " - signature = " << signature;

    // Prepare request
    SignedUpdateFirmwareReq req;
    req.requestId = request_id;
    req.retries   = retries;
    if (retry_interval.isSet())
    {
        req.retryInterval = static_cast<unsigned int>(retry_interval.value().count());
    }
    req.firmware.location.assign(uri);
    req.firmware.retrieveDateTime = retrieve_date;
    req.firmware.installDateTime  = install_date;
    req.firmware.signingCertificate.assign(signing_certificate.pem());
    req.firmware.signature.assign(signature);

    // Send request
    SignedUpdateFirmwareConf resp;
    CallResult               res = m_msg_sender.call(SIGNED_UPDATE_FIRMWARE_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret = resp.status;
        LOG_INFO << "[" << m_identifier << "] - Signed firmware update : " << UpdateFirmwareStatusEnumTypeHelper.toString(resp.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - Call failed";
    }

    return ret;
}

// ISO 15118 PnC extensions

/** @copydoc bool ICentralSystem::IChargePoint::iso15118CertificateSigned(const ocpp::x509::Certificate&) */
bool ChargePointProxy::iso15118CertificateSigned(const ocpp::x509::Certificate& certificate_chain)
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier
             << "] - [ISO15118] Certificate signed : certificate chain size = " << certificate_chain.pemChain().size();

    // Prepare request
    CertificateSignedReq request;
    request.certificateChain.assign(certificate_chain.pem());

    // Send request
    CertificateSignedConf response;
    if (send("CertificateSigned", CERTIFICATE_SIGNED_ACTION, request, response))
    {
        // Extract response
        ret = (response.status == CertificateSignedStatusEnumType::Accepted);
        LOG_INFO << "[" << m_identifier
                 << "] - [ISO15118] Certificate signed : " << CertificateSignedStatusEnumTypeHelper.toString(response.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - [ISO15118] Call failed";
    }

    return ret;
}

/** @copydoc ocpp::types::ocpp16::DeleteCertificateStatusEnumType ICentralSystem::IChargePoint::iso15118DeleteCertificate(
                                const ocpp::types::ocpp16::CertificateHashDataType&) */
ocpp::types::ocpp16::DeleteCertificateStatusEnumType ChargePointProxy::iso15118DeleteCertificate(
    const ocpp::types::ocpp16::CertificateHashDataType& certificate)
{
    DeleteCertificateStatusEnumType ret = DeleteCertificateStatusEnumType::Failed;

    LOG_INFO << "[" << m_identifier << "] - [ISO15118] Delete certificate : serialNumber = " << certificate.serialNumber.str();

    // Prepare request
    DeleteCertificateReq request;
    request.certificateHashData = certificate;

    // Send request
    DeleteCertificateConf response;
    if (send("DeleteCertificate", DELETE_CERTIFICATE_ACTION, request, response))
    {
        // Extract response
        ret = response.status;
        LOG_INFO << "[" << m_identifier
                 << "] - [ISO15118] Delete certificate : " << DeleteCertificateStatusEnumTypeHelper.toString(response.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - [ISO15118] Call failed";
    }

    return ret;
}

/** @copydoc bool ICentralSystem::IChargePoint::iso15118GetInstalledCertificateIds(ocpp::types::ocpp16::GetCertificateIdUseEnumType,
                                                                                       std::vector<ocpp::types::ocpp16::CertificateHashDataChainType>&) */
bool ChargePointProxy::iso15118GetInstalledCertificateIds(const std::vector<ocpp::types::ocpp16::GetCertificateIdUseEnumType>& types,
                                                          std::vector<ocpp::types::ocpp16::CertificateHashDataChainType>&      certificates)
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier << "] - [ISO15118] Get installed certificate ids : certificateType size = " << types.size();

    // Prepare request
    Iso15118GetInstalledCertificateIdsReq request;
    request.certificateType = types;

    // Send request
    Iso15118GetInstalledCertificateIdsConf response;
    if (send("Iso15118GetInstalledCertificateIds", ISO15118_GET_INSTALLED_CERTIFICATE_IDS_ACTION, request, response))
    {
        // Extract response
        LOG_INFO << "[" << m_identifier << "] - [ISO15118] Get installed certificate ids : status = "
                 << GetInstalledCertificateStatusEnumTypeHelper.toString(response.status)
                 << " - count = " << response.certificateHashDataChain.size();
        certificates = response.certificateHashDataChain;
        ret          = true;
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - [ISO15118] Call failed";
    }

    return ret;
}

/** @copydoc ocpp::types::ocpp16::InstallCertificateStatusEnumType ICentralSystem::IChargePoint::iso15118CertificateSigned(
                                                                ocpp::types::ocpp16::InstallCertificateUseEnumType,
                                                                const ocpp::x509::Certificate&) */
ocpp::types::ocpp16::InstallCertificateStatusEnumType ChargePointProxy::iso15118InstallCertificate(
    ocpp::types::ocpp16::InstallCertificateUseEnumType type, const ocpp::x509::Certificate& certificate)
{
    InstallCertificateStatusEnumType ret = InstallCertificateStatusEnumType::Rejected;

    LOG_INFO << "[" << m_identifier
             << "] - [ISO15118] Install certificate : certificateType = " << InstallCertificateUseEnumTypeHelper.toString(type)
             << " - certificate subject = " << certificate.subjectString();

    // Prepare request
    Iso15118InstallCertificateReq request;
    request.certificateType = type;
    request.certificate.assign(certificate.pem());

    // Send request
    Iso15118InstallCertificateConf response;
    if (send("Iso15118InstallCertificate", ISO15118_INSTALL_CERTIFICATE_ACTION, request, response))
    {
        // Extract response
        ret = response.status;
        LOG_INFO << "[" << m_identifier
                 << "] - [ISO15118] Install certificate : " << InstallCertificateStatusEnumTypeHelper.toString(response.status);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - [ISO15118] Call failed";
    }

    return ret;
}

/** @copydoc bool ICentralSystem::IChargePoint::iso15118TriggerSignCertificate() */
bool ChargePointProxy::iso15118TriggerSignCertificate()
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier << "] - [ISO15118] Trigger sign certificate";

    // Prepare request
    Iso15118TriggerMessageReq request;

    // Send request
    Iso15118TriggerMessageConf response;
    if (send("Iso15118TriggerMessage", ISO15118_TRIGGER_MESSAGE_ACTION, request, response))
    {
        // Extract response
        LOG_INFO << "[" << m_identifier
                 << "] - [ISO15118] Trigger sign certificate : status = " << TriggerMessageStatusEnumTypeHelper.toString(response.status);
        ret = (response.status == TriggerMessageStatusEnumType::Accepted);
    }
    else
    {
        LOG_ERROR << "[" << m_identifier << "] - [ISO15118] Call failed";
    }

    return ret;
}

// IRpc::IListener interface

/** @copydoc void IRpc::IListener::rpcDisconnected() */
void ChargePointProxy::rpcDisconnected()
{
    LOG_WARNING << "[" << m_identifier << "] - Disconnected";
    if (m_user_handler)
    {
        m_user_handler->disconnected();
    }
}

/** @copydoc void IRpc::IListener::rpcError() */
void ChargePointProxy::rpcError()
{
    LOG_ERROR << "[" << m_identifier << "] - Connection error";
}

/** @copydoc bool IRpc::IListener::rpcCallReceived(const std::string&,
                                                       const rapidjson::Value&,
                                                       rapidjson::Document&,
                                                       std::string&,
                                                       std::string&) */
bool ChargePointProxy::rpcCallReceived(const std::string&      action,
                                       const rapidjson::Value& payload,
                                       rapidjson::Document&    response,
                                       std::string&            error_code,
                                       std::string&            error_message)
{
    return m_msg_dispatcher.dispatchMessage(action, payload, response, error_code, error_message);
}

// IRpc::ISpy interface

/** @copydoc void IRpc::ISpy::rcpMessageReceived(const std::string& msg) */
void ChargePointProxy::rcpMessageReceived(const std::string& msg)
{
    LOG_COM << "[" << m_identifier << "] - RX : " << msg;
}

void ChargePointProxy::rcpMessageSent(const std::string& msg)
{
    LOG_COM << "[" << m_identifier << "] - TX : " << msg;
}

} // namespace centralsystem
} // namespace ocpp
