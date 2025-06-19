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

#include "Iso15118GetInstalledCertificateIds.h"
#include "CertificateHashDataChainTypeConverter.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace messages
{
namespace ocpp16
{
/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool Iso15118GetInstalledCertificateIdsReqConverter::fromJson(const rapidjson::Value&                json,
                                                              Iso15118GetInstalledCertificateIdsReq& data,
                                                              std::string&                           error_code,
                                                              std::string&                           error_message)
{
    (void)error_code;
    (void)error_message;
    if (json.HasMember("certificateType"))
    {
        const rapidjson::Value& certificateType = json["certificateType"];
        for (auto it_cert = certificateType.Begin(); it_cert != certificateType.End(); ++it_cert)
        {
            data.certificateType.push_back(GetCertificateIdUseEnumTypeHelper.fromString(it_cert->GetString()));
        }
    }
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool Iso15118GetInstalledCertificateIdsReqConverter::toJson(const Iso15118GetInstalledCertificateIdsReq& data, rapidjson::Document& json)
{
    if (!data.certificateType.empty())
    {
        rapidjson::Value certificateType(rapidjson::kArrayType);
        for (const GetCertificateIdUseEnumType& cert_type : data.certificateType)
        {
            rapidjson::Value value(GetCertificateIdUseEnumTypeHelper.toString(cert_type).c_str(), *allocator);
            certificateType.PushBack(value.Move(), *allocator);
        }
        json.AddMember(rapidjson::StringRef("certificateType"), certificateType.Move(), *allocator);
    }
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool Iso15118GetInstalledCertificateIdsConfConverter::fromJson(const rapidjson::Value&                 json,
                                                               Iso15118GetInstalledCertificateIdsConf& data,
                                                               std::string&                            error_code,
                                                               std::string&                            error_message)
{
    bool ret    = true;
    data.status = GetInstalledCertificateStatusEnumTypeHelper.fromString(json["status"].GetString());
    if (json.HasMember("certificateHashDataChain"))
    {
        const rapidjson::Value&               certificateHashDataChain = json["certificateHashDataChain"];
        CertificateHashDataChainTypeConverter certificate_hash_converter;
        for (auto it_cert = certificateHashDataChain.Begin(); ret && (it_cert != certificateHashDataChain.End()); ++it_cert)
        {
            data.certificateHashDataChain.emplace_back();
            CertificateHashDataChainType& certificate_hash = data.certificateHashDataChain.back();
            ret = ret && certificate_hash_converter.fromJson(*it_cert, certificate_hash, error_code, error_message);
        }
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool Iso15118GetInstalledCertificateIdsConfConverter::toJson(const Iso15118GetInstalledCertificateIdsConf& data, rapidjson::Document& json)
{
    bool ret = true;
    fill(json, "status", GetInstalledCertificateStatusEnumTypeHelper.toString(data.status));
    if (!data.certificateHashDataChain.empty())
    {
        rapidjson::Value                      certificateHashDataChain(rapidjson::kArrayType);
        CertificateHashDataChainTypeConverter certificate_hash_converter;
        certificate_hash_converter.setAllocator(allocator);
        for (const CertificateHashDataChainType& certificate_hash : data.certificateHashDataChain)
        {
            rapidjson::Document value(rapidjson::kObjectType);
            ret = ret && certificate_hash_converter.toJson(certificate_hash, value);
            certificateHashDataChain.PushBack(value.Move(), *allocator);
        }
        json.AddMember(rapidjson::StringRef("certificateHashDataChain"), certificateHashDataChain.Move(), *allocator);
    }
    return ret;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
