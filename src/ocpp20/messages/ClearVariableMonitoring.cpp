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
    Generated by json2cpp from : urn:OCPP:Cp:2:2020:3:ClearVariableMonitoringRequest
                                 OCPP 2.0.1 FINAL
*/

#include "ClearVariableMonitoring.h"

#include "IRpc.h"

namespace ocpp
{
namespace messages
{
namespace ocpp20
{

/** @brief Convert a ClearVariableMonitoringReq from a JSON representation */
bool ClearVariableMonitoringReqConverter::fromJson(const rapidjson::Value&       json,
                                     ClearVariableMonitoringReq&                 data,
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

    // id
    const rapidjson::Value& id_json = json["id"];
    for (auto it = id_json.Begin(); ret && (it != id_json.End()); ++it)
    {
        int& item = data.id.emplace_back();
        item = it->GetInt();
    }

    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }

    return ret;
}

/** @brief Convert a ClearVariableMonitoringReq to a JSON representation */
bool ClearVariableMonitoringReqConverter::toJson(const ClearVariableMonitoringReq& data, rapidjson::Document& json) 
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

    // id
    if (!data.id.empty())
    {
        rapidjson::Value id_json(rapidjson::kArrayType);
        for (const int& item : data.id)
        {
            id_json.PushBack(rapidjson::Value(item), *allocator);
        }
        json.AddMember(rapidjson::StringRef("id"), id_json.Move(), *allocator);
    }

    return ret;
}

/** @brief Convert a ClearVariableMonitoringConf from a JSON representation */
bool ClearVariableMonitoringConfConverter::fromJson(const rapidjson::Value&       json,
                                     ClearVariableMonitoringConf&                 data,
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

    // clearMonitoringResult
    const rapidjson::Value& clearMonitoringResult_json = json["clearMonitoringResult"];
    ocpp::types::ocpp20::ClearMonitoringResultTypeConverter clearMonitoringResult_converter;
    for (auto it = clearMonitoringResult_json.Begin(); ret && (it != clearMonitoringResult_json.End()); ++it)
    {
        ocpp::types::ocpp20::ClearMonitoringResultType& item = data.clearMonitoringResult.emplace_back();
        ret = ret && clearMonitoringResult_converter.fromJson(*it, item, error_code, error_message);
    }

    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }

    return ret;
}

/** @brief Convert a ClearVariableMonitoringConf to a JSON representation */
bool ClearVariableMonitoringConfConverter::toJson(const ClearVariableMonitoringConf& data, rapidjson::Document& json) 
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

    // clearMonitoringResult
    if (!data.clearMonitoringResult.empty())
    {
        rapidjson::Value clearMonitoringResult_json(rapidjson::kArrayType);
        ocpp::types::ocpp20::ClearMonitoringResultTypeConverter clearMonitoringResult_converter;
        clearMonitoringResult_converter.setAllocator(allocator);
        for (const ocpp::types::ocpp20::ClearMonitoringResultType& item : data.clearMonitoringResult)
        {
            rapidjson::Document item_doc;
            item_doc.Parse("{}");
            ret = ret && clearMonitoringResult_converter.toJson(item, item_doc);
            clearMonitoringResult_json.PushBack(item_doc.Move(), *allocator);
        }
        json.AddMember(rapidjson::StringRef("clearMonitoringResult"), clearMonitoringResult_json.Move(), *allocator);
    }

    return ret;
}

} // namespace ocpp20
} // namespace messages
} // namespace ocpp