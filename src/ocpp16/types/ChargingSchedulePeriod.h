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

#ifndef OPENOCPP_CHARGINGSCHEDULEPERIOD_H
#define OPENOCPP_CHARGINGSCHEDULEPERIOD_H

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

/** @brief Charging schedule period structure defines a time period in a charging schedule, as used in: ChargingSchedule */
struct ChargingSchedulePeriod
{
    /** @brief Required. Start of the period, in seconds from the start of schedule. The value of
               StartPeriod also defines the stop time of the previous period */
    int startPeriod;
    /** @brief Required. Charging rate limit during the schedule period, in the applicable
               chargingRateUnit, for example in Amperes or Watts. Accepts at most one digit
               fraction (e.g. 8.1). */
    float limit;
    /** @brief Optional. The number of phases that can be used for charging. If a number of
               phases is needed, numberPhases=3 will be assumed unless another number is given */
    Optional<unsigned int> numberPhases;
};

} // namespace ocpp16
} // namespace types
} // namespace ocpp

#endif // OPENOCPP_CHARGINGSCHEDULEPERIOD_H
