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

#include "GetLog.h"
#include "IRpc.h"
#include "Url.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief Helper to convert a LogEnumType enum to string */
const EnumToStringFromString<LogEnumType> LogEnumTypeHelper = {{LogEnumType::DiagnosticsLog, "DiagnosticsLog"},
                                                               {LogEnumType::SecurityLog, "SecurityLog"}};
/** @brief Helper to convert a LogStatusEnumType enum to string */
const EnumToStringFromString<LogStatusEnumType> LogStatusEnumTypeHelper = {{LogStatusEnumType::Accepted, "Accepted"},
                                                                           {LogStatusEnumType::Rejected, "Rejected"},
                                                                           {LogStatusEnumType::AcceptedCanceled, "AcceptedCanceled"}};

} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool GetLogReqConverter::fromJson(const rapidjson::Value& json, GetLogReq& data, std::string& error_code, std::string& error_message)
{
    bool ret;
    data.logType = LogEnumTypeHelper.fromString(json["logType"].GetString());
    extract(json, "requestId", data.requestId);
    ret = extract(json, "retries", data.retries, error_message);
    ret = ret && extract(json, "retryInterval", data.retryInterval, error_message);

    const rapidjson::Value& log = json["log"];
    extract(log, "remoteLocation", data.log.remoteLocation);
    ocpp::websockets::Url url(data.log.remoteLocation);
    ret = ret && url.isValid();
    ret = ret && extract(log, "oldestTimestamp", data.log.oldestTimestamp, error_message);
    ret = ret && extract(log, "latestTimestamp", data.log.latestTimestamp, error_message);
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool GetLogReqConverter::toJson(const GetLogReq& data, rapidjson::Document& json)
{
    fill(json, "logType", LogEnumTypeHelper.toString(data.logType));
    fill(json, "requestId", data.requestId);
    fill(json, "retries", data.retries);
    fill(json, "retryInterval", data.retryInterval);

    rapidjson::Document log(rapidjson::kObjectType);
    fill(log, "remoteLocation", data.log.remoteLocation);
    fill(log, "oldestTimestamp", data.log.oldestTimestamp);
    fill(log, "latestTimestamp", data.log.latestTimestamp);
    json.AddMember(rapidjson::StringRef("log"), log.Move(), *allocator);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool GetLogConfConverter::fromJson(const rapidjson::Value& json, GetLogConf& data, std::string& error_code, std::string& error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = LogStatusEnumTypeHelper.fromString(json["status"].GetString());
    extract(json, "filename", data.fileName);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool GetLogConfConverter::toJson(const GetLogConf& data, rapidjson::Document& json)
{
    fill(json, "status", LogStatusEnumTypeHelper.toString(data.status));
    fill(json, "filename", data.fileName);
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
