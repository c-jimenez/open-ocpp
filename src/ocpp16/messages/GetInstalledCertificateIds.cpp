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

#include "GetInstalledCertificateIds.h"
#include "CertificateHashDataTypeConverter.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{
/** @brief Helper to convert a enum class CertificateUseEnumType enum to string */
const EnumToStringFromString<CertificateUseEnumType> CertificateUseEnumTypeHelper = {
    {CertificateUseEnumType::CentralSystemRootCertificate, "CentralSystemRootCertificate"},
    {CertificateUseEnumType::ManufacturerRootCertificate, "ManufacturerRootCertificate"}};

/** @brief Helper to convert a enum class GetInstalledCertificateStatusEnumType enum to string */
const EnumToStringFromString<GetInstalledCertificateStatusEnumType> GetInstalledCertificateStatusEnumTypeHelper = {
    {GetInstalledCertificateStatusEnumType::Accepted, "Accepted"}, {GetInstalledCertificateStatusEnumType::NotFound, "NotFound"}};

} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool GetInstalledCertificateIdsReqConverter::fromJson(const rapidjson::Value&        json,
                                                      GetInstalledCertificateIdsReq& data,
                                                      std::string&                   error_code,
                                                      std::string&                   error_message)
{
    (void)error_code;
    (void)error_message;
    data.certificateType = CertificateUseEnumTypeHelper.fromString(json["certificateType"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool GetInstalledCertificateIdsReqConverter::toJson(const GetInstalledCertificateIdsReq& data, rapidjson::Document& json)
{
    fill(json, "certificateType", CertificateUseEnumTypeHelper.toString(data.certificateType));
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool GetInstalledCertificateIdsConfConverter::fromJson(const rapidjson::Value&         json,
                                                       GetInstalledCertificateIdsConf& data,
                                                       std::string&                    error_code,
                                                       std::string&                    error_message)
{
    bool ret    = true;
    data.status = GetInstalledCertificateStatusEnumTypeHelper.fromString(json["status"].GetString());
    if (json.HasMember("certificateHashData"))
    {
        const rapidjson::Value&          certificateHashData = json["certificateHashData"];
        CertificateHashDataTypeConverter certificate_hash_converter;
        for (auto it_cert = certificateHashData.Begin(); ret && (it_cert != certificateHashData.End()); ++it_cert)
        {
            data.certificateHashData.emplace_back();
            CertificateHashDataType& certificate_hash = data.certificateHashData.back();
            ret = ret && certificate_hash_converter.fromJson(*it_cert, certificate_hash, error_code, error_message);
        }
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool GetInstalledCertificateIdsConfConverter::toJson(const GetInstalledCertificateIdsConf& data, rapidjson::Document& json)
{
    bool ret = true;
    fill(json, "status", GetInstalledCertificateStatusEnumTypeHelper.toString(data.status));
    if (!data.certificateHashData.empty())
    {
        rapidjson::Value                 certificateHashData(rapidjson::kArrayType);
        CertificateHashDataTypeConverter certificate_hash_converter;
        certificate_hash_converter.setAllocator(allocator);
        for (const CertificateHashDataType& certificate_hash : data.certificateHashData)
        {
            rapidjson::Document value(rapidjson::kObjectType);
            ret = ret && certificate_hash_converter.toJson(certificate_hash, value);
            certificateHashData.PushBack(value.Move(), *allocator);
        }
        json.AddMember(rapidjson::StringRef("certificateHashData"), certificateHashData.Move(), *allocator);
    }
    return ret;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
