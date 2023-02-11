/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License version 2.1
as published by the Free Software Foundation.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#include "OcspRequestDataTypeConverter.h"
#include "Enums.h"
#include "IRpc.h"

using namespace ocpp::types;

namespace ocpp
{
namespace messages
{

/** @bcopydoc bool IMessageConverter<ocpp::types::OcspRequestDataType>::fromJson(const rapidjson::Value&,
 *                                                                               ocpp::types::OcspRequestDataType&,
 *                                                                               std::string&,
 *                                                                               std::string&) */
bool OcspRequestDataTypeConverter::fromJson(const rapidjson::Value&           json,
                                            ocpp::types::OcspRequestDataType& data,
                                            std::string&                      error_code,
                                            std::string&                      error_message)
{
    (void)error_code;
    (void)error_message;
    data.hashAlgorithm = HashAlgorithmEnumTypeHelper.fromString(json["hashAlgorithm"].GetString());
    extract(json, "issuerKeyHash", data.issuerKeyHash);
    extract(json, "issuerNameHash", data.issuerNameHash);
    extract(json, "serialNumber", data.serialNumber);
    extract(json, "responderURL", data.responderURL);
    return true;
}

/** @copydoc bool IMessageConverter<ocpp::types::OcspRequestDataType>::toJson(const ocpp::types::OcspRequestDataType&,
 *                                                                            rapidjson::Document&) */
bool OcspRequestDataTypeConverter::toJson(const ocpp::types::OcspRequestDataType& data, rapidjson::Document& json)
{
    fill(json, "hashAlgorithm", HashAlgorithmEnumTypeHelper.toString(data.hashAlgorithm));
    fill(json, "issuerKeyHash", data.issuerKeyHash);
    fill(json, "issuerNameHash", data.issuerNameHash);
    fill(json, "serialNumber", data.serialNumber);
    fill(json, "responderURL", data.responderURL);
    return true;
}

} // namespace messages
} // namespace ocpp
