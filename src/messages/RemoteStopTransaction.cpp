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

#include "RemoteStopTransaction.h"
#include "IRpcClient.h"

using namespace ocpp::types;

namespace ocpp
{
namespace messages
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, const char*&, std::string&) */
bool RemoteStopTransactionReqConverter::fromJson(const rapidjson::Value&   json,
                                                 RemoteStopTransactionReq& data,
                                                 const char*&              error_code,
                                                 std::string&              error_message)
{
    (void)error_code;
    (void)error_message;
    extract(json, "transactionId", data.transactionId);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, const char*&, std::string&) */
bool RemoteStopTransactionReqConverter::toJson(const RemoteStopTransactionReq& data, rapidjson::Document& json)
{
    fill(json, "transactionId", data.transactionId);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, const char*&, std::string&) */
bool RemoteStopTransactionConfConverter::fromJson(const rapidjson::Value&    json,
                                                  RemoteStopTransactionConf& data,
                                                  const char*&               error_code,
                                                  std::string&               error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = RemoteStartStopStatusHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, const char*&, std::string&) */
bool RemoteStopTransactionConfConverter::toJson(const RemoteStopTransactionConf& data, rapidjson::Document& json)
{
    fill(json, "status", RemoteStartStopStatusHelper.toString(data.status));
    return true;
}

} // namespace messages
} // namespace ocpp
