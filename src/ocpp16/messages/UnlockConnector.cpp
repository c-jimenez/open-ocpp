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

#include "UnlockConnector.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief Helper to convert a UnlockStatus enum to string */
const EnumToStringFromString<UnlockStatus> UnlockStatusHelper = {
    {UnlockStatus::Unlocked, "Unlocked"}, {UnlockStatus::UnlockFailed, "UnlockFailed"}, {UnlockStatus::NotSupported, "NotSupported"}};

} // namespace ocpp16
} // namespace types

namespace messages
{
namespace ocpp16
{
/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool UnlockConnectorReqConverter::fromJson(const rapidjson::Value& json,
                                           UnlockConnectorReq&     data,
                                           std::string&            error_code,
                                           std::string&            error_message)
{
    bool ret = extract(json, "connectorId", data.connectorId, error_message);
    ret      = ret && (data.connectorId > 0);
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool UnlockConnectorReqConverter::toJson(const UnlockConnectorReq& data, rapidjson::Document& json)
{
    fill(json, "connectorId", data.connectorId);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool UnlockConnectorConfConverter::fromJson(const rapidjson::Value& json,
                                            UnlockConnectorConf&    data,
                                            std::string&            error_code,
                                            std::string&            error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = UnlockStatusHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool UnlockConnectorConfConverter::toJson(const UnlockConnectorConf& data, rapidjson::Document& json)
{
    fill(json, "status", UnlockStatusHelper.toString(data.status));
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
