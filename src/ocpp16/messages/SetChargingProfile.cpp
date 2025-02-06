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

#include "SetChargingProfile.h"
#include "ChargingProfileConverter.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief Helper to convert a enum class ChargingProfileStatus enum to string */
const EnumToStringFromString<ChargingProfileStatus> ChargingProfileStatusHelper = {{ChargingProfileStatus::Accepted, "Accepted"},
                                                                                   {ChargingProfileStatus::Rejected, "Rejected"},
                                                                                   {ChargingProfileStatus::NotSupported, "NotSupported"}};

/** @brief Helper to convert a enum class ChargingProfileKindType enum to string */
const EnumToStringFromString<ChargingProfileKindType> ChargingProfileKindTypeHelper = {{ChargingProfileKindType::Absolute, "Absolute"},
                                                                                       {ChargingProfileKindType::Recurring, "Recurring"},
                                                                                       {ChargingProfileKindType::Relative, "Relative"}};

/** @brief Helper to convert a enum class RecurrencyKindType enum to string */
const EnumToStringFromString<RecurrencyKindType> RecurrencyKindTypeHelper = {{RecurrencyKindType::Daily, "Daily"},
                                                                             {RecurrencyKindType::Weekly, "Weekly"}};

/** @brief Helper to convert a enum class ChargingRateUnitType enum to string */
const EnumToStringFromString<ChargingRateUnitType> ChargingRateUnitTypeHelper = {{ChargingRateUnitType::W, "W"},
                                                                                 {ChargingRateUnitType::A, "A"}};

} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool SetChargingProfileReqConverter::fromJson(const rapidjson::Value& json,
                                              SetChargingProfileReq&  data,
                                              std::string&            error_code,
                                              std::string&            error_message)
{
    bool ret = extract(json, "connectorId", data.connectorId, error_message);

    const rapidjson::Value&  csChargingProfiles = json["csChargingProfiles"];
    ChargingProfileConverter charging_profile_converter;
    ret = ret && charging_profile_converter.fromJson(csChargingProfiles, data.csChargingProfiles, error_code, error_message);
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }

    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool SetChargingProfileReqConverter::toJson(const SetChargingProfileReq& data, rapidjson::Document& json)
{
    (void)data;
    (void)json;

    fill(json, "connectorId", data.connectorId);

    ChargingProfileConverter charging_profile_converter;
    charging_profile_converter.setAllocator(allocator);

    rapidjson::Document csChargingProfiles(rapidjson::kObjectType);
    bool                ret = charging_profile_converter.toJson(data.csChargingProfiles, csChargingProfiles);
    json.AddMember(rapidjson::StringRef("csChargingProfiles"), csChargingProfiles.Move(), *allocator);

    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool SetChargingProfileConfConverter::fromJson(const rapidjson::Value& json,
                                               SetChargingProfileConf& data,
                                               std::string&            error_code,
                                               std::string&            error_message)
{
    (void)error_code;
    (void)error_message;

    data.status = ChargingProfileStatusHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool SetChargingProfileConfConverter::toJson(const SetChargingProfileConf& data, rapidjson::Document& json)
{
    fill(json, "status", ChargingProfileStatusHelper.toString(data.status));
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
