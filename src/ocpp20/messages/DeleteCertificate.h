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
    Generated by json2cpp from : urn:OCPP:Cp:2:2020:3:DeleteCertificateRequest
                                 OCPP 2.0.1 FINAL
*/

#ifndef OPENOCPP_DELETECERTIFICATE_H
#define OPENOCPP_DELETECERTIFICATE_H

#include "IMessageConverter.h"

#include "CustomDataType.h"
#include "CertificateHashDataType.h"
#include "DeleteCertificateStatusEnumType.h"
#include "StatusInfoType.h"

namespace ocpp
{
namespace messages
{
namespace ocpp20
{

/** @brief Action corresponding to the DeleteCertificate messages */
static const std::string DELETECERTIFICATE_ACTION = "DeleteCertificate";

/** @brief DeleteCertificateReq message */
struct DeleteCertificateReq
{
    /** @brief  */
    ocpp::types::Optional<ocpp::types::ocpp20::CustomDataType> customData;
    /** @brief  */
    ocpp::types::ocpp20::CertificateHashDataType certificateHashData;
};

/** @brief DeleteCertificateConf message */
struct DeleteCertificateConf
{
    /** @brief  */
    ocpp::types::Optional<ocpp::types::ocpp20::CustomDataType> customData;
    /** @brief  */
    ocpp::types::ocpp20::DeleteCertificateStatusEnumType status;
    /** @brief  */
    ocpp::types::Optional<ocpp::types::ocpp20::StatusInfoType> statusInfo;
};

// Message converters
MESSAGE_CONVERTERS(DeleteCertificate)

} // namespace ocpp20
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_DELETECERTIFICATE_H