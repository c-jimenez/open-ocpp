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

#ifndef OPENOCPP_IDTOKENINFOTYPECONVERTER_H
#define OPENOCPP_IDTOKENINFOTYPECONVERTER_H

#include "IMessageConverter.h"
#include "IdTokenInfoType.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Converter class for IdTokenInfoType type */
class IdTokenInfoTypeConverter : public IMessageConverter<ocpp::types::ocpp16::IdTokenInfoType>
{
  public:
    /** @copydoc IMessageConverter<ocpp::types::ocpp16::IdTokenInfoType>* IMessageConverter<ocpp::types::ocpp16::IdTokenInfoType>::clone() const */
    IMessageConverter<ocpp::types::ocpp16::IdTokenInfoType>* clone() const override { return new IdTokenInfoTypeConverter(); }

    /** @copydoc bool IMessageConverter<ocpp::types::ocpp16::IdTokenInfoType>::fromJson(const rapidjson::Value&,
     *                                                                    ocpp::types::ocpp16::IdTokenInfoType&,
     *                                                                    std::string&,
     *                                                                    std::string&) */
    bool fromJson(const rapidjson::Value&               json,
                  ocpp::types::ocpp16::IdTokenInfoType& data,
                  std::string&                          error_code,
                  std::string&                          error_message) override;

    /** @copydoc bool IMessageConverter<ocpp::types::ocpp16::IdTokenInfoType>::toJson(const ocpp::types::ocpp16::IdTokenInfoType&,
     *                                                                  rapidjson::Document&) */
    bool toJson(const ocpp::types::ocpp16::IdTokenInfoType& data, rapidjson::Document& json) override;
};

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_IDTOKENINFOTYPECONVERTER_H
