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

#ifndef OPENOCPP_CERTIFICATESIGNED_H
#define OPENOCPP_CERTIFICATESIGNED_H

#include "CiStringType.h"
#include "Enums.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the CertificateSigned messages */
static const std::string CERTIFICATE_SIGNED_ACTION = "CertificateSigned";

/** @brief CertificateSigned.req message */
struct CertificateSignedReq
{
    /** @brief Required. The signed PEM encoded X.509 certificates. This can also contain the
               necessary sub CA certificates. The maximum size of this field is be limited by the
               configuration key: CertificateSignedMaxSize. */
    ocpp::types::CiStringType<10000u> certificateChain;
};

/** @brief CertificateSigned.conf message */
struct CertificateSignedConf
{
    /** @brief Required. Returns whether certificate signing has been accepted, otherwise
               rejected */
    ocpp::types::ocpp16::CertificateSignedStatusEnumType status;
};

// Message converters
MESSAGE_CONVERTERS(CertificateSigned)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_CERTIFICATESIGNED_H
