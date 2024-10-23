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

#include "ReserveNow.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{
/** @brief Helper to convert a enum class ReservationStatus enum to string */
const EnumToStringFromString<ReservationStatus> ReservationStatusHelper = {{ReservationStatus::Accepted, "Accepted"},
                                                                           {ReservationStatus::Faulted, "Faulted"},
                                                                           {ReservationStatus::Occupied, "Occupied"},
                                                                           {ReservationStatus::Rejected, "Rejected"},
                                                                           {ReservationStatus::Unavailable, "Unavailable"}};

} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool ReserveNowReqConverter::fromJson(const rapidjson::Value& json,
                                      ReserveNowReq&          data,
                                      std::string&            error_code,
                                      std::string&            error_message)
{
    bool ret;
    ret = extract(json, "connectorId", data.connectorId, error_message);
    ret = ret && extract(json, "expiryDate", data.expiryDate, error_message);
    extract(json, "idTag", data.idTag);
    extract(json, "parentIdTag", data.parentIdTag);
    extract(json, "reservationId", data.reservationId);
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool ReserveNowReqConverter::toJson(const ReserveNowReq& data, rapidjson::Document& json)
{
    fill(json, "connectorId", data.connectorId);
    fill(json, "expiryDate", data.expiryDate);
    fill(json, "idTag", data.idTag);
    fill(json, "parentIdTag", data.parentIdTag);
    fill(json, "reservationId", data.reservationId);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool ReserveNowConfConverter::fromJson(const rapidjson::Value& json,
                                       ReserveNowConf&         data,
                                       std::string&            error_code,
                                       std::string&            error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = ReservationStatusHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool ReserveNowConfConverter::toJson(const ReserveNowConf& data, rapidjson::Document& json)
{
    fill(json, "status", ReservationStatusHelper.toString(data.status));
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
