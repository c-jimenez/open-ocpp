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

#ifndef OPENOCPP_EXTENDEDTRIGGERMESSAGE_H
#define OPENOCPP_EXTENDEDTRIGGERMESSAGE_H

#include "Enums.h"
#include "IMessageConverter.h"

#include <string>

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the ExtendedTriggerMessage messages */
static const std::string EXTENDED_TRIGGER_MESSAGE_ACTION = "ExtendedTriggerMessage";

/** @brief ExtendedTriggerMessage.req message */
struct ExtendedTriggerMessageReq
{
    /** @brief Required. Type of the message to be triggered */
    ocpp::types::ocpp16::MessageTriggerEnumType requestedMessage;
    /** @brief Optional. Only filled in when request applies to a specific connector */
    ocpp::types::Optional<unsigned int> connectorId;
};

/** @brief ExtendedTriggerMessage.conf message */
struct ExtendedTriggerMessageConf
{
    /** @brief Required. Indicates whether the Charge Point will send the requested
               notification or not */
    ocpp::types::ocpp16::TriggerMessageStatusEnumType status;
};

// Message converters
MESSAGE_CONVERTERS(ExtendedTriggerMessage)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_EXTENDEDTRIGGERMESSAGE_H
