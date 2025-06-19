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

#include "SecurityEventNotification.h"
#include "IRpc.h"

using namespace ocpp::types;

namespace ocpp
{
namespace messages
{
namespace ocpp16
{
/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool SecurityEventNotificationReqConverter::fromJson(const rapidjson::Value&       json,
                                                     SecurityEventNotificationReq& data,
                                                     std::string&                  error_code,
                                                     std::string&                  error_message)
{
    extract(json, "type", data.type);
    bool ret = extract(json, "timestamp", data.timestamp, error_message);
    extract(json, "techInfo", data.techInfo);
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool SecurityEventNotificationReqConverter::toJson(const SecurityEventNotificationReq& data, rapidjson::Document& json)
{
    fill(json, "type", data.type);
    fill(json, "timestamp", data.timestamp);
    fill(json, "techInfo", data.techInfo);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool SecurityEventNotificationConfConverter::fromJson(const rapidjson::Value&        json,
                                                      SecurityEventNotificationConf& data,
                                                      std::string&                   error_code,
                                                      std::string&                   error_message)
{
    (void)json;
    (void)data;
    (void)error_code;
    (void)error_message;
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool SecurityEventNotificationConfConverter::toJson(const SecurityEventNotificationConf& data, rapidjson::Document& json)
{
    (void)json;
    (void)data;
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
