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

#ifndef OPENOCPP_UPDATEFIRMWARE_H
#define OPENOCPP_UPDATEFIRMWARE_H

#include "DateTime.h"
#include "IMessageConverter.h"

#include <string>

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the UpdateFirmware messages */
static const std::string UPDATE_FIRMWARE_ACTION = "UpdateFirmware";

/** @brief UpdateFirmware.req message */
struct UpdateFirmwareReq
{
    /** @brief Required. This contains a string containing a URI pointing to a location from
               which to retrieve the firmware */
    std::string location;
    /** @brief Optional. This specifies how many times Charge Point must try to download the
               firmware before giving up. If this field is not present, it is left to Charge Point to
               decide how many times it wants to retry. */
    ocpp::types::Optional<unsigned int> retries;
    /** @brief Required. This contains the date and time after which the Charge Point is
               allowed to retrieve the (new) firmware. */
    ocpp::types::DateTime retrieveDate;
    /** @brief Optional. The interval in seconds after which a retry may be attempted. If this
               field is not present, it is left to Charge Point to decide how long to wait between
               attempts. */
    ocpp::types::Optional<unsigned int> retryInterval;
};

/** @brief UpdateFirmware.conf message */
struct UpdateFirmwareConf
{
    // No fields are defined.
};

// Message converters
MESSAGE_CONVERTERS(UpdateFirmware)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_UPDATEFIRMWARE_H
