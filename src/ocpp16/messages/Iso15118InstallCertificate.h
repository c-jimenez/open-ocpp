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

#ifndef OPENOCPP_ISO15118INSTALLCERTIFICATE_H
#define OPENOCPP_ISO15118INSTALLCERTIFICATE_H

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

/** @brief Action corresponding to the Iso15118InstallCertificate messages */
static const std::string ISO15118_INSTALL_CERTIFICATE_ACTION = "InstallCertificate";

/** @brief Iso15118InstallCertificate.req message */
struct Iso15118InstallCertificateReq
{
    /** @brief Required. Indicates the certificate type that is sent */
    ocpp::types::ocpp16::InstallCertificateUseEnumType certificateType;
    /** @brief Required. An PEM encoded X.509 certificate */
    ocpp::types::CiStringType<5500u> certificate;
};

/** @brief Iso15118InstallCertificate.conf message */
struct Iso15118InstallCertificateConf
{
    /** @brief Required. Charge Point indicates if installation was successful */
    ocpp::types::ocpp16::InstallCertificateStatusEnumType status;
};

// Message converters
MESSAGE_CONVERTERS(Iso15118InstallCertificate)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_ISO15118INSTALLCERTIFICATE_H
