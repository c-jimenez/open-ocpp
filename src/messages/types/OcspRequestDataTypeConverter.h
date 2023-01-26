/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPENOCPP_OCSPREQUESTDATATYPECONVERTER_H
#define OPENOCPP_OCSPREQUESTDATATYPECONVERTER_H

#include "IMessageConverter.h"
#include "OcspRequestDataType.h"

namespace ocpp
{
namespace messages
{

/** @brief Converter class for OcspRequestDataType type */
class OcspRequestDataTypeConverter : public IMessageConverter<ocpp::types::OcspRequestDataType>
{
  public:
    /** @copydoc IMessageConverter<ocpp::types::OcspRequestDataType>* IMessageConverter<ocpp::types::OcspRequestDataType>::clone() const */
    IMessageConverter<ocpp::types::OcspRequestDataType>* clone() const override { return new OcspRequestDataTypeConverter(); }

    /** @copydoc bool IMessageConverter<ocpp::types::OcspRequestDataType>::fromJson(const rapidjson::Value&,
    *                                                                               ocpp::types::OcspRequestDataType&,
    *                                                                               std::string&,
    *                                                                               std::string&) */
    bool fromJson(const rapidjson::Value&           json,
                  ocpp::types::OcspRequestDataType& data,
                  std::string&                      error_code,
                  std::string&                      error_message) override;

    /** @copydoc bool IMessageConverter<ocpp::types::OcspRequestDataType>::toJson(const ocpp::types::OcspRequestDataType&,
     *                                                                            rapidjson::Document&) */
    bool toJson(const ocpp::types::OcspRequestDataType& data, rapidjson::Document& json) override;
};

} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_OCSPREQUESTDATATYPECONVERTER_H
