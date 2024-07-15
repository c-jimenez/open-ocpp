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

#ifndef OPENOCPP_CLEARCHARGINGPROFILE_H
#define OPENOCPP_CLEARCHARGINGPROFILE_H

#include "Enums.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the ClearChargingProfile messages */
static const std::string CLEAR_CHARGING_PROFILE_ACTION = "ClearChargingProfile";

/** @brief ClearChargingProfile.req message */
struct ClearChargingProfileReq
{
    /** @brief Optional. The ID of the charging profile to clear */
    ocpp::types::Optional<int> id;
    /** @brief Optional. Specifies the ID of the connector for which to clear
               charging profiles. A connectorId of zero (0) specifies the charging
               profile for the overall Charge Point. Absence of this parameter
               means the clearing applies to all charging profiles that match the
               other criteria in the request */
    ocpp::types::Optional<unsigned int> connectorId;
    /** @brief Optional. Specifies to purpose of the charging profiles that will be
               cleared, if they meet the other criteria in the request */
    ocpp::types::Optional<ocpp::types::ocpp16::ChargingProfilePurposeType> chargingProfilePurpose;
    /** @brief Optional. specifies the stackLevel for which charging profiles will
               be cleared, if they meet the other criteria in the request */
    ocpp::types::Optional<unsigned int> stackLevel;
};

/** @brief ClearChargingProfile.conf message */
struct ClearChargingProfileConf
{
    /** @brief Required. Indicates if the Charge Point was able to execute the
               request */
    ocpp::types::ocpp16::ClearChargingProfileStatus status;
};

// Message converters
MESSAGE_CONVERTERS(ClearChargingProfile)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_CLEARCHARGINGPROFILE_H
