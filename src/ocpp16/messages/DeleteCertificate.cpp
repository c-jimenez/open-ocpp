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

#include "DeleteCertificate.h"
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
/** @brief Helper to convert a enum class DeleteCertificateStatusEnumType enum to string */
const EnumToStringFromString<DeleteCertificateStatusEnumType> DeleteCertificateStatusEnumTypeHelper = {
    {DeleteCertificateStatusEnumType::Accepted, "Accepted"},
    {DeleteCertificateStatusEnumType::Failed, "Failed"},
    {DeleteCertificateStatusEnumType::NotFound, "NotFound"}};

} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool DeleteCertificateReqConverter::fromJson(const rapidjson::Value& json,
                                             DeleteCertificateReq&   data,
                                             std::string&            error_code,
                                             std::string&            error_message)
{
    CertificateHashDataTypeConverter certificate_hash_converter;
    return certificate_hash_converter.fromJson(json["certificateHashData"], data.certificateHashData, error_code, error_message);
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool DeleteCertificateReqConverter::toJson(const DeleteCertificateReq& data, rapidjson::Document& json)
{
    CertificateHashDataTypeConverter certificate_hash_converter;
    certificate_hash_converter.setAllocator(allocator);
    rapidjson::Document value(rapidjson::kObjectType);
    bool                ret = certificate_hash_converter.toJson(data.certificateHashData, value);
    json.AddMember(rapidjson::StringRef("certificateHashData"), value.Move(), *allocator);
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool DeleteCertificateConfConverter::fromJson(const rapidjson::Value& json,
                                              DeleteCertificateConf&  data,
                                              std::string&            error_code,
                                              std::string&            error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = DeleteCertificateStatusEnumTypeHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool DeleteCertificateConfConverter::toJson(const DeleteCertificateConf& data, rapidjson::Document& json)
{
    fill(json, "status", DeleteCertificateStatusEnumTypeHelper.toString(data.status));
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
