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

#include "Iso15118Authorize.h"
#include "IRpc.h"
#include "IdTokenInfoTypeConverter.h"
#include "OcspRequestDataTypeConverter.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{
/** @brief Helper to convert a AuthorizeCertificateStatusEnumType enum to string */
const EnumToStringFromString<AuthorizeCertificateStatusEnumType> AuthorizeCertificateStatusEnumTypeHelper = {
    {AuthorizeCertificateStatusEnumType::Accepted, "Accepted"},
    {AuthorizeCertificateStatusEnumType::CertChainError, "CertChainError"},
    {AuthorizeCertificateStatusEnumType::CertificateExpired, "CertificateExpired"},
    {AuthorizeCertificateStatusEnumType::CertificateRevoked, "CertificateRevoked"},
    {AuthorizeCertificateStatusEnumType::ContractCancelled, "ContractCancelled"},
    {AuthorizeCertificateStatusEnumType::NoCertificateAvailable, "NoCertificateAvailable"},
    {AuthorizeCertificateStatusEnumType::SignatureError, "SignatureError"}};
} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool Iso15118AuthorizeReqConverter::fromJson(const rapidjson::Value& json,
                                             Iso15118AuthorizeReq&   data,
                                             std::string&            error_code,
                                             std::string&            error_message)
{
    bool ret = true;
    extract(json, "certificate", data.certificate);
    extract(json, "idToken", data.idToken);
    if (json.HasMember("iso15118CertificateHashData"))
    {
        const rapidjson::Value&      certificateHashData = json["iso15118CertificateHashData"];
        OcspRequestDataTypeConverter certificate_hash_converter;
        for (auto it_cert = certificateHashData.Begin(); ret && (it_cert != certificateHashData.End()); ++it_cert)
        {
            data.iso15118CertificateHashData.emplace_back();
            OcspRequestDataType& certificate_hash = data.iso15118CertificateHashData.back();
            ret = ret && certificate_hash_converter.fromJson(*it_cert, certificate_hash, error_code, error_message);
        }
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool Iso15118AuthorizeReqConverter::toJson(const Iso15118AuthorizeReq& data, rapidjson::Document& json)
{
    bool ret = true;
    if (data.certificate.isSet())
    {
        fill(json, "certificate", data.certificate);
    }
    fill(json, "idToken", data.idToken);
    if (!data.iso15118CertificateHashData.empty())
    {
        rapidjson::Value             certificateHashData(rapidjson::kArrayType);
        OcspRequestDataTypeConverter certificate_hash_converter;
        certificate_hash_converter.setAllocator(allocator);
        for (const OcspRequestDataType& certificate_hash : data.iso15118CertificateHashData)
        {
            rapidjson::Document value(rapidjson::kObjectType);
            ret = ret && certificate_hash_converter.toJson(certificate_hash, value);
            certificateHashData.PushBack(value.Move(), *allocator);
        }
        json.AddMember(rapidjson::StringRef("iso15118CertificateHashData"), certificateHashData.Move(), *allocator);
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool Iso15118AuthorizeConfConverter::fromJson(const rapidjson::Value& json,
                                              Iso15118AuthorizeConf&  data,
                                              std::string&            error_code,
                                              std::string&            error_message)
{
    IdTokenInfoTypeConverter id_token_info_converter;
    bool                     ret = id_token_info_converter.fromJson(json["idTokenInfo"], data.idTokenInfo, error_code, error_message);
    if (json.HasMember("certificateStatus"))
    {
        data.certificateStatus = AuthorizeCertificateStatusEnumTypeHelper.fromString(json["certificateStatus"].GetString());
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool Iso15118AuthorizeConfConverter::toJson(const Iso15118AuthorizeConf& data, rapidjson::Document& json)
{
    IdTokenInfoTypeConverter id_token_info_converter;
    id_token_info_converter.setAllocator(allocator);
    rapidjson::Document id_token_info(rapidjson::kObjectType);
    bool                ret = id_token_info_converter.toJson(data.idTokenInfo, id_token_info);
    json.AddMember(rapidjson::StringRef("idTokenInfo"), id_token_info.Move(), *allocator);
    if (data.certificateStatus.isSet())
    {
        fill(json, "certificateStatus", AuthorizeCertificateStatusEnumTypeHelper.toString(data.certificateStatus));
    }
    return ret;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
