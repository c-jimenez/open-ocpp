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

#ifndef OPENOCPP_GETCERTIFICATESTATUS_H
#define OPENOCPP_GETCERTIFICATESTATUS_H

#include "CiStringType.h"
#include "Enums.h"
#include "IMessageConverter.h"
#include "OcspRequestDataType.h"
#include "Optional.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the GetCertificateStatus messages */
static const std::string GET_CERTIFICATE_STATUS_ACTION = "GetCertificateStatus";

/** @brief GetCertificateStatus.req message */
struct GetCertificateStatusReq
{
    /** @brief Required. Indicates the certificate of which the status is requested */
    ocpp::types::ocpp16::OcspRequestDataType ocspRequestData;
};

/** @brief GetCertificateStatus.conf message */
struct GetCertificateStatusConf
{
    /** @brief Required. This indicates whether the charging station was able to retrieve the OCSP certificate status */
    ocpp::types::ocpp16::GetCertificateStatusEnumType status;
    /** @brief Optional. OCSPResponse class as defined in IETF RFC 6960.
               DER encoded (as defined in IETF RFC 6960), and then base64
               encoded. MAY only be omitted when status is not Accepted */
    ocpp::types::Optional<ocpp::types::CiStringType<5500u>> ocspResult;
};

// Message converters
MESSAGE_CONVERTERS(GetCertificateStatus)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_GETCERTIFICATESTATUS_H
