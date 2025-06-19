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

#include "GetCertificateStatus.h"
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
/** @brief Helper to convert a GetCertificateStatusEnumType enum to string */
const EnumToStringFromString<GetCertificateStatusEnumType> GetCertificateStatusEnumTypeHelper = {
    {GetCertificateStatusEnumType::Accepted, "Accepted"}, {GetCertificateStatusEnumType::Failed, "Failed"}};
} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool GetCertificateStatusReqConverter::fromJson(const rapidjson::Value&  json,
                                                GetCertificateStatusReq& data,
                                                std::string&             error_code,
                                                std::string&             error_message)
{
    const rapidjson::Value&      ocspRequestData = json["ocspRequestData"];
    OcspRequestDataTypeConverter ocsp_request_converter;
    bool                         ret = ocsp_request_converter.fromJson(ocspRequestData, data.ocspRequestData, error_code, error_message);
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool GetCertificateStatusReqConverter::toJson(const GetCertificateStatusReq& data, rapidjson::Document& json)
{
    OcspRequestDataTypeConverter ocsp_request_converter;
    ocsp_request_converter.setAllocator(allocator);

    rapidjson::Document value(rapidjson::kObjectType);
    bool                ret = ocsp_request_converter.toJson(data.ocspRequestData, value);
    if (ret)
    {
        json.AddMember(rapidjson::StringRef("ocspRequestData"), value.Move(), *allocator);
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool GetCertificateStatusConfConverter::fromJson(const rapidjson::Value&   json,
                                                 GetCertificateStatusConf& data,
                                                 std::string&              error_code,
                                                 std::string&              error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = GetCertificateStatusEnumTypeHelper.fromString(json["status"].GetString());
    extract(json, "ocspResult", data.ocspResult);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool GetCertificateStatusConfConverter::toJson(const GetCertificateStatusConf& data, rapidjson::Document& json)
{
    fill(json, "status", GetCertificateStatusEnumTypeHelper.toString(data.status));
    fill(json, "ocspResult", data.ocspResult);
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
