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

#include "SendLocalList.h"
#include "AuthorizationDataConverter.h"
#include "IRpcClient.h"

using namespace ocpp::types;

namespace ocpp
{
namespace types
{
/** @brief Helper to convert a UpdateType enum to string */
const EnumToStringFromString<UpdateType> UpdateTypeHelper = {{UpdateType::Differential, "Differential"}, {UpdateType::Full, "Full"}};

/** @brief Helper to convert a UpdateStatus enum to string */
const EnumToStringFromString<UpdateStatus> UpdateStatusHelper = {{UpdateStatus::Accepted, "Accepted"},
                                                                 {UpdateStatus::Failed, "Failed"},
                                                                 {UpdateStatus::NotSupported, "NotSupported"},
                                                                 {UpdateStatus::VersionMismatch, "VersionMismatch"}};
} // namespace types
namespace messages
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, const char*&, std::string&) */
bool SendLocalListReqConverter::fromJson(const rapidjson::Value& json,
                                         SendLocalListReq&       data,
                                         const char*&            error_code,
                                         std::string&            error_message)
{
    bool ret = true;
    extract(json, "listVersion", data.listVersion);
    if (json.HasMember(""))
    {
        AuthorizationDataConverter      authorization_data_converter;
        std::vector<AuthorizationData>& local_authorization_list = data.localAuthorizationList;
        const rapidjson::Value&         localAuthorizationList   = json["localAuthorizationList"];
        for (auto it_authorization_data = localAuthorizationList.Begin(); ret && (it_authorization_data != localAuthorizationList.End());
             ++it_authorization_data)
        {
            local_authorization_list.emplace_back();
            AuthorizationData& authorization_data = local_authorization_list.back();
            ret = ret && authorization_data_converter.fromJson(*it_authorization_data, authorization_data, error_code, error_message);
        }
    }
    data.updateType = UpdateTypeHelper.fromString(json["updateType"].GetString());
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, const char*&, std::string&) */
bool SendLocalListReqConverter::toJson(const SendLocalListReq& data, rapidjson::Document& json)
{
    bool ret = true;
    fill(json, "listVersion", data.listVersion);
    if (!data.localAuthorizationList.empty())
    {
        AuthorizationDataConverter authorization_data_converter;
        authorization_data_converter.setAllocator(allocator);
        rapidjson::Value localAuthorizationList(rapidjson::kArrayType);
        for (const AuthorizationData& authorization_data : data.localAuthorizationList)
        {
            rapidjson::Document value;
            value.Parse("{}");
            ret = ret && authorization_data_converter.toJson(authorization_data, value);
            localAuthorizationList.PushBack(value.Move(), *allocator);
        }
        json.AddMember(rapidjson::StringRef("localAuthorizationList"), localAuthorizationList.Move(), *allocator);
    }
    fill(json, "updateType", UpdateTypeHelper.toString(data.updateType));
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, const char*&, std::string&) */
bool SendLocalListConfConverter::fromJson(const rapidjson::Value& json,
                                          SendLocalListConf&      data,
                                          const char*&            error_code,
                                          std::string&            error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = UpdateStatusHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, const char*&, std::string&) */
bool SendLocalListConfConverter::toJson(const SendLocalListConf& data, rapidjson::Document& json)
{
    fill(json, "status", UpdateStatusHelper.toString(data.status));
    return true;
}

} // namespace messages
} // namespace ocpp
