/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SIGNEDFIRMWAREUPDATE_H
#define SIGNEDFIRMWAREUPDATE_H

#include "Enums.h"
#include "FirmwareType.h"
#include "IMessageConverter.h"
#include "Optional.h"

namespace ocpp
{
namespace messages
{

/** @brief Action corresponding to the SignedFirmwareUpdate messages */
static const std::string SIGNED_FIRMWARE_UPDATE_ACTION = "SignedFirmwareUpdate";

/** @brief SignedFirmwareUpdate.req message */
struct SignedFirmwareUpdateReq
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
    ocpp::types::FirmwareType firmware;
};

/** @brief SignedFirmwareUpdate.conf message */
struct SignedFirmwareUpdateConf
{
    /** @brief Required. This field indicates whether the Charge Point was able to accept the request */
    ocpp::types::UpdateFirmwareStatusEnumType status;
};

// Message converters
MESSAGE_CONVERTERS(SignedFirmwareUpdate)

} // namespace messages
} // namespace ocpp

#endif // SIGNEDFIRMWAREUPDATE_H
