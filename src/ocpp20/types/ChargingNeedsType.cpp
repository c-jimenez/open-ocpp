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

#include "ChargingNeedsType.h"

#include "IRpc.h"

namespace ocpp
{
namespace types
{
namespace ocpp20
{

/** @brief Convert a ChargingNeedsType from a JSON representation */
bool ChargingNeedsTypeConverter::fromJson(const rapidjson::Value&       json,
                                      ChargingNeedsType&                data,
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

    // acChargingParameters
    if (json.HasMember("acChargingParameters"))
    {
    ACChargingParametersTypeConverter acChargingParameters_converter;
    ret = ret && acChargingParameters_converter.fromJson(json["acChargingParameters"], data.acChargingParameters, error_code, error_message);
    }

    // dcChargingParameters
    if (json.HasMember("dcChargingParameters"))
    {
    DCChargingParametersTypeConverter dcChargingParameters_converter;
    ret = ret && dcChargingParameters_converter.fromJson(json["dcChargingParameters"], data.dcChargingParameters, error_code, error_message);
    }

    // requestedEnergyTransfer
    data.requestedEnergyTransfer = EnergyTransferModeEnumTypeHelper.fromString(json["requestedEnergyTransfer"].GetString());

    // departureTime
    ret = ret && extract(json, "departureTime", data.departureTime, error_message);

    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }

    return ret;
}

/** @brief Convert a ChargingNeedsType to a JSON representation */
bool ChargingNeedsTypeConverter::toJson(const ChargingNeedsType& data, rapidjson::Document& json) 
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

    // acChargingParameters
    if (data.acChargingParameters.isSet())
    {
    ACChargingParametersTypeConverter acChargingParameters_converter;
    acChargingParameters_converter.setAllocator(allocator);
    rapidjson::Document acChargingParameters_doc;
    acChargingParameters_doc.Parse("{}");
    ret = ret && acChargingParameters_converter.toJson(data.acChargingParameters, acChargingParameters_doc);
    json.AddMember(rapidjson::StringRef("acChargingParameters"), acChargingParameters_doc.Move(), *allocator);
    }

    // dcChargingParameters
    if (data.dcChargingParameters.isSet())
    {
    DCChargingParametersTypeConverter dcChargingParameters_converter;
    dcChargingParameters_converter.setAllocator(allocator);
    rapidjson::Document dcChargingParameters_doc;
    dcChargingParameters_doc.Parse("{}");
    ret = ret && dcChargingParameters_converter.toJson(data.dcChargingParameters, dcChargingParameters_doc);
    json.AddMember(rapidjson::StringRef("dcChargingParameters"), dcChargingParameters_doc.Move(), *allocator);
    }

    // requestedEnergyTransfer
    fill(json, "requestedEnergyTransfer", EnergyTransferModeEnumTypeHelper.toString(data.requestedEnergyTransfer));

    // departureTime
    fill(json, "departureTime", data.departureTime);

    return ret;
}

} // namespace ocpp20
} // namespace types
} // namespace ocpp