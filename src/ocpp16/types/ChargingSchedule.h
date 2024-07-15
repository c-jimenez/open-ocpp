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

#ifndef OPENOCPP_CHARGINGSCHEDULE_H
#define OPENOCPP_CHARGINGSCHEDULE_H

#include "ChargingSchedulePeriod.h"
#include "DateTime.h"
#include "Enums.h"
#include "Optional.h"

#include <vector>

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief Charging schedule structure defines a list of charging periods, as used in: GetCompositeSchedule.conf and
           ChargingProfile */
struct ChargingSchedule
{
    /** @brief Optional. Duration of the charging schedule in seconds. If the
               duration is left empty, the last period will continue indefinitely or
               until end of the transaction in case startSchedule is absent */
    Optional<int> duration;
    /** @brief Optional. Starting point of an absolute schedule. If absent the
               schedule will be relative to start of charging */
    Optional<DateTime> startSchedule;
    /** @brief Required. The unit of measure Limit is expressed in */
    ChargingRateUnitType chargingRateUnit;
    /** @brief Required. List of ChargingSchedulePeriod elements defining
               maximum power or current usage over time. The startSchedule of
               the first ChargingSchedulePeriod SHALL always be 0 */
    std::vector<ChargingSchedulePeriod> chargingSchedulePeriod;
    /** @brief Optional. Minimum charging rate supported by the electric vehicle.
               The unit of measure is defined by the chargingRateUnit. This parameter
               is intended to be used by a local smart charging algorithm to optimize
               the power allocation for in the case a charging process is inefficient
               at lower charging rates. Accepts at most one digit fraction (e.g. 8.1) */
    Optional<float> minChargingRate;
};

} // namespace ocpp16
} // namespace types
} // namespace ocpp

#endif // OPENOCPP_CHARGINGSCHEDULE_H
