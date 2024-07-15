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

#ifndef OPENOCPP_ISO15118AUTHORIZE_H
#define OPENOCPP_ISO15118AUTHORIZE_H

#include "IMessageConverter.h"
#include "IdToken.h"
#include "IdTokenInfoType.h"
#include "OcspRequestDataType.h"
#include "Optional.h"

#include <vector>

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the Iso15118Authorize messages */
static const std::string ISO15118_AUTHORIZE_ACTION = "Authorize";

/** @brief Iso15118Authorize.req message */
struct Iso15118AuthorizeReq
{
    /** @brief Optional. The X.509 certificated presented by EV and encoded in PEM format */
    ocpp::types::Optional<ocpp::types::CiStringType<5500u>> certificate;
    /** @brief Required. This contains the identifier that needs to be authorized */
    ocpp::types::ocpp16::IdToken idToken;
    /** @brief Optional. Contains the information needed to verify the EV Contract Certificate via OCSP */
    std::vector<ocpp::types::ocpp16::OcspRequestDataType> iso15118CertificateHashData;
};

/** @brief Iso15118Authorize.conf message */
struct Iso15118AuthorizeConf
{
    /** @brief Optional. Certificate status information. - if all certificates are
               valid: return 'Accepted'. - if one of the certificates was revoked,
               return 'CertificateRevoked' */
    ocpp::types::Optional<ocpp::types::ocpp16::AuthorizeCertificateStatusEnumType> certificateStatus;
    /** @brief Required. This contains information about authorization status,
               expiry and group id */
    ocpp::types::ocpp16::IdTokenInfoType idTokenInfo;
};

// Message converters
MESSAGE_CONVERTERS(Iso15118Authorize)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_ISO15118AUTHORIZE_H
