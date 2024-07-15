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

#ifndef OPENOCPP_SIGNEDUPDATEFIRMWARE_H
#define OPENOCPP_SIGNEDUPDATEFIRMWARE_H

#include "Enums.h"
#include "FirmwareType.h"
#include "IMessageConverter.h"
#include "Optional.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the SignedUpdateFirmware messages */
static const std::string SIGNED_UPDATE_FIRMWARE_ACTION = "SignedUpdateFirmware";

/** @brief SignedUpdateFirmware.req message */
struct SignedUpdateFirmwareReq
{
    /** @brief Optional. This specifies how many times Charge Point must try to download the
               firmware before giving up. If this field is not present, it is left to Charge Point to
               decide how many times it wants to retry. */
    ocpp::types::Optional<unsigned int> retries;

    /** @brief The interval in seconds after which a retry may be attempted.If this field is not present,
               it is left to Charge Point to decide how long to wait between attempts. */
    ocpp::types::Optional<unsigned int> retryInterval;
    /** @brief Required. The Id of this request */
    int requestId;

    /** @brief Required.Specifies the firmware to be updated on the Charge Point */
    ocpp::types::ocpp16::FirmwareType firmware;
};

/** @brief SignedUpdateFirmware.conf message */
struct SignedUpdateFirmwareConf
{
    /** @brief Required. This field indicates whether the Charge Point was able to accept the request */
    ocpp::types::ocpp16::UpdateFirmwareStatusEnumType status;
};

// Message converters
MESSAGE_CONVERTERS(SignedUpdateFirmware)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_SIGNEDUPDATEFIRMWARE_H
