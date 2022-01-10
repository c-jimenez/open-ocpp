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

#ifndef CHARGINGSCHEDULECONVERTER_H
#define CHARGINGSCHEDULECONVERTER_H

#include "ChargingSchedule.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{

/** @brief Converter class for ChargingSchedule type */
class ChargingScheduleConverter : public IMessageConverter<ocpp::types::ChargingSchedule>
{
  public:
    /** @copydoc bool IMessageConverter<ocpp::types::ChargingSchedule>::fromJson(const rapidjson::Value&,
     *                                                                    ocpp::types::ChargingSchedule&,
     *                                                                    const char*&,
     *                                                                    std::string&) */
    bool fromJson(const rapidjson::Value&        json,
                  ocpp::types::ChargingSchedule& data,
                  const char*&                   error_code,
                  std::string&                   error_message) override;

    /** @copydoc bool IMessageConverter<ocpp::types::ChargingSchedule>::toJson(const ocpp::types::ChargingSchedule&,
     *                                                                  rapidjson::Document&) */
    bool toJson(const ocpp::types::ChargingSchedule& data, rapidjson::Document& json) override;
};

} // namespace messages
} // namespace ocpp

#endif // CHARGINGSCHEDULECONVERTER_H
