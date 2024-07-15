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

#ifndef OPENOCPP_KEYVALUE_H
#define OPENOCPP_KEYVALUE_H

#include "CiStringType.h"
#include "Optional.h"

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief Contains information about a specific configuration key. It is returned in GetConfiguration.conf */
struct KeyValue
{
    /** @brief Required. */
    CiStringType<50u> key;
    /** @brief Required. False if the value can be set with the ChangeConfiguration message */
    bool readonly;
    /** @brief Optional. If key is known but not set, this field may be absent */
    Optional<CiStringType<500u>> value;
};

} // namespace ocpp16
} // namespace types
} // namespace ocpp

#endif // OPENOCPP_KEYVALUE_H
