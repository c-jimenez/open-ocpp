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

#include "UpdateFirmware.h"
#include "IRpc.h"
#include "Url.h"

using namespace ocpp::types;

namespace ocpp
{
namespace messages
{
namespace ocpp16
{
/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool UpdateFirmwareReqConverter::fromJson(const rapidjson::Value& json,
                                          UpdateFirmwareReq&      data,
                                          std::string&            error_code,
                                          std::string&            error_message)
{
    bool ret;
    extract(json, "location", data.location);
    ocpp::websockets::Url url(data.location);
    ret = url.isValid();
    ret = ret && extract(json, "retries", data.retries, error_message);
    ret = ret && extract(json, "retryInterval", data.retryInterval, error_message);
    ret = ret && extract(json, "retrieveDate", data.retrieveDate, error_message);
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool UpdateFirmwareReqConverter::toJson(const UpdateFirmwareReq& data, rapidjson::Document& json)
{
    fill(json, "location", data.location);
    fill(json, "retries", data.retries);
    fill(json, "retryInterval", data.retryInterval);
    fill(json, "retrieveDate", data.retrieveDate);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool UpdateFirmwareConfConverter::fromJson(const rapidjson::Value& json,
                                           UpdateFirmwareConf&     data,
                                           std::string&            error_code,
                                           std::string&            error_message)
{
    (void)json;
    (void)data;
    (void)error_code;
    (void)error_message;
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool UpdateFirmwareConfConverter::toJson(const UpdateFirmwareConf& data, rapidjson::Document& json)
{
    (void)json;
    (void)data;
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
