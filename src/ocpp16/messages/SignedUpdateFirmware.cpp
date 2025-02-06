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

#include "SignedUpdateFirmware.h"
#include "IRpc.h"
#include "Url.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief Helper to convert a UpdateFirmwareStatusEnumType enum to string */
const EnumToStringFromString<UpdateFirmwareStatusEnumType> UpdateFirmwareStatusEnumTypeHelper = {
    {UpdateFirmwareStatusEnumType::Accepted, "Accepted"},
    {UpdateFirmwareStatusEnumType::Rejected, "Rejected"},
    {UpdateFirmwareStatusEnumType::AcceptedCanceled, "AcceptedCanceled"},
    {UpdateFirmwareStatusEnumType::InvalidCertificate, "InvalidCertificate"},
    {UpdateFirmwareStatusEnumType::RevokedCertificate, "RevokedCertificate"}};

} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool SignedUpdateFirmwareReqConverter::fromJson(const rapidjson::Value&  json,
                                                SignedUpdateFirmwareReq& data,
                                                std::string&             error_code,
                                                std::string&             error_message)
{
    bool ret;
    extract(json, "requestId", data.requestId);
    ret = extract(json, "retries", data.retries, error_message);
    ret = ret && extract(json, "retryInterval", data.retryInterval, error_message);

    const rapidjson::Value& firmware = json["firmware"];
    extract(firmware, "location", data.firmware.location);
    ocpp::websockets::Url url(data.firmware.location);
    ret = ret && url.isValid();
    ret = ret && extract(firmware, "retrieveDateTime", data.firmware.retrieveDateTime, error_message);
    ret = ret && extract(firmware, "installDateTime", data.firmware.installDateTime, error_message);
    extract(firmware, "signingCertificate", data.firmware.signingCertificate);
    extract(firmware, "signature", data.firmware.signature);
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool SignedUpdateFirmwareReqConverter::toJson(const SignedUpdateFirmwareReq& data, rapidjson::Document& json)
{
    fill(json, "requestId", data.requestId);
    fill(json, "retries", data.retries);
    fill(json, "retryInterval", data.retryInterval);

    rapidjson::Document firmware(rapidjson::kObjectType);
    fill(firmware, "location", data.firmware.location);
    fill(firmware, "retrieveDateTime", data.firmware.retrieveDateTime);
    fill(firmware, "installDateTime", data.firmware.installDateTime);
    fill(firmware, "signingCertificate", data.firmware.signingCertificate);
    fill(firmware, "signature", data.firmware.signature);
    json.AddMember(rapidjson::StringRef("firmware"), firmware.Move(), *allocator);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool SignedUpdateFirmwareConfConverter::fromJson(const rapidjson::Value&   json,
                                                 SignedUpdateFirmwareConf& data,
                                                 std::string&              error_code,
                                                 std::string&              error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = UpdateFirmwareStatusEnumTypeHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool SignedUpdateFirmwareConfConverter::toJson(const SignedUpdateFirmwareConf& data, rapidjson::Document& json)
{
    fill(json, "status", UpdateFirmwareStatusEnumTypeHelper.toString(data.status));
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
