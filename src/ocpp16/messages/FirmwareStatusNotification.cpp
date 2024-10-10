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

#include "FirmwareStatusNotification.h"
#include "ChargingProfileConverter.h"
#include "IRpc.h"

using namespace ocpp::types;

namespace ocpp
{
namespace types
{
/** @brief Helper to convert a enum class FirmwareStatus enum to string */
const EnumToStringFromString<FirmwareStatus> FirmwareStatusHelper = {{FirmwareStatus::Downloaded, "Downloaded"},
                                                                     {FirmwareStatus::DownloadFailed, "DownloadFailed"},
                                                                     {FirmwareStatus::Downloading, "Downloading"},
                                                                     {FirmwareStatus::Idle, "Idle"},
                                                                     {FirmwareStatus::InstallationFailed, "InstallationFailed"},
                                                                     {FirmwareStatus::Installing, "Installing"},
                                                                     {FirmwareStatus::Installed, "Installed"}};

} // namespace types

namespace messages
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool FirmwareStatusNotificationReqConverter::fromJson(const rapidjson::Value&        json,
                                                      FirmwareStatusNotificationReq& data,
                                                      std::string&                   error_code,
                                                      std::string&                   error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = FirmwareStatusHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool FirmwareStatusNotificationReqConverter::toJson(const FirmwareStatusNotificationReq& data, rapidjson::Document& json)
{
    fill(json, "status", FirmwareStatusHelper.toString(data.status));
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool FirmwareStatusNotificationConfConverter::fromJson(const rapidjson::Value&         json,
                                                       FirmwareStatusNotificationConf& data,
                                                       std::string&                    error_code,
                                                       std::string&                    error_message)
{
    (void)json;
    (void)data;
    (void)error_code;
    (void)error_message;
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool FirmwareStatusNotificationConfConverter::toJson(const FirmwareStatusNotificationConf& data, rapidjson::Document& json)
{
    (void)json;
    (void)data;
    return true;
}

} // namespace messages
} // namespace ocpp
