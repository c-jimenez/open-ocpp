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
    Generated by json2cpp from : urn:OCPP:Cp:2:2020:3:NotifyReportRequest
                                 OCPP 2.0.1 FINAL
*/

#ifndef OPENOCPP_VARIABLECHARACTERISTICSTYPE_H
#define OPENOCPP_VARIABLECHARACTERISTICSTYPE_H

#include "CustomDataType.h"
#include "DataEnumType.h"

#include "Optional.h"
#include "CiStringType.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace types
{
namespace ocpp20
{

/** @brief Fixed read-only parameters of a variable.
 */
struct VariableCharacteristicsType
{
    /** @brief  */
    ocpp::types::Optional<CustomDataType> customData;
    /** @brief Unit of the variable. When the transmitted value has a unit, this field SHALL be included. */
    ocpp::types::Optional<ocpp::types::CiStringType<16u>> unit;
    /** @brief  */
    DataEnumType dataType;
    /** @brief Minimum possible value of this variable. */
    ocpp::types::Optional<float> minLimit;
    /** @brief Maximum possible value of this variable. When the datatype of this Variable is String, OptionList, SequenceList or MemberList, this field defines the maximum length of the (CSV) string. */
    ocpp::types::Optional<float> maxLimit;
    /** @brief Allowed values when variable is Option/Member/SequenceList. 

* OptionList: The (Actual) Variable value must be a single value from the reported (CSV) enumeration list.

* MemberList: The (Actual) Variable value  may be an (unordered) (sub-)set of the reported (CSV) valid values list.

* SequenceList: The (Actual) Variable value  may be an ordered (priority, etc)  (sub-)set of the reported (CSV) valid values.

This is a comma separated list.

The Configuration Variable &lt;&lt;configkey-configuration-value-size,ConfigurationValueSize&gt;&gt; can be used to limit SetVariableData.attributeValue and VariableCharacteristics.valueList. The max size of these values will always remain equal. */
    ocpp::types::Optional<ocpp::types::CiStringType<1000u>> valuesList;
    /** @brief Flag indicating if this variable supports monitoring. */
    bool supportsMonitoring;
};

/** @brief Converter class for VariableCharacteristicsType type */
class VariableCharacteristicsTypeConverter : public ocpp::messages::IMessageConverter<VariableCharacteristicsType>
{
  public:
    /** @brief Clone the converter */
    ocpp::messages::IMessageConverter<VariableCharacteristicsType>* clone() const override { return new VariableCharacteristicsTypeConverter(); }

    /** @brief Convert a VariableCharacteristicsType from a JSON representation */
    bool fromJson(const rapidjson::Value&       json,
                  VariableCharacteristicsType&                data,
                  std::string&                  error_code,
                  [[maybe_unused]] std::string& error_message) override;

    /** @brief Convert a VariableCharacteristicsType to a JSON representation */
    bool toJson(const VariableCharacteristicsType& data, rapidjson::Document& json) override;
};

} // namespace ocpp20
} // namespace types
} // namespace ocpp

#endif // OPENOCPP_VARIABLECHARACTERISTICSTYPE_H