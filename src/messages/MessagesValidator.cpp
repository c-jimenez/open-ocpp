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

#include "MessagesValidator.h"
#include "Logger.h"

#include "Authorize.h"
#include "BootNotification.h"
#include "CancelReservation.h"
#include "CertificateSigned.h"
#include "ChangeAvailability.h"
#include "ChangeConfiguration.h"
#include "ClearCache.h"
#include "ClearChargingProfile.h"
#include "DataTransfer.h"
#include "DeleteCertificate.h"
#include "DiagnosticsStatusNotification.h"
#include "ExtendedTriggerMessage.h"
#include "FirmwareStatusNotification.h"
#include "GetCompositeSchedule.h"
#include "GetConfiguration.h"
#include "GetDiagnostics.h"
#include "GetInstalledCertificateIds.h"
#include "GetLocalListVersion.h"
#include "GetLog.h"
#include "Heartbeat.h"
#include "InstallCertificate.h"
#include "LogStatusNotification.h"
#include "MeterValues.h"
#include "RemoteStartTransaction.h"
#include "RemoteStopTransaction.h"
#include "ReserveNow.h"
#include "Reset.h"
#include "SecurityEventNotification.h"
#include "SendLocalList.h"
#include "SetChargingProfile.h"
#include "SignCertificate.h"
#include "SignedFirmwareStatusNotification.h"
#include "SignedUpdateFirmware.h"
#include "StartTransaction.h"
#include "StatusNotification.h"
#include "StopTransaction.h"
#include "TriggerMessage.h"
#include "UnlockConnector.h"
#include "UpdateFirmware.h"

namespace ocpp
{
namespace messages
{
/** @brief Constructor */
MessagesValidator::MessagesValidator() : m_req_validators(), m_resp_validators() { }

/** @brief Destructor */
MessagesValidator::~MessagesValidator() { }

/** @brief Load the messages validators */
bool MessagesValidator::load(const std::string& schemas_path)
{
    bool ret = true;

    // Load validators for all the messages
    m_req_validators.clear();
    m_resp_validators.clear();
    ret = addValidator(schemas_path, AUTHORIZE_ACTION) && ret;
    ret = addValidator(schemas_path, BOOT_NOTIFICATION_ACTION) && ret;
    ret = addValidator(schemas_path, CANCEL_RESERVATION_ACTION) && ret;
    ret = addValidator(schemas_path, CERTIFICATE_SIGNED_ACTION) && ret;
    ret = addValidator(schemas_path, CHANGE_AVAILABILITY_ACTION) && ret;
    ret = addValidator(schemas_path, CHANGE_CONFIGURATION_ACTION) && ret;
    ret = addValidator(schemas_path, CLEAR_CACHE_ACTION) && ret;
    ret = addValidator(schemas_path, CLEAR_CHARGING_PROFILE_ACTION) && ret;
    ret = addValidator(schemas_path, DATA_TRANSFER_ACTION) && ret;
    ret = addValidator(schemas_path, DELETE_CERTIFICATE_ACTION) && ret;
    ret = addValidator(schemas_path, DIAGNOSTIC_STATUS_NOTIFICATION_ACTION) && ret;
    ret = addValidator(schemas_path, EXTENDED_TRIGGER_MESSAGE_ACTION) && ret;
    ret = addValidator(schemas_path, FIRMWARE_STATUS_NOTIFICATION_ACTION) && ret;
    ret = addValidator(schemas_path, GET_COMPOSITE_SCHEDULE_ACTION) && ret;
    ret = addValidator(schemas_path, GET_CONFIGURATION_ACTION) && ret;
    ret = addValidator(schemas_path, GET_DIAGNOSTICS_ACTION) && ret;
    ret = addValidator(schemas_path, GET_INSTALLED_CERTIFICATE_IDS_ACTION) && ret;
    ret = addValidator(schemas_path, GET_LOCAL_LIST_VERSION_ACTION) && ret;
    ret = addValidator(schemas_path, GET_LOG_ACTION) && ret;
    ret = addValidator(schemas_path, HEARTBEAT_ACTION) && ret;
    ret = addValidator(schemas_path, INSTALL_CERTIFICATE_ACTION) && ret;
    ret = addValidator(schemas_path, LOG_STATUS_NOTIFICATION_ACTION) && ret;
    ret = addValidator(schemas_path, METER_VALUES_ACTION) && ret;
    ret = addValidator(schemas_path, REMOTE_START_TRANSACTION_ACTION) && ret;
    ret = addValidator(schemas_path, REMOTE_STOP_TRANSACTION_ACTION) && ret;
    ret = addValidator(schemas_path, RESERVE_NOW_ACTION) && ret;
    ret = addValidator(schemas_path, RESET_ACTION) && ret;
    ret = addValidator(schemas_path, SECURITY_EVENT_NOTIFICATION_ACTION) && ret;
    ret = addValidator(schemas_path, SEND_LOCAL_LIST_ACTION) && ret;
    ret = addValidator(schemas_path, SET_CHARGING_PROFILE_ACTION) && ret;
    ret = addValidator(schemas_path, SIGN_CERTIFICATE_ACTION) && ret;
    ret = addValidator(schemas_path, SIGNED_FIRMWARE_STATUS_NOTIFICATION_ACTION) && ret;
    ret = addValidator(schemas_path, SIGNED_UPDATE_FIRMWARE_ACTION) && ret;
    ret = addValidator(schemas_path, START_TRANSACTION_ACTION) && ret;
    ret = addValidator(schemas_path, STATUS_NOTIFICATION_ACTION) && ret;
    ret = addValidator(schemas_path, STOP_TRANSACTION_ACTION) && ret;
    ret = addValidator(schemas_path, TRIGGER_MESSAGE_ACTION) && ret;
    ret = addValidator(schemas_path, UNLOCK_CONNECTOR_ACTION) && ret;
    ret = addValidator(schemas_path, UPDATE_FIRMWARE_ACTION) && ret;

    return ret;
}

/** @brief Get the message validator corresponding to a given action */
ocpp::json::JsonValidator* MessagesValidator::getValidator(const std::string& action, bool is_req) const
{
    ocpp::json::JsonValidator* validator = nullptr;

    // Get validators list
    const std::unordered_map<std::string, std::shared_ptr<ocpp::json::JsonValidator>>* validators = &m_resp_validators;
    if (is_req)
    {
        validators = &m_req_validators;
    }

    auto iter = validators->find(action);
    if (iter != validators->cend())
    {
        validator = iter->second.get();
    }

    return validator;
}

/** @brief Add a message validator bfor both request and response */
bool MessagesValidator::addValidator(const std::string& schemas_path, const std::string& action)
{
    bool ret;

    // Add validator for request
    std::filesystem::path req_filepath(schemas_path);
    req_filepath.append(action + ".json");
    ret = addValidator(req_filepath, action, true);

    // Add validator for response
    std::filesystem::path resp_filepath(schemas_path);
    resp_filepath.append(action + "Response.json");
    ret = addValidator(resp_filepath, action, false) && ret;

    return ret;
}
/** @brief Add a message validator */
bool MessagesValidator::addValidator(const std::filesystem::path& validator_path, const std::string& action, bool is_req)
{
    bool ret = false;

    // Load validator
    std::shared_ptr<ocpp::json::JsonValidator> validator = std::make_shared<ocpp::json::JsonValidator>();
    if (validator->init(validator_path.string()))
    {
        LOG_DEBUG << "[" << action << "] Validator loaded : " << validator_path;

        // Add validator
        if (is_req)
        {
            m_req_validators[action] = validator;
        }
        else
        {
            m_resp_validators[action] = validator;
        }

        ret = true;
    }
    else
    {
        LOG_ERROR << "[" << action << "] Unable to load validator : " << validator_path;
    }

    return ret;
}

} // namespace messages
} // namespace ocpp
