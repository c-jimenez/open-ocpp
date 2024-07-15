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

#ifndef OPENOCPP_STATUSNOTIFICATION_H
#define OPENOCPP_STATUSNOTIFICATION_H

#include "CiStringType.h"
#include "DateTime.h"
#include "Enums.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the StatusNotification messages */
static const std::string STATUS_NOTIFICATION_ACTION = "StatusNotification";

/** @brief StatusNotification.req message */
struct StatusNotificationReq
{
    /** @brief Required. The id of the connector for which the status is reported.
               Id '0' (zero) is used if the status is for the Charge Point main
               controller */
    unsigned int connectorId;
    /** @brief Required. This contains the error code reported by the Charge
               Point */
    ocpp::types::ocpp16::ChargePointErrorCode errorCode;
    /** @brief Optional. Additional free format information related to the error */
    ocpp::types::Optional<ocpp::types::CiStringType<50u>> info;
    /** @brief Required. This contains the current status of the Charge Point */
    ocpp::types::ocpp16::ChargePointStatus status;
    /** @brief Optional. The time for which the status is reported. If absent time
               of receipt of the message will be assumed */
    ocpp::types::Optional<ocpp::types::DateTime> timestamp;
    /** @brief Optional. This identifies the vendor-specific implementation */
    ocpp::types::Optional<ocpp::types::CiStringType<255u>> vendorId;
    /** @brief Optional. This contains the vendor-specific error code */
    ocpp::types::Optional<ocpp::types::CiStringType<50u>> vendorErrorCode;
};

/** @brief StatusNotification.conf message */
struct StatusNotificationConf
{
    // No fields are defined
};

// Message converters
MESSAGE_CONVERTERS(StatusNotification)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_STATUSNOTIFICATION_H
