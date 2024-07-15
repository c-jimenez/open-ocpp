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

#ifndef OPENOCPP_METERVALUES_H
#define OPENOCPP_METERVALUES_H

#include "IMessageConverter.h"
#include "MeterValue.h"
#include "Optional.h"

#include <string>
#include <vector>

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the MeterValues messages */
static const std::string METER_VALUES_ACTION = "MeterValues";

/** @brief MeterValues.req message */
struct MeterValuesReq
{
    /** @brief Required. This contains a number (>0) designating a connector of the Charge
               Point.‘0’ (zero) is used to designate the main powermeter */
    unsigned int connectorId;
    /** @brief Optional. The transaction to which these meter samples are related */
    ocpp::types::Optional<int> transactionId;
    /** @brief Required. The sampled meter values with timestamps */
    std::vector<ocpp::types::ocpp16::MeterValue> meterValue;
};

/** @brief MeterValues.conf message */
struct MeterValuesConf
{
    // No fields are defined
};

// Message converters
MESSAGE_CONVERTERS(MeterValues)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_METERVALUES_H
