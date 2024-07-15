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

#ifndef OPENOCPP_SIGNCERTIFICATE_H
#define OPENOCPP_SIGNCERTIFICATE_H

#include "CiStringType.h"
#include "Enums.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the SignCertificate messages */
static const std::string SIGN_CERTIFICATE_ACTION = "SignCertificate";

/** @brief SignCertificate.req message */
struct SignCertificateReq
{
    /** @brief Required. The Charge Point SHALL send the public key in form of a Certificate
               Signing Request (CSR) as described in RFC 2986 [14] and then PEM encoded,
               using the SignCertificate.req message */
    ocpp::types::CiStringType<5500u> csr;
};

/** @brief SignCertificate.conf message */
struct SignCertificateConf
{
    /** @brief Required. Specifies whether the Central System can process the request */
    ocpp::types::ocpp16::GenericStatusEnumType status;
};

// Message converters
MESSAGE_CONVERTERS(SignCertificate)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_SIGNCERTIFICATE_H
