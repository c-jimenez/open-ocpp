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

/*
    Generated by json2cpp from : urn:OCPP:Cp:2:2020:3:PublishFirmwareStatusNotificationRequest
                                 OCPP 2.0.1 FINAL
*/

#ifndef OPENOCPP_PUBLISHFIRMWARESTATUSNOTIFICATION_H
#define OPENOCPP_PUBLISHFIRMWARESTATUSNOTIFICATION_H

#include "IMessageConverter.h"

#include "CustomDataType.h"
#include "PublishFirmwareStatusEnumType.h"

namespace ocpp
{
namespace messages
{
namespace ocpp20
{

/** @brief Action corresponding to the PublishFirmwareStatusNotification messages */
static const std::string PUBLISHFIRMWARESTATUSNOTIFICATION_ACTION = "PublishFirmwareStatusNotification";

/** @brief PublishFirmwareStatusNotificationReq message */
struct PublishFirmwareStatusNotificationReq
{
    /** @brief  */
    ocpp::types::Optional<ocpp::types::ocpp20::CustomDataType> customData;
    /** @brief  */
    ocpp::types::ocpp20::PublishFirmwareStatusEnumType status;
    /** @brief Required if status is Published. Can be multiple URI’s, if the Local Controller supports e.g. HTTP, HTTPS, and FTP. */
    std::vector<std::string> location;
    /** @brief The request id that was
provided in the
PublishFirmwareRequest which
triggered this action. */
    ocpp::types::Optional<int> requestId;
};

/** @brief PublishFirmwareStatusNotificationConf message */
struct PublishFirmwareStatusNotificationConf
{
    /** @brief  */
    ocpp::types::Optional<ocpp::types::ocpp20::CustomDataType> customData;
};

// Message converters
MESSAGE_CONVERTERS(PublishFirmwareStatusNotification)

} // namespace ocpp20
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_PUBLISHFIRMWARESTATUSNOTIFICATION_H