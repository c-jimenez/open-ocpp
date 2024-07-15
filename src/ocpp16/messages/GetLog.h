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

#ifndef OPENOCPP_GETLOG_H
#define OPENOCPP_GETLOG_H

#include "CiStringType.h"
#include "Enums.h"
#include "IMessageConverter.h"
#include "LogParametersType.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the GetLog messages */
static const std::string GET_LOG_ACTION = "GetLog";

/** @brief GetLog.req message */
struct GetLogReq
{
    /** @brief Required. This contains the type of log file that the Charge Point should send */
    ocpp::types::ocpp16::LogEnumType logType;
    /** @brief Required. The Id of this request */
    int requestId;
    /** @brief Optional. This specifies how many times the Charge Point must try to upload the
               log before giving up. If this field is not present, it is left to Charge Point to decide
               how many times it wants to retry. */
    ocpp::types::Optional<unsigned int> retries;
    /** @brief Optional. The interval in seconds after which a retry may be attempted. If this
               field is not present, it is left to Charge Point to decide how long to wait between
               attempts. */
    ocpp::types::Optional<unsigned int> retryInterval;
    /** @brief Required. This field specifies the requested log and the location to which the log
               should be sent */
    ocpp::types::ocpp16::LogParametersType log;
};

/** @brief GetLog.conf message */
struct GetLogConf
{
    /** @brief Required. This field indicates whether the Charge Point was able to accept the
               request */
    ocpp::types::ocpp16::LogStatusEnumType status;
    /** @brief Optional. This contains the name of the log file that will be uploaded. This field is
               not present when no logging information is available. */
    ocpp::types::CiStringType<255> fileName;
};

// Message converters
MESSAGE_CONVERTERS(GetLog)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_GETLOG_H
