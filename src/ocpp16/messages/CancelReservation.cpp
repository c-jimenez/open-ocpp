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

#include "CancelReservation.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{
/** @brief Helper to convert a enum class CancelReservationStatus enum to string */
const EnumToStringFromString<CancelReservationStatus> CancelReservationStatusHelper = {{CancelReservationStatus::Accepted, "Accepted"},
                                                                                       {CancelReservationStatus::Rejected, "Rejected"}};

} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool CancelReservationReqConverter::fromJson(const rapidjson::Value& json,
                                             CancelReservationReq&   data,
                                             std::string&            error_code,
                                             std::string&            error_message)
{
    (void)error_code;
    (void)error_message;
    extract(json, "reservationId", data.reservationId);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool CancelReservationReqConverter::toJson(const CancelReservationReq& data, rapidjson::Document& json)
{
    fill(json, "reservationId", data.reservationId);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool CancelReservationConfConverter::fromJson(const rapidjson::Value& json,
                                              CancelReservationConf&  data,
                                              std::string&            error_code,
                                              std::string&            error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = CancelReservationStatusHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool CancelReservationConfConverter::toJson(const CancelReservationConf& data, rapidjson::Document& json)
{
    fill(json, "status", CancelReservationStatusHelper.toString(data.status));
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
