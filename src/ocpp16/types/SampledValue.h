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

#ifndef OPENOCPP_SAMPLEDVALUE_H
#define OPENOCPP_SAMPLEDVALUE_H

#include "Enums.h"
#include "Optional.h"

#include <string>

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief Single sampled value in MeterValues. Each value can be accompanied by optional fields */
struct SampledValue
{
    /** @brief Required. Value as a “Raw” (decimal) number or “SignedData”. Field Type is
               “string” to allow for digitally signed data readings. Decimal numeric values are
               also acceptable to allow fractional values for measurands such as Temperature
               and Current */
    std::string value;
    /** @brief Optional. Type of detail value: start, end or sample. Default = “Sample.Periodic” */
    Optional<ReadingContext> context;
    /** @brief Optional. Raw or signed data. Default = “Raw” */
    Optional<ValueFormat> format;
    /** @brief Optional. Type of measurement. Default = “Energy.Active.Import.Register” */
    Optional<Measurand> measurand;
    /** @brief Optional. indicates how the measured value is to be interpreted. For instance
               between L1 and neutral (L1-N) Please note that not all values of phase are
               applicable to all Measurands. When phase is absent, the measured value is
               interpreted as an overall value */
    Optional<Phase> phase;
    /** @brief Optional. Location of measurement. Default=”Outlet” */
    Optional<Location> location;
    /** @brief Optional. Unit of the value. Default = “Wh” if the (default) measurand is an
               “Energy” type */
    Optional<UnitOfMeasure> unit;
};

} // namespace ocpp16
} // namespace types
} // namespace ocpp

#endif // OPENOCPP_SAMPLEDVALUE_H
