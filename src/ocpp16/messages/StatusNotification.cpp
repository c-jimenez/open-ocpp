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

#include "StatusNotification.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{
/** @brief Helper to convert a ChargePointErrorCode enum to string */
const EnumToStringFromString<ChargePointErrorCode> ChargePointErrorCodeHelper = {
    {ChargePointErrorCode::ConnectorLockFailure, "ConnectorLockFailure"},
    {ChargePointErrorCode::EVCommunicationError, "EVCommunicationError"},
    {ChargePointErrorCode::GroundFailure, "GroundFailure"},
    {ChargePointErrorCode::HighTemperature, "HighTemperature"},
    {ChargePointErrorCode::InternalError, "InternalError"},
    {ChargePointErrorCode::LocalListConflict, "LocalListConflict"},
    {ChargePointErrorCode::NoError, "NoError"},
    {ChargePointErrorCode::OtherError, "OtherError"},
    {ChargePointErrorCode::OverCurrentFailure, "OverCurrentFailure"},
    {ChargePointErrorCode::OverVoltage, "OverVoltage"},
    {ChargePointErrorCode::PowerMeterFailure, "PowerMeterFailure"},
    {ChargePointErrorCode::PowerSwitchFailure, "PowerSwitchFailure"},
    {ChargePointErrorCode::ReaderFailure, "ReaderFailure"},
    {ChargePointErrorCode::ResetFailure, "ResetFailure"},
    {ChargePointErrorCode::UnderVoltage, "UnderVoltage"},
    {ChargePointErrorCode::WeakSignal, "WeakSignal"}};

/** @brief Helper to convert a ChargePointStatus enum to string */
const EnumToStringFromString<ChargePointStatus> ChargePointStatusHelper = {{ChargePointStatus::Available, "Available"},
                                                                           {ChargePointStatus::Charging, "Charging"},
                                                                           {ChargePointStatus::Faulted, "Faulted"},
                                                                           {ChargePointStatus::Finishing, "Finishing"},
                                                                           {ChargePointStatus::Preparing, "Preparing"},
                                                                           {ChargePointStatus::Reserved, "Reserved"},
                                                                           {ChargePointStatus::SuspendedEV, "SuspendedEV"},
                                                                           {ChargePointStatus::SuspendedEVSE, "SuspendedEVSE"},
                                                                           {ChargePointStatus::Unavailable, "Unavailable"}};

} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool StatusNotificationReqConverter::fromJson(const rapidjson::Value& json,
                                              StatusNotificationReq&  data,
                                              std::string&            error_code,
                                              std::string&            error_message)
{
    bool ret;
    ret            = extract(json, "connectorId", data.connectorId, error_message);
    data.errorCode = ChargePointErrorCodeHelper.fromString(json["errorCode"].GetString());
    extract(json, "info", data.info);
    data.status = ChargePointStatusHelper.fromString(json["status"].GetString());
    ret         = ret && extract(json, "timestamp", data.timestamp, error_message);
    extract(json, "vendorId", data.vendorId);
    extract(json, "vendorErrorCode", data.vendorErrorCode);
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool StatusNotificationReqConverter::toJson(const StatusNotificationReq& data, rapidjson::Document& json)
{
    fill(json, "connectorId", data.connectorId);
    fill(json, "errorCode", ChargePointErrorCodeHelper.toString(data.errorCode));
    fill(json, "status", ChargePointStatusHelper.toString(data.status));
    fill(json, "timestamp", data.timestamp);
    if (data.info.isSet())
        fill(json, "info", data.info);
    if (data.vendorId.isSet())
        fill(json, "vendorId", data.vendorId);
    if (data.vendorErrorCode.isSet())
        fill(json, "vendorErrorCode", data.vendorErrorCode);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool StatusNotificationConfConverter::fromJson(const rapidjson::Value& json,
                                               StatusNotificationConf& data,
                                               std::string&            error_code,
                                               std::string&            error_message)
{
    (void)json;
    (void)data;
    (void)error_code;
    (void)error_message;
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool StatusNotificationConfConverter::toJson(const StatusNotificationConf& data, rapidjson::Document& json)
{
    (void)data;
    (void)json;
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
