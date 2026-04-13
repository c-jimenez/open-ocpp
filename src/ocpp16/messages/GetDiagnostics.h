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

#ifndef OPENOCPP_GETDIAGNOSTICS_H
#define OPENOCPP_GETDIAGNOSTICS_H

#include "CiStringType.h"
#include "DateTime.h"
#include "IMessageConverter.h"

#include <string>

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the GetDiagnostics messages */
static const std::string GET_DIAGNOSTICS_ACTION = "GetDiagnostics";

/** @brief GetDiagnostics.req message */
struct GetDiagnosticsReq
{
    /** @brief Required. This contains the location (directory) where the diagnostics file shall
               be uploaded to */
    std::string location;
    /** @brief Optional. This specifies how many times Charge Point must try to upload the
               diagnostics before giving up. If this field is not present, it is left to Charge Point
               to decide how many times it wants to retry. */
    ocpp::types::Optional<unsigned int> retries;
    /** @brief Optional. The interval in seconds after which a retry may be attempted. If this
               field is not present, it is left to Charge Point to decide how long to wait between
               attempts. */
    ocpp::types::Optional<unsigned int> retryInterval;
    /** @brief Optional. This contains the date and time of the oldest logging information to
               include in the diagnostics. */
    ocpp::types::Optional<ocpp::types::DateTime> startTime;
    /** @brief Optional. This contains the date and time of the latest logging information to
               include in the diagnostics. */
    ocpp::types::Optional<ocpp::types::DateTime> stopTime;
};

/** @brief GetDiagnostics.conf message */
struct GetDiagnosticsConf
{
    /** @brief Optional. This contains the name of the file with diagnostic information that will
               be uploaded. This field is not present when no diagnostic information is
               available. */
    ocpp::types::Optional<ocpp::types::CiStringType<255>> fileName;
};

// Message converters
MESSAGE_CONVERTERS(GetDiagnostics)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_GETDIAGNOSTICS_H
