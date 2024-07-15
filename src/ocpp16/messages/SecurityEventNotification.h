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

#ifndef OPENOCPP_SECURITYEVENTNOTIFICATION_H
#define OPENOCPP_SECURITYEVENTNOTIFICATION_H

#include "CiStringType.h"
#include "DateTime.h"
#include "IMessageConverter.h"
#include "Optional.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the SecurityEventNotification messages */
static const std::string SECURITY_EVENT_NOTIFICATION_ACTION = "SecurityEventNotification";

/** @brief SecurityEventNotification.req message */
struct SecurityEventNotificationReq
{
    /** @brief Required. Type of the security event (See list of currently known security events) */
    ocpp::types::CiStringType<50u> type;
    /** @brief Required. Date and time at which the event occurred */
    ocpp::types::DateTime timestamp;
    /** @brief Additional information about the occurred security event */
    ocpp::types::Optional<ocpp::types::CiStringType<255u>> techInfo;
};

/** @brief SecurityEventNotification.conf message */
struct SecurityEventNotificationConf
{
    // No fields are defined
};

// Message converters
MESSAGE_CONVERTERS(SecurityEventNotification)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_SECURITYEVENTNOTIFICATION_H
