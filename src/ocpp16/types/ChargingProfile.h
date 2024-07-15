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

#ifndef OPENOCPP_CHARGINGPROFILE_H
#define OPENOCPP_CHARGINGPROFILE_H

#include "ChargingSchedule.h"
#include "DateTime.h"
#include "Enums.h"
#include "Optional.h"

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief A ChargingProfile consists of a ChargingSchedule, describing the amount of power or current that can be
           delivered per time interval */
struct ChargingProfile
{
    /** @brief Required. Unique identifier for this profile */
    int chargingProfileId;
    /** @brief Optional. Only valid if ChargingProfilePurpose is set to TxProfile,
               the transactionId MAY be used to match the profile to a specific transaction. */
    Optional<int> transactionId;
    /** @brief Required. Value determining level in hierarchy stack of profiles.
               Higher values have precedence over lower values. Lowest level is 0 */
    unsigned int stackLevel;
    /** @brief Required. Defines the purpose of the schedule transferred by this
               message */
    ChargingProfilePurposeType chargingProfilePurpose;
    /** @brief Required. Indicates the kind of schedule */
    ChargingProfileKindType chargingProfileKind;
    /** @brief Optional. Indicates the start point of a recurrence */
    Optional<RecurrencyKindType> recurrencyKind;
    /** @brief Optional. Point in time at which the profile starts to be valid. If
               absent, the profile is valid as soon as it is received by the Charge Point */
    Optional<DateTime> validFrom;
    /** @brief Optional. Point in time at which the profile stops to be valid. If
               absent, the profile is valid until it is replaced by another profile */
    Optional<DateTime> validTo;
    /** @brief Required. Contains limits for the available power or current overtime */
    ChargingSchedule chargingSchedule;
};

} // namespace ocpp16
} // namespace types
} // namespace ocpp

#endif // OPENOCPP_CHARGINGPROFILE_H
