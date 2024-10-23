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

#include "ClearChargingProfile.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief Helper to convert a enum class ChargingProfilePurposeType enum to string */
const EnumToStringFromString<ChargingProfilePurposeType> ChargingProfilePurposeTypeHelper = {
    {ChargingProfilePurposeType::ChargePointMaxProfile, "ChargePointMaxProfile"},
    {ChargingProfilePurposeType::TxDefaultProfile, "TxDefaultProfile"},
    {ChargingProfilePurposeType::TxProfile, "TxProfile"}};

/** @brief Helper to convert a enum class ClearChargingProfileStatus enum to string */
const EnumToStringFromString<ClearChargingProfileStatus> ClearChargingProfileStatusHelper = {
    {ClearChargingProfileStatus::Accepted, "Accepted"}, {ClearChargingProfileStatus::Unknown, "Unknown"}};

} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool ClearChargingProfileReqConverter::fromJson(const rapidjson::Value&  json,
                                                ClearChargingProfileReq& data,
                                                std::string&             error_code,
                                                std::string&             error_message)
{
    extract(json, "id", data.id);
    bool ret = extract(json, "connectorId", data.connectorId, error_message);
    if (json.HasMember("chargingProfilePurpose"))
    {
        data.chargingProfilePurpose = ChargingProfilePurposeTypeHelper.fromString(json["chargingProfilePurpose"].GetString());
    }
    ret = ret && extract(json, "stackLevel", data.stackLevel, error_message);
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool ClearChargingProfileReqConverter::toJson(const ClearChargingProfileReq& data, rapidjson::Document& json)
{
    fill(json, "id", data.id);
    fill(json, "connectorId", data.connectorId);
    if (data.chargingProfilePurpose.isSet())
    {
        fill(json, "chargingProfilePurpose", ChargingProfilePurposeTypeHelper.toString(data.chargingProfilePurpose));
    }
    fill(json, "stackLevel", data.stackLevel);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool ClearChargingProfileConfConverter::fromJson(const rapidjson::Value&   json,
                                                 ClearChargingProfileConf& data,
                                                 std::string&              error_code,
                                                 std::string&              error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = ClearChargingProfileStatusHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool ClearChargingProfileConfConverter::toJson(const ClearChargingProfileConf& data, rapidjson::Document& json)
{
    fill(json, "status", ClearChargingProfileStatusHelper.toString(data.status));
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
