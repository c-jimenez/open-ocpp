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
    Generated by json2cpp from : urn:OCPP:Cp:2:2020:3:CustomerInformationRequest
                                 OCPP 2.0.1 FINAL
*/

#include "CustomerInformation.h"

#include "IRpc.h"

namespace ocpp
{
namespace messages
{
namespace ocpp20
{

/** @brief Convert a CustomerInformationReq from a JSON representation */
bool CustomerInformationReqConverter::fromJson(const rapidjson::Value&       json,
                                     CustomerInformationReq&                 data,
                                     std::string&                  error_code,
                                     std::string&                  error_message)
{
    bool ret = true;

    // customData
    if (json.HasMember("customData"))
    {
    ocpp::types::ocpp20::CustomDataTypeConverter customData_converter;
    ret = ret && customData_converter.fromJson(json["customData"], data.customData, error_code, error_message);
    }

    // customerCertificate
    if (json.HasMember("customerCertificate"))
    {
    ocpp::types::ocpp20::CertificateHashDataTypeConverter customerCertificate_converter;
    ret = ret && customerCertificate_converter.fromJson(json["customerCertificate"], data.customerCertificate, error_code, error_message);
    }

    // idToken
    if (json.HasMember("idToken"))
    {
    ocpp::types::ocpp20::IdTokenTypeConverter idToken_converter;
    ret = ret && idToken_converter.fromJson(json["idToken"], data.idToken, error_code, error_message);
    }

    // requestId
    extract(json, "requestId", data.requestId);

    // report
    extract(json, "report", data.report);

    // clear
    extract(json, "clear", data.clear);

    // customerIdentifier
    extract(json, "customerIdentifier", data.customerIdentifier);

    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }

    return ret;
}

/** @brief Convert a CustomerInformationReq to a JSON representation */
bool CustomerInformationReqConverter::toJson(const CustomerInformationReq& data, rapidjson::Document& json) 
{
    bool ret = true;

    // customData
    if (data.customData.isSet())
    {
    ocpp::types::ocpp20::CustomDataTypeConverter customData_converter;
    customData_converter.setAllocator(allocator);
    rapidjson::Document customData_doc;
    customData_doc.Parse("{}");
    ret = ret && customData_converter.toJson(data.customData, customData_doc);
    json.AddMember(rapidjson::StringRef("customData"), customData_doc.Move(), *allocator);
    }

    // customerCertificate
    if (data.customerCertificate.isSet())
    {
    ocpp::types::ocpp20::CertificateHashDataTypeConverter customerCertificate_converter;
    customerCertificate_converter.setAllocator(allocator);
    rapidjson::Document customerCertificate_doc;
    customerCertificate_doc.Parse("{}");
    ret = ret && customerCertificate_converter.toJson(data.customerCertificate, customerCertificate_doc);
    json.AddMember(rapidjson::StringRef("customerCertificate"), customerCertificate_doc.Move(), *allocator);
    }

    // idToken
    if (data.idToken.isSet())
    {
    ocpp::types::ocpp20::IdTokenTypeConverter idToken_converter;
    idToken_converter.setAllocator(allocator);
    rapidjson::Document idToken_doc;
    idToken_doc.Parse("{}");
    ret = ret && idToken_converter.toJson(data.idToken, idToken_doc);
    json.AddMember(rapidjson::StringRef("idToken"), idToken_doc.Move(), *allocator);
    }

    // requestId
    fill(json, "requestId", data.requestId);

    // report
    fill(json, "report", data.report);

    // clear
    fill(json, "clear", data.clear);

    // customerIdentifier
    fill(json, "customerIdentifier", data.customerIdentifier);

    return ret;
}

/** @brief Convert a CustomerInformationConf from a JSON representation */
bool CustomerInformationConfConverter::fromJson(const rapidjson::Value&       json,
                                     CustomerInformationConf&                 data,
                                     std::string&                  error_code,
                                     std::string&                  error_message)
{
    bool ret = true;

    // customData
    if (json.HasMember("customData"))
    {
    ocpp::types::ocpp20::CustomDataTypeConverter customData_converter;
    ret = ret && customData_converter.fromJson(json["customData"], data.customData, error_code, error_message);
    }

    // status
    data.status = ocpp::types::ocpp20::CustomerInformationStatusEnumTypeHelper.fromString(json["status"].GetString());

    // statusInfo
    if (json.HasMember("statusInfo"))
    {
    ocpp::types::ocpp20::StatusInfoTypeConverter statusInfo_converter;
    ret = ret && statusInfo_converter.fromJson(json["statusInfo"], data.statusInfo, error_code, error_message);
    }

    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }

    return ret;
}

/** @brief Convert a CustomerInformationConf to a JSON representation */
bool CustomerInformationConfConverter::toJson(const CustomerInformationConf& data, rapidjson::Document& json) 
{
    bool ret = true;

    // customData
    if (data.customData.isSet())
    {
    ocpp::types::ocpp20::CustomDataTypeConverter customData_converter;
    customData_converter.setAllocator(allocator);
    rapidjson::Document customData_doc;
    customData_doc.Parse("{}");
    ret = ret && customData_converter.toJson(data.customData, customData_doc);
    json.AddMember(rapidjson::StringRef("customData"), customData_doc.Move(), *allocator);
    }

    // status
    fill(json, "status", ocpp::types::ocpp20::CustomerInformationStatusEnumTypeHelper.toString(data.status));

    // statusInfo
    if (data.statusInfo.isSet())
    {
    ocpp::types::ocpp20::StatusInfoTypeConverter statusInfo_converter;
    statusInfo_converter.setAllocator(allocator);
    rapidjson::Document statusInfo_doc;
    statusInfo_doc.Parse("{}");
    ret = ret && statusInfo_converter.toJson(data.statusInfo, statusInfo_doc);
    json.AddMember(rapidjson::StringRef("statusInfo"), statusInfo_doc.Move(), *allocator);
    }

    return ret;
}

} // namespace ocpp20
} // namespace messages
} // namespace ocpp