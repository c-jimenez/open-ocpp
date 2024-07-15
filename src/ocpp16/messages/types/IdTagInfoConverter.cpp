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

#include "IdTagInfoConverter.h"
#include "Enums.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @bcopydoc bool IMessageConverter<ocpp::types::ocpp16::IdTagInfo>::fromJson(const rapidjson::Value&,
 *                                                                     ocpp::types::ocpp16::IdTagInfo&,
 *                                                                     std::string&,
 *                                                                     std::string&) */
bool IdTagInfoConverter::fromJson(const rapidjson::Value&         json,
                                  ocpp::types::ocpp16::IdTagInfo& data,
                                  std::string&                    error_code,
                                  std::string&                    error_message)
{
    bool ret;
    ret = extract(json, "expiryDate", data.expiryDate, error_message);
    extract(json, "parentIdTag", data.parentIdTag);
    data.status = AuthorizationStatusHelper.fromString(json["status"].GetString());
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<ocpp::types::ocpp16::IdTagInfo>::toJson(const ocpp::types::ocpp16::IdTagInfo&,
 *                                                                  rapidjson::Document&) */
bool IdTagInfoConverter::toJson(const ocpp::types::ocpp16::IdTagInfo& data, rapidjson::Document& json)
{
    fill(json, "expiryDate", data.expiryDate);
    fill(json, "parentIdTag", data.parentIdTag);
    fill(json, "status", AuthorizationStatusHelper.toString(data.status));
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
