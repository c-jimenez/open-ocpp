/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License version 2.1
as published by the Free Software Foundation.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#include "IdTokenInfoTypeConverter.h"
#include "Enums.h"
#include "IRpc.h"

using namespace ocpp::types;

namespace ocpp
{
namespace messages
{

/** @bcopydoc bool IMessageConverter<ocpp::types::IdTokenInfoType>::fromJson(const rapidjson::Value&,
 *                                                                     ocpp::types::IdTokenInfoType&,
 *                                                                     std::string&,
 *                                                                     std::string&) */
bool IdTokenInfoTypeConverter::fromJson(const rapidjson::Value&       json,
                                        ocpp::types::IdTokenInfoType& data,
                                        std::string&                  error_code,
                                        std::string&                  error_message)
{
    data.status = AuthorizationStatusHelper.fromString(json["status"].GetString());
    bool ret    = extract(json, "cacheExpiryDateTime", data.cacheExpiryDateTime, error_message);
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<ocpp::types::IdTokenInfoType>::toJson(const ocpp::types::IdTokenInfoType&,
 *                                                                  rapidjson::Document&) */
bool IdTokenInfoTypeConverter::toJson(const ocpp::types::IdTokenInfoType& data, rapidjson::Document& json)
{
    fill(json, "status", AuthorizationStatusHelper.toString(data.status));
    if (data.cacheExpiryDateTime.isSet())
    {
        fill(json, "cacheExpiryDateTime", data.cacheExpiryDateTime);
    }
    return true;
}

} // namespace messages
} // namespace ocpp
