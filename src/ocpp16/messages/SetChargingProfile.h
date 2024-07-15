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

#ifndef OPENOCPP_SETCHARGINGPROFILE_H
#define OPENOCPP_SETCHARGINGPROFILE_H

#include "ChargingProfile.h"
#include "Enums.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the SetChargingProfile messages */
static const std::string SET_CHARGING_PROFILE_ACTION = "SetChargingProfile";

/** @brief SetChargingProfile.req message */
struct SetChargingProfileReq
{
    /** @brief Required. The connector to which the charging profile applies. If connectorId = 0,
               the message contains an overall limit for the Charge Point */
    unsigned int connectorId;
    /** @brief Required. The charging profile to be set at the Charge Point */
    ocpp::types::ocpp16::ChargingProfile csChargingProfiles;
};

/** @brief SetChargingProfile.conf message */
struct SetChargingProfileConf
{
    /** @brief Required. Returns whether the Charge Point has been able to process the
               message successfully. This does not guarantee the schedule will be followed to
               the letter. There might be other constraints the Charge Point may need to take
               into account */
    ocpp::types::ocpp16::ChargingProfileStatus status;
};

// Message converters
MESSAGE_CONVERTERS(SetChargingProfile)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_SETCHARGINGPROFILE_H
