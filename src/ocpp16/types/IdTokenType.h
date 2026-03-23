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

#ifndef OPENOCPP_IDTOKENTYPE_H
#define OPENOCPP_IDTOKENTYPE_H

#include "CiStringType.h"
#include "Enums.h"

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief IdTokenType is used by: Authorize.req */
struct IdTokenType
{
    /** @brief Required. IdToken is case insensitive. Might hold the hidden id of an RFID tag, but can for example also contain a UUID. */
    CiStringType<20u> idToken;
};

} // namespace ocpp16
} // namespace types
} // namespace ocpp

#endif // OPENOCPP_IDTOKENTYPE_H
