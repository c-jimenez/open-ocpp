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

/*
    Generated by json2cpp from : urn:OCPP:Cp:2:2020:3:RequestStartTransactionRequest
                                 OCPP 2.0.1 FINAL
*/

#ifndef OPENOCPP_CHARGINGSCHEDULEPERIODTYPE_H
#define OPENOCPP_CHARGINGSCHEDULEPERIODTYPE_H

#include "CustomDataType.h"

#include "Optional.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace types
{
namespace ocpp20
{

/** @brief Charging_ Schedule_ Period
urn:x-oca:ocpp:uid:2:233257
Charging schedule period structure defines a time period in a charging schedule.
 */
struct ChargingSchedulePeriodType
{
    /** @brief  */
    ocpp::types::Optional<CustomDataType> customData;
    /** @brief Charging_ Schedule_ Period. Start_ Period. Elapsed_ Time
urn:x-oca:ocpp:uid:1:569240
Start of the period, in seconds from the start of schedule. The value of StartPeriod also defines the stop time of the previous period. */
    int startPeriod;
    /** @brief Charging_ Schedule_ Period. Limit. Measure
urn:x-oca:ocpp:uid:1:569241
Charging rate limit during the schedule period, in the applicable chargingRateUnit, for example in Amperes (A) or Watts (W). Accepts at most one digit fraction (e.g. 8.1). */
    float limit;
    /** @brief Charging_ Schedule_ Period. Number_ Phases. Counter
urn:x-oca:ocpp:uid:1:569242
The number of phases that can be used for charging. If a number of phases is needed, numberPhases=3 will be assumed unless another number is given. */
    ocpp::types::Optional<int> numberPhases;
    /** @brief Values: 1..3, Used if numberPhases=1 and if the EVSE is capable of switching the phase connected to the EV, i.e. ACPhaseSwitchingSupported is defined and true. It’s not allowed unless both conditions above are true. If both conditions are true, and phaseToUse is omitted, the Charging Station / EVSE will make the selection on its own. */
    ocpp::types::Optional<int> phaseToUse;
};

/** @brief Converter class for ChargingSchedulePeriodType type */
class ChargingSchedulePeriodTypeConverter : public ocpp::messages::IMessageConverter<ChargingSchedulePeriodType>
{
  public:
    /** @brief Clone the converter */
    ocpp::messages::IMessageConverter<ChargingSchedulePeriodType>* clone() const override { return new ChargingSchedulePeriodTypeConverter(); }

    /** @brief Convert a ChargingSchedulePeriodType from a JSON representation */
    bool fromJson(const rapidjson::Value&       json,
                  ChargingSchedulePeriodType&                data,
                  std::string&                  error_code,
                  [[maybe_unused]] std::string& error_message) override;

    /** @brief Convert a ChargingSchedulePeriodType to a JSON representation */
    bool toJson(const ChargingSchedulePeriodType& data, rapidjson::Document& json) override;
};

} // namespace ocpp20
} // namespace types
} // namespace ocpp

#endif // OPENOCPP_CHARGINGSCHEDULEPERIODTYPE_H