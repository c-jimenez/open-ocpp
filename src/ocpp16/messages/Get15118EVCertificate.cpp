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

#include "Get15118EVCertificate.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{
/** @brief Helper to convert a enum class CertificateActionEnumType enum to string */
const EnumToStringFromString<CertificateActionEnumType> CertificateActionEnumTypeHelper = {{CertificateActionEnumType::Install, "Install"},
                                                                                           {CertificateActionEnumType::Update, "Update"}};

/** @brief Helper to convert a enum class Iso15118EVCertificateStatusEnumType enum to string */
const EnumToStringFromString<Iso15118EVCertificateStatusEnumType> Iso15118EVCertificateStatusEnumTypeHelper = {
    {Iso15118EVCertificateStatusEnumType::Accepted, "Accepted"}, {Iso15118EVCertificateStatusEnumType::Failed, "Failed"}};

} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool Get15118EVCertificateReqConverter::fromJson(const rapidjson::Value&   json,
                                                 Get15118EVCertificateReq& data,
                                                 std::string&              error_code,
                                                 std::string&              error_message)
{
    (void)error_code;
    (void)error_message;
    extract(json, "iso15118SchemaVersion", data.iso15118SchemaVersion);
    data.action = CertificateActionEnumTypeHelper.fromString(json["action"].GetString());
    extract(json, "exiRequest", data.exiRequest);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool Get15118EVCertificateReqConverter::toJson(const Get15118EVCertificateReq& data, rapidjson::Document& json)
{
    fill(json, "iso15118SchemaVersion", data.iso15118SchemaVersion);
    fill(json, "action", CertificateActionEnumTypeHelper.toString(data.action));
    fill(json, "exiRequest", data.exiRequest);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool Get15118EVCertificateConfConverter::fromJson(const rapidjson::Value&    json,
                                                  Get15118EVCertificateConf& data,
                                                  std::string&               error_code,
                                                  std::string&               error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = Iso15118EVCertificateStatusEnumTypeHelper.fromString(json["status"].GetString());
    extract(json, "exiResponse", data.exiResponse);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool Get15118EVCertificateConfConverter::toJson(const Get15118EVCertificateConf& data, rapidjson::Document& json)
{
    fill(json, "status", Iso15118EVCertificateStatusEnumTypeHelper.toString(data.status));
    fill(json, "exiResponse", data.exiResponse);
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
