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

#include "ExtendedTriggerMessage.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{
/** @brief Helper to convert a MessageTriggerEnumType enum to string */
const EnumToStringFromString<MessageTriggerEnumType> MessageTriggerEnumTypeHelper = {
    {MessageTriggerEnumType::BootNotification, "BootNotification"},
    {MessageTriggerEnumType::LogStatusNotification, "LogStatusNotification"},
    {MessageTriggerEnumType::FirmwareStatusNotification, "FirmwareStatusNotification"},
    {MessageTriggerEnumType::Heartbeat, "Heartbeat"},
    {MessageTriggerEnumType::MeterValues, "MeterValues"},
    {MessageTriggerEnumType::StatusNotification, "StatusNotification"},
    {MessageTriggerEnumType::SignChargePointCertificate, "SignChargePointCertificate"}};

/** @brief Helper to convert a TriggerMessageStatusEnumType enum to string */
const EnumToStringFromString<TriggerMessageStatusEnumType> TriggerMessageStatusEnumTypeHelper = {
    {TriggerMessageStatusEnumType::Accepted, "Accepted"},
    {TriggerMessageStatusEnumType::NotImplemented, "NotImplemented"},
    {TriggerMessageStatusEnumType::Rejected, "Rejected"}};

} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool ExtendedTriggerMessageReqConverter::fromJson(const rapidjson::Value&    json,
                                                  ExtendedTriggerMessageReq& data,
                                                  std::string&               error_code,
                                                  std::string&               error_message)
{
    bool ret;
    data.connectorId      = 0;
    ret                   = extract(json, "connectorId", data.connectorId, error_message);
    data.requestedMessage = MessageTriggerEnumTypeHelper.fromString(json["requestedMessage"].GetString());
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool ExtendedTriggerMessageReqConverter::toJson(const ExtendedTriggerMessageReq& data, rapidjson::Document& json)
{
    fill(json, "connectorId", data.connectorId);
    fill(json, "requestedMessage", MessageTriggerEnumTypeHelper.toString(data.requestedMessage));
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool ExtendedTriggerMessageConfConverter::fromJson(const rapidjson::Value&     json,
                                                   ExtendedTriggerMessageConf& data,
                                                   std::string&                error_code,
                                                   std::string&                error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = TriggerMessageStatusEnumTypeHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool ExtendedTriggerMessageConfConverter::toJson(const ExtendedTriggerMessageConf& data, rapidjson::Document& json)
{
    fill(json, "status", TriggerMessageStatusEnumTypeHelper.toString(data.status));
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
