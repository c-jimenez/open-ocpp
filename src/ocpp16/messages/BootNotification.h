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

#ifndef OPENOCPP_BOOTNOTIFICATION_H
#define OPENOCPP_BOOTNOTIFICATION_H

#include "CiStringType.h"
#include "DateTime.h"
#include "Enums.h"
#include "IMessageConverter.h"
#include "Optional.h"

#include <string>

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the BootNotification messages */
static const std::string BOOT_NOTIFICATION_ACTION = "BootNotification";

/** @brief BootNotification.req message */
struct BootNotificationReq
{
    /** @brief Optional. This contains a value that identifies the serial number of
               the Charge Box inside the Charge Point. Deprecated, will be
               removed in future version */
    ocpp::types::Optional<ocpp::types::CiStringType<25u>> chargeBoxSerialNumber;
    /** @brief Required. This contains a value that identifies the model of the
               ChargePoint */
    ocpp::types::CiStringType<20u> chargePointModel;
    /** @brief Optional. This contains a value that identifies the serial number of
               the Charge Point */
    ocpp::types::Optional<ocpp::types::CiStringType<25u>> chargePointSerialNumber;
    /** @brief Required. This contains a value that identifies the vendor of the
               ChargePoint */
    ocpp::types::CiStringType<20u> chargePointVendor;
    /** @brief Optional. This contains the firmware version of the Charge Point */
    ocpp::types::Optional<ocpp::types::CiStringType<50u>> firmwareVersion;
    /** @brief Optional. This contains the ICCID of the modem’s SIM card */
    ocpp::types::Optional<ocpp::types::CiStringType<20u>> iccid;
    /** @brief Optional. This contains the IMSI of the modem’s SIM card */
    ocpp::types::Optional<ocpp::types::CiStringType<20u>> imsi;
    /** @brief Optional. This contains the serial number of the main electrical
               meter of the Charge Point */
    ocpp::types::Optional<ocpp::types::CiStringType<25u>> meterSerialNumber;
    /** @brief Optional. This contains the type of the main electrical meter of
               the Charge Point */
    ocpp::types::Optional<ocpp::types::CiStringType<25u>> meterType;
};

/** @brief BootNotification.conf message */
struct BootNotificationConf
{
    /** @brief Required. This contains the Central System’s current time */
    ocpp::types::DateTime currentTime;
    /** @brief Required. When RegistrationStatus is Accepted, this contains the heartbeat
               interval in seconds. If the Central System returns something other than
               Accepted, the value of the interval field indicates the minimum wait time before
               sending a next BootNotification request */
    unsigned int interval;
    /** @brief Required. This contains whether the Charge Point has been registered within the
               System Central */
    ocpp::types::ocpp16::RegistrationStatus status;
};

// Message converters
MESSAGE_CONVERTERS(BootNotification)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_BOOTNOTIFICATION_H
