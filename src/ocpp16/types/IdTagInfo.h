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

#ifndef OPENOCPP_IDTAGINFO_H
#define OPENOCPP_IDTAGINFO_H

#include "CiStringType.h"
#include "DateTime.h"
#include "Enums.h"
#include "IdToken.h"
#include "Optional.h"

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief Contains status information about an identifier. It is returned in Authorize, Start Transaction and Stop
           Transaction responses.
           If expiryDate is not given, the status has no end date. */
struct IdTagInfo
{
    /** @brief Optional. This contains the date at which idTag should be removed from the
               Authorization Cache */
    Optional<DateTime> expiryDate;
    /** @brief Optional. This contains the parent-identifier */
    Optional<IdToken> parentIdTag;

    /** @brief Required. This contains whether the idTag has been accepted or not by the
               Central System */
    AuthorizationStatus status;
};

} // namespace ocpp16
} // namespace types
} // namespace ocpp

#endif // OPENOCPP_IDTAGINFO_H
