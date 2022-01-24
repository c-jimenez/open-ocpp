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

#include "ChargePointProxy.h"
#include "CancelReservation.h"
#include "ChangeAvailability.h"
#include "ChangeConfiguration.h"
#include "ClearCache.h"
#include "ClearChargingProfile.h"
#include "DataTransfer.h"
#include "GetCompositeSchedule.h"
#include "GetConfiguration.h"
#include "GetDiagnostics.h"
#include "GetLocalListVersion.h"
#include "Logger.h"
#include "RemoteStartTransaction.h"
#include "RemoteStopTransaction.h"
#include "ReserveNow.h"
#include "Reset.h"
#include "SendLocalList.h"
#include "SetChargingProfile.h"
#include "TriggerMessage.h"
#include "UnlockConnector.h"
#include "UpdateFirmware.h"

using namespace ocpp::types;
using namespace ocpp::messages;

namespace ocpp
{
namespace centralsystem
{

/** @brief Constructor */
ChargePointProxy::ChargePointProxy(const std::string&                            identifier,
                                   std::shared_ptr<ocpp::rpc::RpcServer::Client> rpc,
                                   const std::string&                            schemas_path,
                                   ocpp::messages::MessagesConverter&            messages_converter,
                                   std::chrono::milliseconds                     timeout)
    : m_identifier(identifier), m_rpc(rpc), m_msg_dispatcher(schemas_path), m_msg_sender(*m_rpc, messages_converter, timeout)
{
    m_rpc->registerSpy(*this);
    m_rpc->registerListener(*this);
}

/** @brief Destructor */
ChargePointProxy::~ChargePointProxy() { }

// ICentralSystem::IChargePoint interface

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

/** @copydoc ocpp::types::AvailabilityStatus ICentralSystem::IChargePoint::changeAvailability(int, ocpp::types::AvailabilityType) */
ocpp::types::AvailabilityStatus ChargePointProxy::changeAvailability(int connector_id, ocpp::types::AvailabilityType availability)
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

/** @copydoc ocpp::types::ConfigurationStatus ICentralSystem::IChargePoint::changeConfiguration(const std::string&, const std::string&) */
ocpp::types::ConfigurationStatus ChargePointProxy::changeConfiguration(const std::string& key, const std::string& value)
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
                                                                     const ocpp::types::Optional<ocpp::types::ChargingProfilePurposeType>&,
                                                                     const ocpp::types::Optional<unsigned int>&) */
bool ChargePointProxy::clearChargingProfile(const ocpp::types::Optional<int>&                                     profile_id,
                                            const ocpp::types::Optional<unsigned int>&                            connector_id,
                                            const ocpp::types::Optional<ocpp::types::ChargingProfilePurposeType>& purpose,
                                            const ocpp::types::Optional<unsigned int>&                            stack_level)
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
                                                             ocpp::types::DataTransferStatus&,
                                                             std::string&) */
bool ChargePointProxy::dataTransfer(const std::string&               vendor_id,
                                    const std::string&               message_id,
                                    const std::string&               request_data,
                                    ocpp::types::DataTransferStatus& status,
                                    std::string&                     response_data)
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
                                                                     const ocpp::types::Optional<ocpp::types::ChargingRateUnitType>&,
                                                                     ocpp::types::Optional<unsigned int>&,
                                                                     ocpp::types::Optional<ocpp::types::DateTime>&,
                                                                     ocpp::types::Optional<ocpp::types::ChargingSchedule>&) */
bool ChargePointProxy::getCompositeSchedule(unsigned int                                                    connector_id,
                                            std::chrono::seconds                                            duration,
                                            const ocpp::types::Optional<ocpp::types::ChargingRateUnitType>& unit,
                                            ocpp::types::Optional<unsigned int>&                            schedule_connector_id,
                                            ocpp::types::Optional<ocpp::types::DateTime>&                   schedule_start,
                                            ocpp::types::Optional<ocpp::types::ChargingSchedule>&           schedule)
{
    bool ret = false;

    LOG_INFO << "[" << m_identifier << "] - Get composite schedule : connectorId = " << connector_id << " - duration = " << duration.count()
             << " - unit = " << (unit.isSet() ? ChargingRateUnitTypeHelper.toString(unit) : "not set");

    // Prepare request
    GetCompositeScheduleReq req;
    req.connectorId      = connector_id;
    req.duration         = duration.count();
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
                                                                 std::vector<ocpp::types::KeyValue>&,
                                                                 std::vector<std::string>&) */
bool ChargePointProxy::getConfiguration(const std::vector<std::string>&     keys,
                                        std::vector<ocpp::types::KeyValue>& config_keys,
                                        std::vector<std::string>&           unknown_keys)
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
             << " - retry_interval = " << (retry_interval.isSet() ? std::to_string(retry_interval.value().count()) : "not set")
             << " - startTime = " << (start.isSet() ? start.value().str() : "not set")
             << " - stopTime = " << (stop.isSet() ? stop.value().str() : "not set");

    // Prepare request
    GetDiagnosticsReq req;
    req.location = uri;
    req.retries  = retries;
    if (retry_interval.isSet())
    {
        req.retryInterval = retry_interval.value().count();
    }
    req.startTime = start;
    req.stopTime  = stop;

    // Send request
    GetDiagnosticsConf resp;
    CallResult         res = m_msg_sender.call(GET_DIAGNOSTICS_ACTION, req, resp);
    if (res == CallResult::Ok)
    {
        ret                = true;
        diagnotic_filename = resp.fileName;
        LOG_INFO << "[" << m_identifier << "] - Get diagnostics : filename = " << resp.fileName.str();
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
                                                                       const ocpp::types::Optional<ocpp::types::ChargingProfile>&) */
bool ChargePointProxy::remoteStartTransaction(const ocpp::types::Optional<unsigned int>&                 connector_id,
                                              const std::string&                                         id_tag,
                                              const ocpp::types::Optional<ocpp::types::ChargingProfile>& profile)
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

/** @copydoc ocpp::types::ReservationStatus ICentralSystem::IChargePoint::reserveNow(unsigned int,
                                                                                     const ocpp::types::DateTime&,
                                                                                     const std::string&,
                                                                                     const std::string&,
                                                                                     int)*/
ocpp::types::ReservationStatus ChargePointProxy::reserveNow(unsigned int                 connector_id,
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

/** @copydoc bool ICentralSystem::IChargePoint::reset(ocpp::types::ResetType) */
bool ChargePointProxy::reset(ocpp::types::ResetType type)
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

/** @copydoc ocpp::types::UpdateStatus ICentralSystem::IChargePoint::sendLocalList(int,
                                                                                   const std::vector<ocpp::types::AuthorizationData>&,
                                                                                   ocpp::types::UpdateType) */
ocpp::types::UpdateStatus ChargePointProxy::sendLocalList(int                                                version,
                                                          const std::vector<ocpp::types::AuthorizationData>& authorization_list,
                                                          ocpp::types::UpdateType                            update_type)
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

/** @copydoc ocpp::types::ChargingProfileStatus ICentralSystem::IChargePoint::setChargingProfile(unsigned int,
                                                                                                 const ocpp::types::ChargingProfile&) */
ocpp::types::ChargingProfileStatus ChargePointProxy::setChargingProfile(unsigned int                        connector_id,
                                                                        const ocpp::types::ChargingProfile& profile)
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

/** @copydoc ocpp::types::TriggerMessageStatus ICentralSystem::IChargePoint::triggerMessage(ocpp::types::MessageTrigger,
                                                                                                const ocpp::types::Optional<unsigned int>) */
ocpp::types::TriggerMessageStatus ChargePointProxy::triggerMessage(ocpp::types::MessageTrigger               message,
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

/** @copydoc ocpp::types::UnlockStatus ICentralSystem::IChargePoint::unlockConnector(unsigned int) */
ocpp::types::UnlockStatus ChargePointProxy::unlockConnector(unsigned int connector_id)
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
             << " - retry_interval = " << (retry_interval.isSet() ? std::to_string(retry_interval.value().count()) : "not set");

    // Prepare request
    UpdateFirmwareReq req;
    req.location     = uri;
    req.retries      = retries;
    req.retrieveDate = retrieve_date;
    if (retry_interval.isSet())
    {
        req.retryInterval = retry_interval.value().count();
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

// IRpc::IListener interface

/** @copydoc void IRpc::IListener::rpcDisconnected() */
void ChargePointProxy::rpcDisconnected()
{
    LOG_WARNING << "[" << m_identifier << "] - Disconnected";
}

/** @copydoc void IRpc::IListener::rpcError() */
void ChargePointProxy::rpcError()
{
    LOG_ERROR << "[" << m_identifier << "] - Connection error";
}

/** @copydoc bool IRpc::IListener::rpcCallReceived(const std::string&,
                                                       const rapidjson::Value&,
                                                       rapidjson::Document&,
                                                       const char*&,
                                                       std::string&) */
bool ChargePointProxy::rpcCallReceived(const std::string&      action,
                                       const rapidjson::Value& payload,
                                       rapidjson::Document&    response,
                                       const char*&            error_code,
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
