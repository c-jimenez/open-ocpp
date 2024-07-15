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

#ifndef OPENOCPP_METERVALUE_H
#define OPENOCPP_METERVALUE_H

#include "DateTime.h"
#include "Optional.h"
#include "SampledValue.h"

#include <vector>

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief Collection of one or more sampled values in MeterValues.req and StopTransaction.req. All sampled values in a
           MeterValue are sampled at the same point in time */
struct MeterValue
{
    /** @brief Timestamp for measured value(s) */
    DateTime timestamp;
    /** @brief Required. One or more measured values */
    std::vector<SampledValue> sampledValue;
};

} // namespace ocpp16
} // namespace types
} // namespace ocpp

#endif // OPENOCPP_METERVALUE_H
