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

#ifndef OPENOCPP_CERTIFICATEHASHDATATYPE_H
#define OPENOCPP_CERTIFICATEHASHDATATYPE_H

#include "CiStringType.h"
#include "Enums.h"

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief CertificateHashDataType is used by: DeleteCertificate.req, GetInstalledCertificateIds.conf */
struct CertificateHashDataType
{
    /** @brief Required. Used algorithms for the hashes provided */
    HashAlgorithmEnumType hashAlgorithm;
    /** @brief Required. hashed value of the IssuerName */
    CiStringType<128u> issuerNameHash;
    /** @brief Required. Hashed value of the issuers public key */
    CiStringType<128u> issuerKeyHash;
    /** @brief Required. Required. The serial number of the certificate */
    CiStringType<40u> serialNumber;
};

} // namespace ocpp16
} // namespace types
} // namespace ocpp

#endif // OPENOCPP_CERTIFICATEHASHDATATYPE_H
