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

#ifndef OPENOCPP_SIGNEDFIRMWARESTATUSNOTIFICATION_H
#define OPENOCPP_SIGNEDFIRMWARESTATUSNOTIFICATION_H

#include "Enums.h"
#include "IMessageConverter.h"
#include "Optional.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the SignedFirmwareStatusNotification messages */
static const std::string SIGNED_FIRMWARE_STATUS_NOTIFICATION_ACTION = "SignedFirmwareStatusNotification";

/** @brief SignedFirmwareStatusNotification.req message */
struct SignedFirmwareStatusNotificationReq
{
    /** @brief Required. This contains the progress status of the firmware installation */
    ocpp::types::ocpp16::FirmwareStatusEnumType status;
    /** @brief Optional. The request id that was provided in the SignedUpdateFirmware.req
               that started this firmware update. This field is mandatory, unless the message
               was triggered by a TriggerMessage.req or the ExtendedTriggerMessage.req AND
               there is no firmware update ongoing */
    ocpp::types::Optional<int> requestId;
};

/** @brief SignedFirmwareStatusNotification.conf message */
struct SignedFirmwareStatusNotificationConf
{
    // No fields are defined
};

// Message converters
MESSAGE_CONVERTERS(SignedFirmwareStatusNotification)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_SIGNEDFIRMWARESTATUSNOTIFICATION_H
