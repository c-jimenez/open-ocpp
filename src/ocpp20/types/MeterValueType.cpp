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
    Generated by json2cpp from : urn:OCPP:Cp:2:2020:3:MeterValuesRequest
                                 OCPP 2.0.1 FINAL
*/

#include "MeterValueType.h"

#include "IRpc.h"

namespace ocpp
{
namespace types
{
namespace ocpp20
{

/** @brief Convert a MeterValueType from a JSON representation */
bool MeterValueTypeConverter::fromJson(const rapidjson::Value&       json,
                                      MeterValueType&                data,
                                      std::string&                  error_code,
                                      [[maybe_unused]] std::string& error_message)
{
    bool ret = true;

    // customData
    if (json.HasMember("customData"))
    {
    CustomDataTypeConverter customData_converter;
    ret = ret && customData_converter.fromJson(json["customData"], data.customData, error_code, error_message);
    }

    // sampledValue
    const rapidjson::Value& sampledValue_json = json["sampledValue"];
    SampledValueTypeConverter sampledValue_converter;
    for (auto it = sampledValue_json.Begin(); ret && (it != sampledValue_json.End()); ++it)
    {
        SampledValueType& item = data.sampledValue.emplace_back();
        ret = ret && sampledValue_converter.fromJson(*it, item, error_code, error_message);
    }

    // timestamp
    ret = ret && extract(json, "timestamp", data.timestamp, error_message);

    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }

    return ret;
}

/** @brief Convert a MeterValueType to a JSON representation */
bool MeterValueTypeConverter::toJson(const MeterValueType& data, rapidjson::Document& json) 
{
    bool ret = true;

    // customData
    if (data.customData.isSet())
    {
    CustomDataTypeConverter customData_converter;
    customData_converter.setAllocator(allocator);
    rapidjson::Document customData_doc;
    customData_doc.Parse("{}");
    ret = ret && customData_converter.toJson(data.customData, customData_doc);
    json.AddMember(rapidjson::StringRef("customData"), customData_doc.Move(), *allocator);
    }

    // sampledValue
    if (!data.sampledValue.empty())
    {
        rapidjson::Value sampledValue_json(rapidjson::kArrayType);
        SampledValueTypeConverter sampledValue_converter;
        sampledValue_converter.setAllocator(allocator);
        for (const SampledValueType& item : data.sampledValue)
        {
            rapidjson::Document item_doc;
            item_doc.Parse("{}");
            ret = ret && sampledValue_converter.toJson(item, item_doc);
            sampledValue_json.PushBack(item_doc.Move(), *allocator);
        }
        json.AddMember(rapidjson::StringRef("sampledValue"), sampledValue_json.Move(), *allocator);
    }

    // timestamp
    fill(json, "timestamp", data.timestamp);

    return ret;
}

} // namespace ocpp20
} // namespace types
} // namespace ocpp