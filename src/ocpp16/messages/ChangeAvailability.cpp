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

#include "ChangeAvailability.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{
/** @brief Helper to convert a AvailabilityType enum to string */
const EnumToStringFromString<AvailabilityType> AvailabilityTypeHelper = {{AvailabilityType::Inoperative, "Inoperative"},
                                                                         {AvailabilityType::Operative, "Operative"}};

/** @brief Helper to convert a AvailabilityStatus enum to string */
const EnumToStringFromString<AvailabilityStatus> AvailabilityStatusHelper = {
    {AvailabilityStatus::Accepted, "Accepted"}, {AvailabilityStatus::Rejected, "Rejected"}, {AvailabilityStatus::Scheduled, "Scheduled"}};

} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool ChangeAvailabilityReqConverter::fromJson(const rapidjson::Value& json,
                                              ChangeAvailabilityReq&  data,
                                              std::string&            error_code,
                                              std::string&            error_message)
{
    bool ret;
    ret       = extract(json, "connectorId", data.connectorId, error_message);
    data.type = AvailabilityTypeHelper.fromString(json["type"].GetString());
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool ChangeAvailabilityReqConverter::toJson(const ChangeAvailabilityReq& data, rapidjson::Document& json)
{
    fill(json, "connectorId", data.connectorId);
    fill(json, "type", AvailabilityTypeHelper.toString(data.type));
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool ChangeAvailabilityConfConverter::fromJson(const rapidjson::Value& json,
                                               ChangeAvailabilityConf& data,
                                               std::string&            error_code,
                                               std::string&            error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = AvailabilityStatusHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool ChangeAvailabilityConfConverter::toJson(const ChangeAvailabilityConf& data, rapidjson::Document& json)
{
    fill(json, "status", AvailabilityStatusHelper.toString(data.status));
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
