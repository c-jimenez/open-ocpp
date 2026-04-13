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

#include "Iso15118TriggerMessage.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace messages
{
namespace ocpp16
{
/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool Iso15118TriggerMessageReqConverter::fromJson(const rapidjson::Value&    json,
                                                  Iso15118TriggerMessageReq& data,
                                                  std::string&               error_code,
                                                  std::string&               error_message)
{
    (void)json;
    (void)data;
    (void)error_code;
    (void)error_message;
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool Iso15118TriggerMessageReqConverter::toJson(const Iso15118TriggerMessageReq& data, rapidjson::Document& json)
{
    (void)json;
    (void)data;
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool Iso15118TriggerMessageConfConverter::fromJson(const rapidjson::Value&     json,
                                                   Iso15118TriggerMessageConf& data,
                                                   std::string&                error_code,
                                                   std::string&                error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = TriggerMessageStatusEnumTypeHelper.fromString(json["status"].GetString());
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool Iso15118TriggerMessageConfConverter::toJson(const Iso15118TriggerMessageConf& data, rapidjson::Document& json)
{
    fill(json, "status", TriggerMessageStatusEnumTypeHelper.toString(data.status));
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
