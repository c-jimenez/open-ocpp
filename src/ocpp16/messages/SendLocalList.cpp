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

#include "SendLocalList.h"
#include "AuthorizationDataConverter.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{
/** @brief Helper to convert a UpdateType enum to string */
const EnumToStringFromString<UpdateType> UpdateTypeHelper = {{UpdateType::Differential, "Differential"}, {UpdateType::Full, "Full"}};

/** @brief Helper to convert a UpdateStatus enum to string */
const EnumToStringFromString<UpdateStatus> UpdateStatusHelper = {{UpdateStatus::Accepted, "Accepted"},
                                                                 {UpdateStatus::Failed, "Failed"},
                                                                 {UpdateStatus::NotSupported, "NotSupported"},
                                                                 {UpdateStatus::VersionMismatch, "VersionMismatch"}};
} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool SendLocalListReqConverter::fromJson(const rapidjson::Value& json,
                                         SendLocalListReq&       data,
                                         std::string&            error_code,
                                         std::string&            error_message)
{
    bool ret = true;
    extract(json, "listVersion", data.listVersion);
    if (json.HasMember("localAuthorizationList"))
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

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
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
            rapidjson::Document value(rapidjson::kObjectType);
            ret = ret && authorization_data_converter.toJson(authorization_data, value);
            localAuthorizationList.PushBack(value.Move(), *allocator);
        }
        json.AddMember(rapidjson::StringRef("localAuthorizationList"), localAuthorizationList.Move(), *allocator);
    }
    fill(json, "updateType", UpdateTypeHelper.toString(data.updateType));
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool SendLocalListConfConverter::fromJson(const rapidjson::Value& json,
                                          SendLocalListConf&      data,
                                          std::string&            error_code,
                                          std::string&            error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = UpdateStatusHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool SendLocalListConfConverter::toJson(const SendLocalListConf& data, rapidjson::Document& json)
{
    fill(json, "status", UpdateStatusHelper.toString(data.status));
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
