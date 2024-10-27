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

#include "MeterValueConverter.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<ocpp::types::ocpp16::MeterValue>::fromJson(const rapidjson::Value&,
 *                                                                    ocpp::types::ocpp16::MeterValue&,
 *                                                                    std::string&,
 *                                                                    std::string&) */
bool MeterValueConverter::fromJson(const rapidjson::Value&          json,
                                   ocpp::types::ocpp16::MeterValue& data,
                                   std::string&                     error_code,
                                   std::string&                     error_message)
{
    bool ret = extract(json, "timestamp", data.timestamp, error_message);

    const rapidjson::Value& sampledValue = json["sampledValue"];
    for (auto it_sampled = sampledValue.Begin(); ret && (it_sampled != sampledValue.End()); ++it_sampled)
    {
        data.sampledValue.emplace_back();
        SampledValue& sampled_value = data.sampledValue.back();

        extract(*it_sampled, "value", sampled_value.value);
        if ((*it_sampled).HasMember("context"))
        {
            sampled_value.context = ReadingContextHelper.fromString((*it_sampled)["context"].GetString());
        }
        if ((*it_sampled).HasMember("format"))
        {
            sampled_value.format = ValueFormatHelper.fromString((*it_sampled)["format"].GetString());
        }
        if ((*it_sampled).HasMember("measurand"))
        {
            sampled_value.measurand = MeasurandHelper.fromString((*it_sampled)["measurand"].GetString());
        }
        if ((*it_sampled).HasMember("phase"))
        {
            sampled_value.phase = PhaseHelper.fromString((*it_sampled)["phase"].GetString());
        }
        if ((*it_sampled).HasMember("location"))
        {
            sampled_value.location = LocationHelper.fromString((*it_sampled)["location"].GetString());
        }
        if ((*it_sampled).HasMember("unit"))
        {
            sampled_value.unit = UnitOfMeasureHelper.fromString((*it_sampled)["unit"].GetString());
        }
    }
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }

    return ret;
}

/** @copydoc bool IMessageConverter<ocpp::types::ocpp16::MeterValue>::toJson(const ocpp::types::ocpp16::MeterValue&,
 *                                                                  rapidjson::Document&) */
bool MeterValueConverter::toJson(const ocpp::types::ocpp16::MeterValue& data, rapidjson::Document& json)
{
    fill(json, "timestamp", data.timestamp);

    rapidjson::Value sampledValue(rapidjson::kArrayType);
    for (const SampledValue& sampled_value : data.sampledValue)
    {
        rapidjson::Document sampled(rapidjson::kObjectType);
        fill(sampled, "value", sampled_value.value);
        if (sampled_value.context.isSet())
        {
            fill(sampled, "context", ReadingContextHelper.toString(sampled_value.context));
        }
        if (sampled_value.format.isSet())
        {
            fill(sampled, "format", ValueFormatHelper.toString(sampled_value.format));
        }
        if (sampled_value.measurand.isSet())
        {
            fill(sampled, "measurand", MeasurandHelper.toString(sampled_value.measurand));
        }
        if (sampled_value.phase.isSet())
        {
            fill(sampled, "phase", PhaseHelper.toString(sampled_value.phase));
        }
        if (sampled_value.location.isSet())
        {
            fill(sampled, "location", LocationHelper.toString(sampled_value.location));
        }
        if (sampled_value.unit.isSet())
        {
            fill(sampled, "unit", UnitOfMeasureHelper.toString(sampled_value.unit));
        }
        sampledValue.PushBack(sampled.Move(), *allocator);
    }
    json.AddMember(rapidjson::StringRef("sampledValue"), sampledValue.Move(), *allocator);

    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
