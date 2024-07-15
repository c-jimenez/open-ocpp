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

#ifndef OPENOCPP_LOGSTATUSNOTIFICATION_H
#define OPENOCPP_LOGSTATUSNOTIFICATION_H

#include "Enums.h"
#include "IMessageConverter.h"
#include "Optional.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the LogStatusNotification messages */
static const std::string LOG_STATUS_NOTIFICATION_ACTION = "LogStatusNotification";

/** @brief LogStatusNotification.req message */
struct LogStatusNotificationReq
{
    /** @brief Required. This contains the status of the log upload */
    ocpp::types::ocpp16::UploadLogStatusEnumType status;
    /** @brief Optional. The request id that was provided in the GetLog.req that started this log
               upload */
    ocpp::types::Optional<int> requestId;
};

/** @brief LogStatusNotification.conf message */
struct LogStatusNotificationConf
{
    // No fields are defined
};

// Message converters
MESSAGE_CONVERTERS(LogStatusNotification)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_LOGSTATUSNOTIFICATION_H
