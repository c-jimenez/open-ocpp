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

#ifndef OPENOCPP_GETINSTALLEDCERTIFICATEIDS_H
#define OPENOCPP_GETINSTALLEDCERTIFICATEIDS_H

#include "CertificateHashDataType.h"
#include "Enums.h"
#include "IMessageConverter.h"

#include <vector>

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the GetInstalledCertificateIds messages */
static const std::string GET_INSTALLED_CERTIFICATE_IDS_ACTION = "GetInstalledCertificateIds";

/** @brief GetInstalledCertificateIds.req message */
struct GetInstalledCertificateIdsReq
{
    /** @brief Required. Indicates the type of certificates requested */
    ocpp::types::ocpp16::CertificateUseEnumType certificateType;
};

/** @brief GetInstalledCertificateIds.conf message */
struct GetInstalledCertificateIdsConf
{
    /** @brief Required. Charge Point indicates if it can process the request */
    ocpp::types::ocpp16::GetInstalledCertificateStatusEnumType status;
    /** @brief Optional. The Charge Point includes the Certificate information for each
               available certificate */
    std::vector<ocpp::types::ocpp16::CertificateHashDataType> certificateHashData;
};

// Message converters
MESSAGE_CONVERTERS(GetInstalledCertificateIds)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_GETINSTALLEDCERTIFICATEIDS_H
