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

#include "BootNotification.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{
/** @brief Helper to convert a RegistrationStatus enum to string */
const EnumToStringFromString<RegistrationStatus> RegistrationStatusHelper = {
    {RegistrationStatus::Accepted, "Accepted"}, {RegistrationStatus::Pending, "Pending"}, {RegistrationStatus::Rejected, "Rejected"}};
} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool BootNotificationReqConverter::fromJson(const rapidjson::Value& json,
                                            BootNotificationReq&    data,
                                            std::string&            error_code,
                                            std::string&            error_message)
{
    (void)error_code;
    (void)error_message;
    extract(json, "chargeBoxSerialNumber", data.chargeBoxSerialNumber);
    extract(json, "chargePointModel", data.chargePointModel);
    extract(json, "chargePointSerialNumber", data.chargePointSerialNumber);
    extract(json, "chargePointVendor", data.chargePointVendor);
    extract(json, "firmwareVersion", data.firmwareVersion);
    extract(json, "iccid", data.iccid);
    extract(json, "imsi", data.imsi);
    extract(json, "meterSerialNumber", data.meterSerialNumber);
    extract(json, "meterType", data.meterType);

    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool BootNotificationReqConverter::toJson(const BootNotificationReq& data, rapidjson::Document& json)
{
    fill(json, "chargeBoxSerialNumber", data.chargeBoxSerialNumber);
    fill(json, "chargePointModel", data.chargePointModel);
    fill(json, "chargePointSerialNumber", data.chargePointSerialNumber);
    fill(json, "chargePointVendor", data.chargePointVendor);
    fill(json, "firmwareVersion", data.firmwareVersion);
    fill(json, "iccid", data.iccid);
    fill(json, "imsi", data.imsi);
    fill(json, "meterSerialNumber", data.meterSerialNumber);
    fill(json, "meterType", data.meterType);

    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool BootNotificationConfConverter::fromJson(const rapidjson::Value& json,
                                             BootNotificationConf&   data,
                                             std::string&            error_code,
                                             std::string&            error_message)
{
    bool ret;

    ret         = extract(json, "currentTime", data.currentTime, error_message);
    ret         = ret && extract(json, "interval", data.interval, error_message);
    data.status = RegistrationStatusHelper.fromString(json["status"].GetString());
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool BootNotificationConfConverter::toJson(const BootNotificationConf& data, rapidjson::Document& json)
{
    fill(json, "currentTime", data.currentTime.str());
    fill(json, "interval", data.interval);
    fill(json, "status", RegistrationStatusHelper.toString(data.status));
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
