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

#include "RemoteStartTransaction.h"
#include "ChargingProfileConverter.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{
/** @brief Helper to convert a enum class RemoteStartStopStatus enum to string */
const EnumToStringFromString<RemoteStartStopStatus> RemoteStartStopStatusHelper = {{RemoteStartStopStatus::Accepted, "Accepted"},
                                                                                   {RemoteStartStopStatus::Rejected, "Rejected"}};

} // namespace ocpp16
} // namespace types

namespace messages
{
namespace ocpp16
{
/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool RemoteStartTransactionReqConverter::fromJson(const rapidjson::Value&    json,
                                                  RemoteStartTransactionReq& data,
                                                  std::string&               error_code,
                                                  std::string&               error_message)
{
    bool ret = extract(json, "connectorId", data.connectorId, error_message);
    if (ret && (data.connectorId == 0u))
    {
        error_message = "connectorId field must be > 0";
    }
    extract(json, "idTag", data.idTag);
    if (json.HasMember("chargingProfile"))
    {
        ChargingProfileConverter charging_profile_converter;
        ret = ret && charging_profile_converter.fromJson(json["chargingProfile"], data.chargingProfile, error_code, error_message);
    }
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool RemoteStartTransactionReqConverter::toJson(const RemoteStartTransactionReq& data, rapidjson::Document& json)
{
    bool ret = true;
    fill(json, "connectorId", data.connectorId);
    fill(json, "idTag", data.idTag);
    if (data.chargingProfile.isSet())
    {
        ChargingProfileConverter charging_profile_converter;
        charging_profile_converter.setAllocator(allocator);

        rapidjson::Document chargingProfile(rapidjson::kObjectType);
        ret = charging_profile_converter.toJson(data.chargingProfile, chargingProfile);
        json.AddMember(rapidjson::StringRef("chargingProfile"), chargingProfile.Move(), *allocator);
    }

    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool RemoteStartTransactionConfConverter::fromJson(const rapidjson::Value&     json,
                                                   RemoteStartTransactionConf& data,
                                                   std::string&                error_code,
                                                   std::string&                error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = RemoteStartStopStatusHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool RemoteStartTransactionConfConverter::toJson(const RemoteStartTransactionConf& data, rapidjson::Document& json)
{
    fill(json, "status", RemoteStartStopStatusHelper.toString(data.status));
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
