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
    Generated by json2cpp from : urn:OCPP:Cp:2:2020:3:MeterValuesRequest
                                 OCPP 2.0.1 FINAL
*/

#ifndef OPENOCPP_UNITOFMEASURETYPE_H
#define OPENOCPP_UNITOFMEASURETYPE_H

#include "CustomDataType.h"

#include "Optional.h"
#include "CiStringType.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace types
{
namespace ocpp20
{

/** @brief Represents a UnitOfMeasure with a multiplier
 */
struct UnitOfMeasureType
{
    /** @brief  */
    ocpp::types::Optional<CustomDataType> customData;
    /** @brief Unit of the value. Default = "Wh" if the (default) measurand is an "Energy" type.
This field SHALL use a value from the list Standardized Units of Measurements in Part 2 Appendices. 
If an applicable unit is available in that list, otherwise a "custom" unit might be used. */
    ocpp::types::Optional<ocpp::types::CiStringType<20u>> unit;
    /** @brief Multiplier, this value represents the exponent to base 10. I.e. multiplier 3 means 10 raised to the 3rd power. Default is 0. */
    ocpp::types::Optional<int> multiplier;
};

/** @brief Converter class for UnitOfMeasureType type */
class UnitOfMeasureTypeConverter : public ocpp::messages::IMessageConverter<UnitOfMeasureType>
{
  public:
    /** @brief Clone the converter */
    ocpp::messages::IMessageConverter<UnitOfMeasureType>* clone() const override { return new UnitOfMeasureTypeConverter(); }

    /** @brief Convert a UnitOfMeasureType from a JSON representation */
    bool fromJson(const rapidjson::Value&       json,
                  UnitOfMeasureType&                data,
                  std::string&                  error_code,
                  [[maybe_unused]] std::string& error_message) override;

    /** @brief Convert a UnitOfMeasureType to a JSON representation */
    bool toJson(const UnitOfMeasureType& data, rapidjson::Document& json) override;
};

} // namespace ocpp20
} // namespace types
} // namespace ocpp

#endif // OPENOCPP_UNITOFMEASURETYPE_H