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

#include "GetConfiguration.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace messages
{
namespace ocpp16
{
/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool GetConfigurationReqConverter::fromJson(const rapidjson::Value& json,
                                            GetConfigurationReq&    data,
                                            std::string&            error_code,
                                            std::string&            error_message)
{
    if (json.HasMember("key"))
    {
        CiStringType<50u>       key;
        const rapidjson::Value& keys      = json["key"];
        auto&                   data_keys = data.key.value();
        for (auto it = keys.Begin(); it != keys.End(); ++it)
        {
            key.assign(it->GetString());
            data_keys.push_back(std::move(key));
        }
    }
    (void)error_code;
    (void)error_message;
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool GetConfigurationReqConverter::toJson(const GetConfigurationReq& data, rapidjson::Document& json)
{
    if (data.key.isSet())
    {
        rapidjson::Value                    keys(rapidjson::kArrayType);
        rapidjson::Document::AllocatorType& _allocator = json.GetAllocator();
        for (const auto& key : data.key.value())
        {
            keys.PushBack(rapidjson::Value(key.c_str(), static_cast<rapidjson::SizeType>(key.str().size()), _allocator).Move(), _allocator);
        }
        json.AddMember(rapidjson::StringRef("key"), keys.Move(), _allocator);
    }
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool GetConfigurationConfConverter::fromJson(const rapidjson::Value& json,
                                             GetConfigurationConf&   data,
                                             std::string&            error_code,
                                             std::string&            error_message)
{
    (void)error_code;
    (void)error_message;
    if (json.HasMember("configurationKey"))
    {
        KeyValue                key_value;
        const rapidjson::Value& configurationKey = json["configurationKey"];
        auto&                   data_keys        = data.configurationKey.value();
        for (auto it = configurationKey.Begin(); it != configurationKey.End(); ++it)
        {
            extract(*it, "key", key_value.key);
            extract(*it, "readonly", key_value.readonly);
            extract(*it, "value", key_value.value);
            data_keys.push_back(key_value);
        }
    }
    if (json.HasMember("unknownKey"))
    {
        CiStringType<50u>       key;
        const rapidjson::Value& unknownKey = json["unknownKey"];
        auto&                   data_keys  = data.unknownKey.value();
        for (auto it = unknownKey.Begin(); it != unknownKey.End(); ++it)
        {
            key.assign(it->GetString());
            data_keys.push_back(std::move(key));
        }
    }
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool GetConfigurationConfConverter::toJson(const GetConfigurationConf& data, rapidjson::Document& json)
{
    if (data.configurationKey.isSet())
    {
        rapidjson::Value                    configurationKey(rapidjson::kArrayType);
        rapidjson::Document::AllocatorType& _allocator = json.GetAllocator();
        for (const KeyValue& key : data.configurationKey.value())
        {
            rapidjson::Document value(rapidjson::kObjectType);
            fill(value, "key", key.key);
            fill(value, "readonly", key.readonly);
            fill(value, "value", key.value);
            configurationKey.PushBack(value.Move(), _allocator);
        }
        json.AddMember(rapidjson::StringRef("configurationKey"), configurationKey.Move(), _allocator);
    }

    if (data.unknownKey.isSet())
    {
        rapidjson::Value                    unknownKey(rapidjson::kArrayType);
        rapidjson::Document::AllocatorType& _allocator = json.GetAllocator();
        for (const auto& key : data.unknownKey.value())
        {
            unknownKey.PushBack(rapidjson::Value(key.c_str(), static_cast<rapidjson::SizeType>(key.str().size()), _allocator).Move(),
                                _allocator);
        }
        json.AddMember(rapidjson::StringRef("unknownKey"), unknownKey.Move(), _allocator);
    }
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
