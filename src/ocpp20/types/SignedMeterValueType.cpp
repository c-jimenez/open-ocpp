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
    Generated by json2cpp from : urn:OCPP:Cp:2:2020:3:MeterValuesRequest
                                 OCPP 2.0.1 FINAL
*/

#include "SignedMeterValueType.h"

#include "IRpc.h"

namespace ocpp
{
namespace types
{
namespace ocpp20
{

/** @brief Convert a SignedMeterValueType from a JSON representation */
bool SignedMeterValueTypeConverter::fromJson(const rapidjson::Value&       json,
                                      SignedMeterValueType&                data,
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

    // signedMeterData
    extract(json, "signedMeterData", data.signedMeterData);

    // signingMethod
    extract(json, "signingMethod", data.signingMethod);

    // encodingMethod
    extract(json, "encodingMethod", data.encodingMethod);

    // publicKey
    extract(json, "publicKey", data.publicKey);

    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }

    return ret;
}

/** @brief Convert a SignedMeterValueType to a JSON representation */
bool SignedMeterValueTypeConverter::toJson(const SignedMeterValueType& data, rapidjson::Document& json) 
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

    // signedMeterData
    fill(json, "signedMeterData", data.signedMeterData);

    // signingMethod
    fill(json, "signingMethod", data.signingMethod);

    // encodingMethod
    fill(json, "encodingMethod", data.encodingMethod);

    // publicKey
    fill(json, "publicKey", data.publicKey);

    return ret;
}

} // namespace ocpp20
} // namespace types
} // namespace ocpp