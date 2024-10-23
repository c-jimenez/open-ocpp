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

#ifndef OPENOCPP_INSTALLCERTIFICATE_H
#define OPENOCPP_INSTALLCERTIFICATE_H

#include "CiStringType.h"
#include "Enums.h"
#include "IMessageConverter.h"

#include <vector>

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the InstallCertificate messages */
static const std::string INSTALL_CERTIFICATE_ACTION = "InstallCertificate";

/** @brief InstallCertificate.req message */
struct InstallCertificateReq
{
    /** @brief Required. Indicates the certificate type that is sent */
    ocpp::types::ocpp16::CertificateUseEnumType certificateType;
    /** @brief Required. An PEM encoded X.509 certificate */
    ocpp::types::CiStringType<5500u> certificate;
};

/** @brief InstallCertificate.conf message */
struct InstallCertificateConf
{
    /** @brief Required. Charge Point indicates if installation was successful */
    ocpp::types::ocpp16::CertificateStatusEnumType status;
};

// Message converters
MESSAGE_CONVERTERS(InstallCertificate)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_INSTALLCERTIFICATE_H
