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

#include "DiagnosticsStatusNotification.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{
/** @brief Helper to convert a enum class DiagnosticsStatus enum to string */
const EnumToStringFromString<DiagnosticsStatus> DiagnosticsStatusHelper = {{DiagnosticsStatus::Idle, "Idle"},
                                                                           {DiagnosticsStatus::Uploaded, "Uploaded"},
                                                                           {DiagnosticsStatus::UploadFailed, "UploadFailed"},
                                                                           {DiagnosticsStatus::Uploading, "Uploading"}};

} // namespace ocpp16
} // namespace types

namespace messages
{
namespace ocpp16
{
/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool DiagnosticsStatusNotificationReqConverter::fromJson(const rapidjson::Value&           json,
                                                         DiagnosticsStatusNotificationReq& data,
                                                         std::string&                      error_code,
                                                         std::string&                      error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = DiagnosticsStatusHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool DiagnosticsStatusNotificationReqConverter::toJson(const DiagnosticsStatusNotificationReq& data, rapidjson::Document& json)
{
    fill(json, "status", DiagnosticsStatusHelper.toString(data.status));
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool DiagnosticsStatusNotificationConfConverter::fromJson(const rapidjson::Value&            json,
                                                          DiagnosticsStatusNotificationConf& data,
                                                          std::string&                       error_code,
                                                          std::string&                       error_message)
{
    (void)json;
    (void)data;
    (void)error_code;
    (void)error_message;
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool DiagnosticsStatusNotificationConfConverter::toJson(const DiagnosticsStatusNotificationConf& data, rapidjson::Document& json)
{
    (void)json;
    (void)data;
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
