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

#ifndef OPENOCPP_GETCOMPOSITESCHEDULE_H
#define OPENOCPP_GETCOMPOSITESCHEDULE_H

#include "ChargingSchedule.h"
#include "Enums.h"
#include "IMessageConverter.h"
#include "Optional.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the GetCompositeSchedule messages */
static const std::string GET_COMPOSITE_SCHEDULE_ACTION = "GetCompositeSchedule";

/** @brief GetCompositeSchedule.req message */
struct GetCompositeScheduleReq
{
    /** @brief Required. The ID of the Connector for which the schedule is
               requested. When ConnectorId=0, the Charge Point will calculate
               the expected consumption for the grid connection */
    unsigned int connectorId;
    /** @brief Required. Time in seconds. length of requested schedule */
    unsigned int duration;
    /** @brief Optional. Can be used to force a power or current profile */
    ocpp::types::Optional<ocpp::types::ocpp16::ChargingRateUnitType> chargingRateUnit;
};

/** @brief GetCompositeSchedule.conf message */
struct GetCompositeScheduleConf
{
    /** @brief Required. Status of the request. The Charge Point will indicate if it
               was able to process the request */
    ocpp::types::ocpp16::GetCompositeScheduleStatus status;
    /** @brief Optional. The charging schedule contained in this notification
               applies to a Connector */
    ocpp::types::Optional<unsigned int> connectorId;
    /** @brief Optional. Time. Periods contained in the charging profile are
               relative to this point in time.
               If status is "Rejected", this field may be absent */
    ocpp::types::Optional<ocpp::types::DateTime> scheduleStart;
    /** @brief Optional. Planned Composite Charging Schedule, the energy
               consumption over time. Always relative to ScheduleStart.
               If status is "Rejected", this field may be absent */
    ocpp::types::Optional<ocpp::types::ocpp16::ChargingSchedule> chargingSchedule;
};

// Message converters
MESSAGE_CONVERTERS(GetCompositeSchedule)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_GETCOMPOSITESCHEDULE_H
