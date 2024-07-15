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

#ifndef OPENOCPP_CHARGINGPROFILECONVERTER_H
#define OPENOCPP_CHARGINGPROFILECONVERTER_H

#include "ChargingProfile.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Converter class for ChargingProfile type */
class ChargingProfileConverter : public IMessageConverter<ocpp::types::ocpp16::ChargingProfile>
{
  public:
    /** @copydoc IMessageConverter<ocpp::types::ocpp16::ChargingProfile>* IMessageConverter<ocpp::types::ocpp16::ChargingProfile>::clone() const */
    IMessageConverter<ocpp::types::ocpp16::ChargingProfile>* clone() const override { return new ChargingProfileConverter(); }

    /** @copydoc bool IMessageConverter<ocpp::types::ocpp16::ChargingProfile>::fromJson(const rapidjson::Value&,
     *                                                                    ocpp::types::ocpp16::ChargingProfile&,
     *                                                                    std::string&,
     *                                                                    std::string&) */
    bool fromJson(const rapidjson::Value&               json,
                  ocpp::types::ocpp16::ChargingProfile& data,
                  std::string&                          error_code,
                  std::string&                          error_message) override;

    /** @copydoc bool IMessageConverter<ocpp::types::ocpp16::ChargingProfile>::toJson(const ocpp::types::ocpp16::ChargingProfile&,
     *                                                                  rapidjson::Document&) */
    bool toJson(const ocpp::types::ocpp16::ChargingProfile& data, rapidjson::Document& json) override;
};

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_CHARGINGPROFILECONVERTER_H
