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

#ifndef OPENOCPP_ISO15118TRIGGERMESSAGE_H
#define OPENOCPP_ISO15118TRIGGERMESSAGE_H

#include "Enums.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the Iso15118TriggerMessage messages */
static const std::string ISO15118_TRIGGER_MESSAGE_ACTION = "TriggerMessage";

/** @brief Iso15118TriggerMessage.req message */
struct Iso15118TriggerMessageReq
{
    // No fields are defined
};

/** @brief Iso15118TriggerMessage.conf message */
struct Iso15118TriggerMessageConf
{
    /** @brief Required. Indicates whether the Charge Point will send the requested
               notification or not */
    ocpp::types::ocpp16::TriggerMessageStatusEnumType status;
};

// Message converters
MESSAGE_CONVERTERS(Iso15118TriggerMessage)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_ISO15118TRIGGERMESSAGE_H
