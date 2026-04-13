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

#include "LogStatusNotification.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{
/** @brief Helper to convert a enum class UploadLogStatusEnumType enum to string */
const EnumToStringFromString<UploadLogStatusEnumType> UploadLogStatusEnumTypeHelper = {
    {UploadLogStatusEnumType::BadMessage, "BadMessage"},
    {UploadLogStatusEnumType::Idle, "Idle"},
    {UploadLogStatusEnumType::NotSupportedOperation, "NotSupportedOperation"},
    {UploadLogStatusEnumType::PermissionDenied, "PermissionDenied"},
    {UploadLogStatusEnumType::Uploaded, "Uploaded"},
    {UploadLogStatusEnumType::UploadFailure, "UploadFailure"},
    {UploadLogStatusEnumType::Uploading, "Uploading"}};

} // namespace ocpp16
} // namespace types

namespace messages
{
namespace ocpp16
{
/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool LogStatusNotificationReqConverter::fromJson(const rapidjson::Value&   json,
                                                 LogStatusNotificationReq& data,
                                                 std::string&              error_code,
                                                 std::string&              error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = UploadLogStatusEnumTypeHelper.fromString(json["status"].GetString());
    extract(json, "requestId", data.requestId);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool LogStatusNotificationReqConverter::toJson(const LogStatusNotificationReq& data, rapidjson::Document& json)
{
    fill(json, "status", UploadLogStatusEnumTypeHelper.toString(data.status));
    fill(json, "requestId", data.requestId);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool LogStatusNotificationConfConverter::fromJson(const rapidjson::Value&    json,
                                                  LogStatusNotificationConf& data,
                                                  std::string&               error_code,
                                                  std::string&               error_message)
{
    (void)json;
    (void)data;
    (void)error_code;
    (void)error_message;
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool LogStatusNotificationConfConverter::toJson(const LogStatusNotificationConf& data, rapidjson::Document& json)
{
    (void)json;
    (void)data;
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
