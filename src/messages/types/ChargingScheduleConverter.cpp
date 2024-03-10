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

#include "ChargingScheduleConverter.h"
#include "Enums.h"
#include "IRpc.h"

using namespace ocpp::types;

namespace ocpp
{
namespace messages
{

/** @copydoc bool IMessageConverter<ocpp::types::ChargingSchedule>::fromJson(const rapidjson::Value&,
 *                                                                    ocpp::types::ChargingSchedule&,
 *                                                                    std::string&,
 *                                                                    std::string&) */
bool ChargingScheduleConverter::fromJson(const rapidjson::Value&        json,
                                         ocpp::types::ChargingSchedule& data,
                                         std::string&                   error_code,
                                         std::string&                   error_message)
{
    bool ret = extract(json, "startSchedule", data.startSchedule, error_message);
    extract(json, "duration", data.duration);
    data.chargingRateUnit = ChargingRateUnitTypeHelper.fromString(json["chargingRateUnit"].GetString());
    extract(json, "minChargingRate", data.minChargingRate);

    std::vector<ChargingSchedulePeriod>& chargingSchedulePeriods = data.chargingSchedulePeriod;
    const rapidjson::Value&              chargingSchedulePeriod  = json["chargingSchedulePeriod"];
    for (auto it_period = chargingSchedulePeriod.Begin(); ret && (it_period != chargingSchedulePeriod.End()); ++it_period)
    {
        chargingSchedulePeriods.emplace_back();
        ChargingSchedulePeriod& period = chargingSchedulePeriods.back();
        extract(*it_period, "startPeriod", period.startPeriod);
        extract(*it_period, "limit", period.limit);
        ret = ret && extract(*it_period, "numberPhases", period.numberPhases, error_message);
        if (ret && period.numberPhases.isSet())
        {
            if ((period.numberPhases == 0u) || (period.numberPhases > 3u))
            {
                error_message = "numberPhases parameter must be in interval [1;3]";
                ret           = false;
            }
        }
    }

    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}


double getDecimal(float data)
{
    std::stringstream m_stringstream;
    std::string m_string;

    m_stringstream.str("");
    m_stringstream.clear();
    m_stringstream << std::fixed << std::setprecision(1) << data;
    m_string = m_stringstream.str();

    double res = std::stod(m_string);

    return res;
}

/** @copydoc bool IMessageConverter<ocpp::types::ChargingSchedule>::toJson(const ocpp::types::ChargingSchedule&,
 *                                                                  rapidjson::Document&) */
bool ChargingScheduleConverter::toJson(const ocpp::types::ChargingSchedule& data, rapidjson::Document& json)
{
    std::stringstream m_stringstream;
    std::string m_string;


    fill(json, "startSchedule", data.startSchedule);
    fill(json, "duration", data.duration);
    fill(json, "chargingRateUnit", ChargingRateUnitTypeHelper.toString(data.chargingRateUnit));
    fill(json, "minChargingRate", getDecimal(data.minChargingRate));


    rapidjson::Value chargingSchedulePeriod(rapidjson::kArrayType);
    for (const ChargingSchedulePeriod& schedule_period : data.chargingSchedulePeriod)
    {
        rapidjson::Document value;
        value.Parse("{}");
        fill(value, "startPeriod", schedule_period.startPeriod);
        fill(value, "limit", getDecimal(schedule_period.limit));
        fill(value, "numberPhases", schedule_period.numberPhases);
        chargingSchedulePeriod.PushBack(value.Move(), *allocator);
    }
    json.AddMember(rapidjson::StringRef("chargingSchedulePeriod"), chargingSchedulePeriod.Move(), *allocator);

    return true;
}

} // namespace messages
} // namespace ocpp
