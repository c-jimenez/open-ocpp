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

#ifndef OPENOCPP_LOGPARAMETERSTYPE_H
#define OPENOCPP_LOGPARAMETERSTYPE_H

#include "CiStringType.h"
#include "DateTime.h"
#include "Optional.h"

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief Class for detailed information the retrieval of logging entries.
           LogParametersType is used by: GetLog.req */
struct LogParametersType
{
    /** @brief Required. The URL of the location at the remote system where the log should be
               stored */
    CiStringType<512> remoteLocation;
    /** @brief Optional. This contains the date and time of the oldest logging information to
               include in the diagnostics */
    Optional<DateTime> oldestTimestamp;
    /** @brief Optional. This contains the date and time of the latest logging information to
               include in the diagnostics */
    Optional<DateTime> latestTimestamp;
};

} // namespace ocpp16
} // namespace types
} // namespace ocpp

#endif // OPENOCPP_LOGPARAMETERSTYPE_H
