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

#include "SignCertificate.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief Helper to convert a enum class GenericStatusEnumType enum to string */
const EnumToStringFromString<GenericStatusEnumType> GenericStatusEnumTypeHelper = {{GenericStatusEnumType::Accepted, "Accepted"},
                                                                                   {GenericStatusEnumType::Rejected, "Rejected"}};

} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool SignCertificateReqConverter::fromJson(const rapidjson::Value& json,
                                           SignCertificateReq&     data,
                                           std::string&            error_code,
                                           std::string&            error_message)
{
    (void)error_code;
    (void)error_message;
    extract(json, "csr", data.csr);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool SignCertificateReqConverter::toJson(const SignCertificateReq& data, rapidjson::Document& json)
{
    fill(json, "csr", data.csr);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool SignCertificateConfConverter::fromJson(const rapidjson::Value& json,
                                            SignCertificateConf&    data,
                                            std::string&            error_code,
                                            std::string&            error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = GenericStatusEnumTypeHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool SignCertificateConfConverter::toJson(const SignCertificateConf& data, rapidjson::Document& json)
{
    fill(json, "status", GenericStatusEnumTypeHelper.toString(data.status));
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
