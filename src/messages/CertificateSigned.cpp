/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#include "CertificateSigned.h"
#include "IRpc.h"

using namespace ocpp::types;

namespace ocpp
{
namespace types
{
/** @brief Helper to convert a enum class CertificateSignedStatusEnumType enum to string */
const EnumToStringFromString<CertificateSignedStatusEnumType> CertificateSignedStatusEnumTypeHelper = {
    {CertificateSignedStatusEnumType::Accepted, "Accepted"}, {CertificateSignedStatusEnumType::Rejected, "Rejected"}};

} // namespace types
namespace messages
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, const char*&, std::string&) */
bool CertificateSignedReqConverter::fromJson(const rapidjson::Value& json,
                                             CertificateSignedReq&   data,
                                             const char*&            error_code,
                                             std::string&            error_message)
{
    (void)error_code;
    (void)error_message;
    extract(json, "certificateChain", data.certificateChain);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, const char*&, std::string&) */
bool CertificateSignedReqConverter::toJson(const CertificateSignedReq& data, rapidjson::Document& json)
{
    fill(json, "certificateChain", data.certificateChain);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, const char*&, std::string&) */
bool CertificateSignedConfConverter::fromJson(const rapidjson::Value& json,
                                              CertificateSignedConf&  data,
                                              const char*&            error_code,
                                              std::string&            error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = CertificateSignedStatusEnumTypeHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, const char*&, std::string&) */
bool CertificateSignedConfConverter::toJson(const CertificateSignedConf& data, rapidjson::Document& json)
{
    fill(json, "status", CertificateSignedStatusEnumTypeHelper.toString(data.status));
    return true;
}

} // namespace messages
} // namespace ocpp
