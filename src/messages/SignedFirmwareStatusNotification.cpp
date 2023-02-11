/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License version 2.1
as published by the Free Software Foundation.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#include "SignedFirmwareStatusNotification.h"
#include "IRpc.h"

using namespace ocpp::types;

namespace ocpp
{
namespace types
{
/** @brief Helper to convert a enum class FirmwareStatusEnumType enum to string */
const EnumToStringFromString<FirmwareStatusEnumType> FirmwareStatusEnumTypeHelper = {
    {FirmwareStatusEnumType::Downloaded, "Downloaded"},
    {FirmwareStatusEnumType::DownloadFailed, "DownloadFailed"},
    {FirmwareStatusEnumType::Downloading, "Downloading"},
    {FirmwareStatusEnumType::DownloadScheduled, "DownloadScheduled"},
    {FirmwareStatusEnumType::DownloadPaused, "DownloadPaused"},
    {FirmwareStatusEnumType::Idle, "Idle"},
    {FirmwareStatusEnumType::InstallationFailed, "InstallationFailed"},
    {FirmwareStatusEnumType::Installing, "Installing"},
    {FirmwareStatusEnumType::Installed, "Installed"},
    {FirmwareStatusEnumType::InstallRebooting, "InstallRebooting"},
    {FirmwareStatusEnumType::InstallScheduled, "InstallScheduled"},
    {FirmwareStatusEnumType::InstallVerificationFailed, "InstallVerificationFailed"},
    {FirmwareStatusEnumType::InvalidSignature, "InvalidSignature"},
    {FirmwareStatusEnumType::SignatureVerified, "SignatureVerified"}};

} // namespace types

namespace messages
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool SignedFirmwareStatusNotificationReqConverter::fromJson(const rapidjson::Value&              json,
                                                            SignedFirmwareStatusNotificationReq& data,
                                                            std::string&                         error_code,
                                                            std::string&                         error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = FirmwareStatusEnumTypeHelper.fromString(json["status"].GetString());
    extract(json, "requestId", data.requestId);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool SignedFirmwareStatusNotificationReqConverter::toJson(const SignedFirmwareStatusNotificationReq& data, rapidjson::Document& json)
{
    fill(json, "status", FirmwareStatusEnumTypeHelper.toString(data.status));
    fill(json, "requestId", data.requestId);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool SignedFirmwareStatusNotificationConfConverter::fromJson(const rapidjson::Value&               json,
                                                             SignedFirmwareStatusNotificationConf& data,
                                                             std::string&                          error_code,
                                                             std::string&                          error_message)
{
    (void)json;
    (void)data;
    (void)error_code;
    (void)error_message;
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool SignedFirmwareStatusNotificationConfConverter::toJson(const SignedFirmwareStatusNotificationConf& data, rapidjson::Document& json)
{
    (void)json;
    (void)data;
    return true;
}

} // namespace messages
} // namespace ocpp
