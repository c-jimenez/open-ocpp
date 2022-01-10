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

#include "IdTagInfoConverter.h"
#include "Enums.h"
#include "IRpcClient.h"

using namespace ocpp::types;

namespace ocpp
{
namespace messages
{

/** @bcopydoc bool IMessageConverter<ocpp::types::IdTagInfo>::fromJson(const rapidjson::Value&,
 *                                                                     ocpp::types::IdTagInfo&,
 *                                                                     const char*&,
 *                                                                     std::string&) */
bool IdTagInfoConverter::fromJson(const rapidjson::Value& json,
                                  ocpp::types::IdTagInfo& data,
                                  const char*&            error_code,
                                  std::string&            error_message)
{
    bool ret;
    ret = extract(json, "expiryDate", data.expiryDate, error_message);
    extract(json, "parentIdTag", data.parentIdTag);
    data.status = AuthorizationStatusHelper.fromString(json["status"].GetString());
    if (!ret)
    {
        error_code = ocpp::rpc::IRpcClient::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<ocpp::types::IdTagInfo>::toJson(const ocpp::types::IdTagInfo&,
 *                                                                  rapidjson::Document&) */
bool IdTagInfoConverter::toJson(const ocpp::types::IdTagInfo& data, rapidjson::Document& json)
{
    fill(json, "expiryDate", data.expiryDate);
    fill(json, "parentIdTag", data.parentIdTag);
    fill(json, "status", AuthorizationStatusHelper.toString(data.status));
    return true;
}

} // namespace messages
} // namespace ocpp
