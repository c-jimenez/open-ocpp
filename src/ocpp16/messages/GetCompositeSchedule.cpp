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

#include "GetCompositeSchedule.h"
#include "ChargingScheduleConverter.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{
/** @brief Helper to convert a enum class GetCompositeScheduleStatus enum to string */
const EnumToStringFromString<GetCompositeScheduleStatus> GetCompositeScheduleStatusHelper = {
    {GetCompositeScheduleStatus::Accepted, "Accepted"}, {GetCompositeScheduleStatus::Rejected, "Rejected"}};

} // namespace ocpp16
} // namespace types

namespace messages
{
namespace ocpp16
{
/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool GetCompositeScheduleReqConverter::fromJson(const rapidjson::Value&  json,
                                                GetCompositeScheduleReq& data,
                                                std::string&             error_code,
                                                std::string&             error_message)
{
    bool ret = extract(json, "connectorId", data.connectorId, error_message);
    ret      = ret && extract(json, "duration", data.duration, error_message);
    if (json.HasMember("chargingRateUnit"))
    {
        data.chargingRateUnit = ChargingRateUnitTypeHelper.fromString(json["chargingRateUnit"].GetString());
    }
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool GetCompositeScheduleReqConverter::toJson(const GetCompositeScheduleReq& data, rapidjson::Document& json)
{
    fill(json, "connectorId", data.connectorId);
    fill(json, "duration", data.duration);
    if (data.chargingRateUnit.isSet())
    {
        fill(json, "chargingRateUnit", ChargingRateUnitTypeHelper.toString(data.chargingRateUnit));
    }
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool GetCompositeScheduleConfConverter::fromJson(const rapidjson::Value&   json,
                                                 GetCompositeScheduleConf& data,
                                                 std::string&              error_code,
                                                 std::string&              error_message)
{
    data.status = GetCompositeScheduleStatusHelper.fromString(json["status"].GetString());
    bool ret    = extract(json, "connectorId", data.connectorId, error_message);
    ret         = ret && extract(json, "scheduleStart", data.scheduleStart, error_message);
    if (json.HasMember("chargingSchedule"))
    {
        ChargingScheduleConverter charging_schedule_converter;
        ret = charging_schedule_converter.fromJson(json["chargingSchedule"], data.chargingSchedule.value(), error_code, error_message);
    }
    if (!ret && error_code.empty())
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool GetCompositeScheduleConfConverter::toJson(const GetCompositeScheduleConf& data, rapidjson::Document& json)
{
    bool ret = true;
    fill(json, "status", GetCompositeScheduleStatusHelper.toString(data.status));
    fill(json, "connectorId", data.connectorId);
    fill(json, "scheduleStart", data.scheduleStart);
    if (data.chargingSchedule.isSet())
    {
        ChargingScheduleConverter charging_schedule_converter;
        charging_schedule_converter.setAllocator(allocator);

        rapidjson::Document value(rapidjson::kObjectType);
        ret = charging_schedule_converter.toJson(data.chargingSchedule, value);
        json.AddMember(rapidjson::StringRef("chargingSchedule"), value.Move(), *allocator);
    }
    return ret;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
