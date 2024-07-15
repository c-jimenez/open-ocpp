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

#include "TriggerMessage.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{
/** @brief Helper to convert a MessageTrigger enum to string */
const EnumToStringFromString<MessageTrigger> MessageTriggerHelper = {
    {MessageTrigger::BootNotification, "BootNotification"},
    {MessageTrigger::DiagnosticsStatusNotification, "DiagnosticsStatusNotification"},
    {MessageTrigger::FirmwareStatusNotification, "FirmwareStatusNotification"},
    {MessageTrigger::Heartbeat, "Heartbeat"},
    {MessageTrigger::MeterValues, "MeterValues"},
    {MessageTrigger::StatusNotification, "StatusNotification"}};

/** @brief Helper to convert a TriggerMessageStatus enum to string */
const EnumToStringFromString<TriggerMessageStatus> TriggerMessageStatusHelper = {{TriggerMessageStatus::Accepted, "Accepted"},
                                                                                 {TriggerMessageStatus::NotImplemented, "NotImplemented"},
                                                                                 {TriggerMessageStatus::Rejected, "Rejected"}};

} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool TriggerMessageReqConverter::fromJson(const rapidjson::Value& json,
                                          TriggerMessageReq&      data,
                                          std::string&            error_code,
                                          std::string&            error_message)
{
    bool ret;
    ret                   = extract(json, "connectorId", data.connectorId, error_message);
    data.requestedMessage = MessageTriggerHelper.fromString(json["requestedMessage"].GetString());
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool TriggerMessageReqConverter::toJson(const TriggerMessageReq& data, rapidjson::Document& json)
{
    fill(json, "connectorId", data.connectorId);
    fill(json, "requestedMessage", MessageTriggerHelper.toString(data.requestedMessage));
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool TriggerMessageConfConverter::fromJson(const rapidjson::Value& json,
                                           TriggerMessageConf&     data,
                                           std::string&            error_code,
                                           std::string&            error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = TriggerMessageStatusHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool TriggerMessageConfConverter::toJson(const TriggerMessageConf& data, rapidjson::Document& json)
{
    fill(json, "status", TriggerMessageStatusHelper.toString(data.status));
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
