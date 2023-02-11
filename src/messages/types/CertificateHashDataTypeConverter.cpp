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

#include "CertificateHashDataTypeConverter.h"
#include "Enums.h"
#include "IRpc.h"

using namespace ocpp::types;

namespace ocpp
{
namespace types
{
/** @brief Helper to convert a enum class HashAlgorithmEnumType enum to string */
const EnumToStringFromString<HashAlgorithmEnumType> HashAlgorithmEnumTypeHelper = {
    {HashAlgorithmEnumType::SHA256, "SHA256"}, {HashAlgorithmEnumType::SHA384, "SHA384"}, {HashAlgorithmEnumType::SHA512, "SHA512"}};

} // namespace types
namespace messages
{

/** @bcopydoc bool IMessageConverter<ocpp::types::CertificateHashDataType>::fromJson(const rapidjson::Value&,
 *                                                                     ocpp::types::CertificateHashDataType&,
 *                                                                     std::string&,
 *                                                                     std::string&) */
bool CertificateHashDataTypeConverter::fromJson(const rapidjson::Value&               json,
                                                ocpp::types::CertificateHashDataType& data,
                                                std::string&                          error_code,
                                                std::string&                          error_message)
{
    (void)error_code;
    (void)error_message;
    data.hashAlgorithm = HashAlgorithmEnumTypeHelper.fromString(json["hashAlgorithm"].GetString());
    extract(json, "issuerKeyHash", data.issuerKeyHash);
    extract(json, "issuerNameHash", data.issuerNameHash);
    extract(json, "serialNumber", data.serialNumber);
    return true;
}

/** @copydoc bool IMessageConverter<ocpp::types::CertificateHashDataType>::toJson(const ocpp::types::CertificateHashDataType&,
 *                                                                  rapidjson::Document&) */
bool CertificateHashDataTypeConverter::toJson(const ocpp::types::CertificateHashDataType& data, rapidjson::Document& json)
{
    fill(json, "hashAlgorithm", HashAlgorithmEnumTypeHelper.toString(data.hashAlgorithm));
    fill(json, "issuerKeyHash", data.issuerKeyHash);
    fill(json, "issuerNameHash", data.issuerNameHash);
    fill(json, "serialNumber", data.serialNumber);
    return true;
}

} // namespace messages
} // namespace ocpp
