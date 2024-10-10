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

#ifndef OPENOCPP_AUTHORIZATIONDATACONVERTER_H
#define OPENOCPP_AUTHORIZATIONDATACONVERTER_H

#include "AuthorizationData.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{

/** @brief Converter class for AuthorizationData type */
class AuthorizationDataConverter : public IMessageConverter<ocpp::types::AuthorizationData>
{
  public:
    /** @copydoc IMessageConverter<ocpp::types::AuthorizationData>* IMessageConverter<ocpp::types::AuthorizationData>::clone() const */
    IMessageConverter<ocpp::types::AuthorizationData>* clone() const override { return new AuthorizationDataConverter(); }

    /** @copydoc bool IMessageConverter<ocpp::types::AuthorizationData>::fromJson(const rapidjson::Value&,
     *                                                                    ocpp::types::AuthorizationData&,
     *                                                                    std::string&,
     *                                                                    std::string&) */
    bool fromJson(const rapidjson::Value&         json,
                  ocpp::types::AuthorizationData& data,
                  std::string&                    error_code,
                  std::string&                    error_message) override;

    /** @copydoc bool IMessageConverter<ocpp::types::AuthorizationData>::toJson(const ocpp::types::AuthorizationData&,
     *                                                                  rapidjson::Document&) */
    bool toJson(const ocpp::types::AuthorizationData& data, rapidjson::Document& json) override;
};

} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_AUTHORIZATIONDATACONVERTER_H
