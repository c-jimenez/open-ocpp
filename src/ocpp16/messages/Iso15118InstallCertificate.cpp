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

#include "Iso15118InstallCertificate.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief Helper to convert a enum class InstallCertificateUseEnumType enum to string */
const EnumToStringFromString<InstallCertificateUseEnumType> InstallCertificateUseEnumTypeHelper = {
    {InstallCertificateUseEnumType::MORootCertificate, "MORootCertificate"},
    {InstallCertificateUseEnumType::V2GRootCertificate, "V2GRootCertificate"},
    {InstallCertificateUseEnumType::OEMRootCertificate, "OEMRootCertificate"}};

/** @brief Helper to convert a enum class InstallCertificateStatusEnumType enum to string */
const EnumToStringFromString<InstallCertificateStatusEnumType> InstallCertificateStatusEnumTypeHelper = {
    {InstallCertificateStatusEnumType::Accepted, "Accepted"},
    {InstallCertificateStatusEnumType::Failed, "Failed"},
    {InstallCertificateStatusEnumType::Rejected, "Rejected"}};

} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool Iso15118InstallCertificateReqConverter::fromJson(const rapidjson::Value&        json,
                                                      Iso15118InstallCertificateReq& data,
                                                      std::string&                   error_code,
                                                      std::string&                   error_message)
{
    (void)error_code;
    (void)error_message;
    data.certificateType = InstallCertificateUseEnumTypeHelper.fromString(json["certificateType"].GetString());
    extract(json, "certificate", data.certificate);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool Iso15118InstallCertificateReqConverter::toJson(const Iso15118InstallCertificateReq& data, rapidjson::Document& json)
{
    fill(json, "certificateType", InstallCertificateUseEnumTypeHelper.toString(data.certificateType));
    fill(json, "certificate", data.certificate);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool Iso15118InstallCertificateConfConverter::fromJson(const rapidjson::Value&         json,
                                                       Iso15118InstallCertificateConf& data,
                                                       std::string&                    error_code,
                                                       std::string&                    error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = InstallCertificateStatusEnumTypeHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool Iso15118InstallCertificateConfConverter::toJson(const Iso15118InstallCertificateConf& data, rapidjson::Document& json)
{
    fill(json, "status", InstallCertificateStatusEnumTypeHelper.toString(data.status));
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
