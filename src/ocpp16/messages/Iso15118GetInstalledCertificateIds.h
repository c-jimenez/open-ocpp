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

#ifndef OPENOCPP_ISO15118GETINSTALLEDCERTIFICATEIDS_H
#define OPENOCPP_ISO15118GETINSTALLEDCERTIFICATEIDS_H

#include "CertificateHashDataChainType.h"
#include "Enums.h"
#include "IMessageConverter.h"

#include <vector>

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the Iso15118GetInstalledCertificateIds messages */
static const std::string ISO15118_GET_INSTALLED_CERTIFICATE_IDS_ACTION = "GetInstalledCertificateIds";

/** @brief Iso15118GetInstalledCertificateIds.req message */
struct Iso15118GetInstalledCertificateIdsReq
{
    /** @brief Optional. Indicates the type of certificates requested. When omitted, all certificate types are requested */
    std::vector<ocpp::types::ocpp16::GetCertificateIdUseEnumType> certificateType;
};

/** @brief Iso15118GetInstalledCertificateIds.conf message */
struct Iso15118GetInstalledCertificateIdsConf
{
    /** @brief Required. Charge Point indicates if it can process the request */
    ocpp::types::ocpp16::GetInstalledCertificateStatusEnumType status;
    /** @brief Optional. The Charge Point includes the Certificate information
               for each available certificate */
    std::vector<ocpp::types::ocpp16::CertificateHashDataChainType> certificateHashDataChain;
};

// Message converters
MESSAGE_CONVERTERS(Iso15118GetInstalledCertificateIds)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_ISO15118GETINSTALLEDCERTIFICATEIDS_H
