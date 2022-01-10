/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CERTIFICATEHASHDATATYPE_H
#define CERTIFICATEHASHDATATYPE_H

#include "CiStringType.h"
#include "Enums.h"

namespace ocpp
{
namespace types
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
};

} // namespace types
} // namespace ocpp

#endif // CERTIFICATEHASHDATATYPE_H
