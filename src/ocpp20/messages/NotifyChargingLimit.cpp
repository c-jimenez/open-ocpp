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

/*
    Generated by json2cpp from : urn:OCPP:Cp:2:2020:3:NotifyChargingLimitRequest
                                 OCPP 2.0.1 FINAL
*/

#include "NotifyChargingLimit.h"

#include "IRpc.h"

namespace ocpp
{
namespace messages
{
namespace ocpp20
{

/** @brief Convert a NotifyChargingLimitReq from a JSON representation */
bool NotifyChargingLimitReqConverter::fromJson(const rapidjson::Value&       json,
                                     NotifyChargingLimitReq&                 data,
                                     std::string&                  error_code,
                                     std::string&                  error_message)
{
    bool ret = true;

    // customData
    if (json.HasMember("customData"))
    {
    ocpp::types::ocpp20::CustomDataTypeConverter customData_converter;
    ret = ret && customData_converter.fromJson(json["customData"], data.customData, error_code, error_message);
    }

    // chargingSchedule
    const rapidjson::Value& chargingSchedule_json = json["chargingSchedule"];
    ocpp::types::ocpp20::ChargingScheduleTypeConverter chargingSchedule_converter;
    for (auto it = chargingSchedule_json.Begin(); ret && (it != chargingSchedule_json.End()); ++it)
    {
        ocpp::types::ocpp20::ChargingScheduleType& item = data.chargingSchedule.emplace_back();
        ret = ret && chargingSchedule_converter.fromJson(*it, item, error_code, error_message);
    }

    // evseId
    extract(json, "evseId", data.evseId);

    // chargingLimit
    ocpp::types::ocpp20::ChargingLimitTypeConverter chargingLimit_converter;
    ret = ret && chargingLimit_converter.fromJson(json["chargingLimit"], data.chargingLimit, error_code, error_message);

    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }

    return ret;
}

/** @brief Convert a NotifyChargingLimitReq to a JSON representation */
bool NotifyChargingLimitReqConverter::toJson(const NotifyChargingLimitReq& data, rapidjson::Document& json) 
{
    bool ret = true;

    // customData
    if (data.customData.isSet())
    {
    ocpp::types::ocpp20::CustomDataTypeConverter customData_converter;
    customData_converter.setAllocator(allocator);
    rapidjson::Document customData_doc;
    customData_doc.Parse("{}");
    ret = ret && customData_converter.toJson(data.customData, customData_doc);
    json.AddMember(rapidjson::StringRef("customData"), customData_doc.Move(), *allocator);
    }

    // chargingSchedule
    if (!data.chargingSchedule.empty())
    {
        rapidjson::Value chargingSchedule_json(rapidjson::kArrayType);
        ocpp::types::ocpp20::ChargingScheduleTypeConverter chargingSchedule_converter;
        chargingSchedule_converter.setAllocator(allocator);
        for (const ocpp::types::ocpp20::ChargingScheduleType& item : data.chargingSchedule)
        {
            rapidjson::Document item_doc;
            item_doc.Parse("{}");
            ret = ret && chargingSchedule_converter.toJson(item, item_doc);
            chargingSchedule_json.PushBack(item_doc.Move(), *allocator);
        }
        json.AddMember(rapidjson::StringRef("chargingSchedule"), chargingSchedule_json.Move(), *allocator);
    }

    // evseId
    fill(json, "evseId", data.evseId);

    // chargingLimit
    ocpp::types::ocpp20::ChargingLimitTypeConverter chargingLimit_converter;
    chargingLimit_converter.setAllocator(allocator);
    rapidjson::Document chargingLimit_doc;
    chargingLimit_doc.Parse("{}");
    ret = ret && chargingLimit_converter.toJson(data.chargingLimit, chargingLimit_doc);
    json.AddMember(rapidjson::StringRef("chargingLimit"), chargingLimit_doc.Move(), *allocator);

    return ret;
}

/** @brief Convert a NotifyChargingLimitConf from a JSON representation */
bool NotifyChargingLimitConfConverter::fromJson(const rapidjson::Value&       json,
                                     NotifyChargingLimitConf&                 data,
                                     std::string&                  error_code,
                                     std::string&                  error_message)
{
    bool ret = true;

    // customData
    if (json.HasMember("customData"))
    {
    ocpp::types::ocpp20::CustomDataTypeConverter customData_converter;
    ret = ret && customData_converter.fromJson(json["customData"], data.customData, error_code, error_message);
    }

    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }

    return ret;
}

/** @brief Convert a NotifyChargingLimitConf to a JSON representation */
bool NotifyChargingLimitConfConverter::toJson(const NotifyChargingLimitConf& data, rapidjson::Document& json) 
{
    bool ret = true;

    // customData
    if (data.customData.isSet())
    {
    ocpp::types::ocpp20::CustomDataTypeConverter customData_converter;
    customData_converter.setAllocator(allocator);
    rapidjson::Document customData_doc;
    customData_doc.Parse("{}");
    ret = ret && customData_converter.toJson(data.customData, customData_doc);
    json.AddMember(rapidjson::StringRef("customData"), customData_doc.Move(), *allocator);
    }

    return ret;
}

} // namespace ocpp20
} // namespace messages
} // namespace ocpp