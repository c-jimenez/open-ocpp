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

/*
    Generated by json2cpp from : urn:OCPP:Cp:2:2020:3:NotifyEVChargingNeedsRequest
                                 OCPP 2.0.1 FINAL
*/

#include "DCChargingParametersType.h"

#include "IRpc.h"

namespace ocpp
{
namespace types
{
namespace ocpp20
{

/** @brief Convert a DCChargingParametersType from a JSON representation */
bool DCChargingParametersTypeConverter::fromJson(const rapidjson::Value&       json,
                                      DCChargingParametersType&                data,
                                      std::string&                  error_code,
                                      [[maybe_unused]] std::string& error_message)
{
    bool ret = true;

    // customData
    if (json.HasMember("customData"))
    {
    CustomDataTypeConverter customData_converter;
    ret = ret && customData_converter.fromJson(json["customData"], data.customData, error_code, error_message);
    }

    // evMaxCurrent
    extract(json, "evMaxCurrent", data.evMaxCurrent);

    // evMaxVoltage
    extract(json, "evMaxVoltage", data.evMaxVoltage);

    // energyAmount
    extract(json, "energyAmount", data.energyAmount);

    // evMaxPower
    extract(json, "evMaxPower", data.evMaxPower);

    // stateOfCharge
    extract(json, "stateOfCharge", data.stateOfCharge);

    // evEnergyCapacity
    extract(json, "evEnergyCapacity", data.evEnergyCapacity);

    // fullSoC
    extract(json, "fullSoC", data.fullSoC);

    // bulkSoC
    extract(json, "bulkSoC", data.bulkSoC);

    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }

    return ret;
}

/** @brief Convert a DCChargingParametersType to a JSON representation */
bool DCChargingParametersTypeConverter::toJson(const DCChargingParametersType& data, rapidjson::Document& json) 
{
    bool ret = true;

    // customData
    if (data.customData.isSet())
    {
    CustomDataTypeConverter customData_converter;
    customData_converter.setAllocator(allocator);
    rapidjson::Document customData_doc;
    customData_doc.Parse("{}");
    ret = ret && customData_converter.toJson(data.customData, customData_doc);
    json.AddMember(rapidjson::StringRef("customData"), customData_doc.Move(), *allocator);
    }

    // evMaxCurrent
    fill(json, "evMaxCurrent", data.evMaxCurrent);

    // evMaxVoltage
    fill(json, "evMaxVoltage", data.evMaxVoltage);

    // energyAmount
    fill(json, "energyAmount", data.energyAmount);

    // evMaxPower
    fill(json, "evMaxPower", data.evMaxPower);

    // stateOfCharge
    fill(json, "stateOfCharge", data.stateOfCharge);

    // evEnergyCapacity
    fill(json, "evEnergyCapacity", data.evEnergyCapacity);

    // fullSoC
    fill(json, "fullSoC", data.fullSoC);

    // bulkSoC
    fill(json, "bulkSoC", data.bulkSoC);

    return ret;
}

} // namespace ocpp20
} // namespace types
} // namespace ocpp