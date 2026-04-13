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

#include "CertificateHashDataChainTypeConverter.h"
#include "CertificateHashDataTypeConverter.h"
#include "Enums.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief Helper to convert a enum class GetCertificateIdUseEnumType enum to string */
const EnumToStringFromString<GetCertificateIdUseEnumType> GetCertificateIdUseEnumTypeHelper = {
    {GetCertificateIdUseEnumType::MORootCertificate, "MORootCertificate"},
    {GetCertificateIdUseEnumType::V2GCertificateChain, "V2GCertificateChain"},
    {GetCertificateIdUseEnumType::V2GRootCertificate, "V2GRootCertificate"},
    {GetCertificateIdUseEnumType::OEMRootCertificate, "OEMRootCertificate"}};
} // namespace ocpp16
} // namespace types

namespace messages
{
namespace ocpp16
{

/** @bcopydoc bool IMessageConverter<ocpp::types::ocpp16::CertificateHashDataChainType>::fromJson(const rapidjson::Value&,
 *                                                                                        ocpp::types::ocpp16::CertificateHashDataChainType&,
 *                                                                                        std::string&,
 *                                                                                        std::string&) */
bool CertificateHashDataChainTypeConverter::fromJson(const rapidjson::Value&                            json,
                                                     ocpp::types::ocpp16::CertificateHashDataChainType& data,
                                                     std::string&                                       error_code,
                                                     std::string&                                       error_message)
{
    CertificateHashDataTypeConverter certificate_hash_data_type_converter;

    data.certificateType = GetCertificateIdUseEnumTypeHelper.fromString(json["certificateType"].GetString());
    bool ret =
        certificate_hash_data_type_converter.fromJson(json["certificateHashData"], data.certificateHashData, error_code, error_message);
    if (ret)
    {
        std::vector<CertificateHashDataType>& child_certificates       = data.childCertificateHashData;
        const rapidjson::Value&               childCertificateHashData = json["childCertificateHashData"];
        for (auto it_cert = childCertificateHashData.Begin(); ret && (it_cert != childCertificateHashData.End()); ++it_cert)
        {
            child_certificates.emplace_back();
            CertificateHashDataType& child_certificate = child_certificates.back();
            ret = ret && certificate_hash_data_type_converter.fromJson(*it_cert, child_certificate, error_code, error_message);
        }
    }
    return ret;
}

/** @copydoc bool IMessageConverter<ocpp::types::ocpp16::CertificateHashDataChainType>::toJson(const ocpp::types::ocpp16::CertificateHashDataChainType&,
 *                                                                                     rapidjson::Document&) */
bool CertificateHashDataChainTypeConverter::toJson(const ocpp::types::ocpp16::CertificateHashDataChainType& data, rapidjson::Document& json)
{
    fill(json, "certificateType", GetCertificateIdUseEnumTypeHelper.toString(data.certificateType));

    CertificateHashDataTypeConverter certificate_hash_data_type_converter;
    certificate_hash_data_type_converter.setAllocator(allocator);
    rapidjson::Document hash_data(rapidjson::kObjectType);
    bool                ret = certificate_hash_data_type_converter.toJson(data.certificateHashData, hash_data);
    if (ret)
    {
        json.AddMember(rapidjson::StringRef("certificateHashData"), hash_data.Move(), *allocator);

        rapidjson::Value childCertificateHashData(rapidjson::kArrayType);
        for (const auto& child_certificate : data.childCertificateHashData)
        {
            rapidjson::Document value(rapidjson::kObjectType);
            ret = ret && certificate_hash_data_type_converter.toJson(child_certificate, value);
            if (ret)
            {
                childCertificateHashData.PushBack(value.Move(), *allocator);
            }
        }
        json.AddMember(rapidjson::StringRef("childCertificateHashData"), childCertificateHashData.Move(), *allocator);
    }
    return ret;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
