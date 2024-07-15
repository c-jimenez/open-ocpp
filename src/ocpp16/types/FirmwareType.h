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

#ifndef OPENOCPP_FIRMWARETYPE_H
#define OPENOCPP_FIRMWARETYPE_H

#include "CiStringType.h"
#include "DateTime.h"
#include "Optional.h"

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief Represents a copy of the firmware that can be loaded/updated on the Charge Point.
           FirmwareType is used by: SignedUpdateFirmware.req */
struct FirmwareType
{
    /** @brief Required. URI defining the origin of the firmware */
    CiStringType<512> location;
    /** @brief Required. Date and time at which the firmware shall be retrieved */
    DateTime retrieveDateTime;
    /** @brief Optional. Date and time at which the firmware shall be installed */
    Optional<DateTime> installDateTime;
    /** @brief Required. Certificate with which the firmware was signed. PEM encoded X.509 certificate */
    CiStringType<5500> signingCertificate;
    /** @brief Required. Base64 encoded firmware signature */
    CiStringType<800> signature;
};

} // namespace ocpp16
} // namespace types
} // namespace ocpp

#endif // OPENOCPP_FIRMWARETYPE_H
