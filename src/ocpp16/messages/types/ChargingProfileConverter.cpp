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

#include "ChargingProfileConverter.h"
#include "ChargingScheduleConverter.h"
#include "Enums.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<ocpp::types::ocpp16::ChargingProfile>::fromJson(const rapidjson::Value&,
 *                                                                    ocpp::types::ocpp16::ChargingProfile&,
 *                                                                    std::string&,
 *                                                                    std::string&) */
bool ChargingProfileConverter::fromJson(const rapidjson::Value&               json,
                                        ocpp::types::ocpp16::ChargingProfile& data,
                                        std::string&                          error_code,
                                        std::string&                          error_message)
{
    bool ret;
    extract(json, "chargingProfileId", data.chargingProfileId);
    extract(json, "transactionId", data.transactionId);
    ret                         = extract(json, "stackLevel", data.stackLevel, error_message);
    data.chargingProfilePurpose = ChargingProfilePurposeTypeHelper.fromString(json["chargingProfilePurpose"].GetString());
    data.chargingProfileKind    = ChargingProfileKindTypeHelper.fromString(json["chargingProfileKind"].GetString());
    if (json.HasMember("recurrencyKind"))
    {
        data.recurrencyKind = RecurrencyKindTypeHelper.fromString(json["recurrencyKind"].GetString());
    }
    ret = ret && extract(json, "validFrom", data.validFrom, error_message);
    ret = ret && extract(json, "validTo", data.validTo, error_message);

    ChargingScheduleConverter charging_schedule_converter;
    ret = ret && charging_schedule_converter.fromJson(json["chargingSchedule"], data.chargingSchedule, error_code, error_message);

    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<ocpp::types::ocpp16::ChargingProfile>::toJson(const ocpp::types::ocpp16::ChargingProfile&,
 *                                                                  rapidjson::Document&) */
bool ChargingProfileConverter::toJson(const ocpp::types::ocpp16::ChargingProfile& data, rapidjson::Document& json)
{
    fill(json, "chargingProfileId", data.chargingProfileId);
    fill(json, "transactionId", data.transactionId);
    fill(json, "stackLevel", data.stackLevel);
    fill(json, "chargingProfilePurpose", ChargingProfilePurposeTypeHelper.toString(data.chargingProfilePurpose));
    fill(json, "chargingProfileKind", ChargingProfileKindTypeHelper.toString(data.chargingProfileKind));
    if (data.recurrencyKind.isSet())
    {
        fill(json, "recurrencyKind", RecurrencyKindTypeHelper.toString(data.recurrencyKind));
    }
    fill(json, "validFrom", data.validFrom);
    fill(json, "validTo", data.validTo);

    ChargingScheduleConverter charging_schedule_converter;
    charging_schedule_converter.setAllocator(allocator);
    rapidjson::Document charging_schedule(rapidjson::kObjectType);
    bool                ret = charging_schedule_converter.toJson(data.chargingSchedule, charging_schedule);
    json.AddMember(rapidjson::StringRef("chargingSchedule"), charging_schedule.Move(), *allocator);

    return ret;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
