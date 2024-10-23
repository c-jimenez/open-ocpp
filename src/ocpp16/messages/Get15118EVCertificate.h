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

#ifndef OPENOCPP_GET15118EVCERTIFICATE_H
#define OPENOCPP_GET15118EVCERTIFICATE_H

#include "CiStringType.h"
#include "Enums.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the Get15118EVCertificate messages */
static const std::string GET_15118_EV_CERTIFICATE_ACTION = "Get15118EVCertificate";

/** @brief Get15118EVCertificate.req message */
struct Get15118EVCertificateReq
{
    /** @brief Required. Schema version currently used for the 15118 session
               between EV and Charge Point. Needed for parsing of the EXI
               stream by the Central System */
    ocpp::types::CiStringType<50u> iso15118SchemaVersion;
    /** @brief Required. Defines whether certificate needs to be installed or updated */
    ocpp::types::ocpp16::CertificateActionEnumType action;
    /** @brief Required. Raw CertificateInstallationReq request from EV, Base64 encoded */
    ocpp::types::CiStringType<5600u> exiRequest;
};

/** @brief Get15118EVCertificate.conf message */
struct Get15118EVCertificateConf
{
    /** @brief Required. Indicates whether the message was processed properly */
    ocpp::types::ocpp16::Iso15118EVCertificateStatusEnumType status;
    /** @brief Required. Raw CertificateInstallationRes response for the EV, Base64 encoded */
    ocpp::types::CiStringType<5600u> exiResponse;
};

// Message converters
MESSAGE_CONVERTERS(Get15118EVCertificate)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_GET15118EVCERTIFICATE_H
