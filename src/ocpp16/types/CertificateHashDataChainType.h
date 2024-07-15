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

#ifndef OPENOCPP_CERTIFICATEHASHDATACHAINTYPE_H
#define OPENOCPP_CERTIFICATEHASHDATACHAINTYPE_H

#include "CertificateHashDataType.h"
#include "Enums.h"

#include <vector>

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief CertificateHashDataChainType is used by: GetInstalledCertificateIds.conf */
struct CertificateHashDataChainType
{
    /** @brief Required. Indicates the type of the requested certificate(s) */
    GetCertificateIdUseEnumType certificateType;
    /** @brief Required. Information to identify a certificate */
    CertificateHashDataType certificateHashData;
    /** @brief Optional. Information to identify the child certificate(s) */
    std::vector<CertificateHashDataType> childCertificateHashData;
};

} // namespace ocpp16
} // namespace types
} // namespace ocpp

#endif // OPENOCPP_CERTIFICATEHASHDATACHAINTYPE_H
