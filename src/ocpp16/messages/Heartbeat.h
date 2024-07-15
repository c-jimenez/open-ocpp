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

#ifndef OPENOCPP_HEARTBEAT_H
#define OPENOCPP_HEARTBEAT_H

#include "DateTime.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the Heartbeat messages */
static const std::string HEARTBEAT_ACTION = "Heartbeat";

/** @brief Heartbeat.req message */
struct HeartbeatReq
{
    // No fields are defined
};

/** @brief Heartbeat.conf message */
struct HeartbeatConf
{
    /** @brief Required. This contains the current time of the Central System */
    ocpp::types::DateTime currentTime;
};

// Message converters
MESSAGE_CONVERTERS(Heartbeat)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_HEARTBEAT_H
