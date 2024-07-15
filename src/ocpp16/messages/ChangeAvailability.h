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

#ifndef OPENOCPP_CHANGEAVAILABILITY_H
#define OPENOCPP_CHANGEAVAILABILITY_H

#include "Enums.h"
#include "IMessageConverter.h"

#include <string>

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the ChangeAvailability messages */
static const std::string CHANGE_AVAILABILITY_ACTION = "ChangeAvailability";

/** @brief ChangeAvailability.req message */
struct ChangeAvailabilityReq
{
    /** @brief Required. The id of the connector for which availability needs to change. Id '0'
               (zero) is used if the availability of the Charge Point and all its connectors needs
               to change */
    unsigned int connectorId;
    /** @brief Required. This contains the type of availability change that the Charge Point
               should perform. */
    ocpp::types::ocpp16::AvailabilityType type;
};

/** @brief ChangeAvailability.conf message */
struct ChangeAvailabilityConf
{
    /** @brief Required. This indicates whether the Charge Point is able to perform the
               availability change */
    ocpp::types::ocpp16::AvailabilityStatus status;
};

// Message converters
MESSAGE_CONVERTERS(ChangeAvailability)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_CHANGEAVAILABILITY_H
