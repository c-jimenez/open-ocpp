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

#include "AuthorizationDataConverter.h"
#include "Enums.h"
#include "IRpc.h"
#include "IdTagInfoConverter.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @bcopydoc bool IMessageConverter<ocpp::types::ocpp16::AuthorizationData>::fromJson(const rapidjson::Value&,
 *                                                                     ocpp::types::ocpp16::AuthorizationData&,
 *                                                                     std::string&,
 *                                                                     std::string&) */
bool AuthorizationDataConverter::fromJson(const rapidjson::Value&                 json,
                                          ocpp::types::ocpp16::AuthorizationData& data,
                                          std::string&                            error_code,
                                          std::string&                            error_message)
{
    bool ret = true;
    extract(json, "idTag", data.idTag);
    if (json.HasMember("idTagInfo"))
    {
        IdTagInfoConverter id_tag_info_converter;
        ret = id_tag_info_converter.fromJson(json["idTagInfo"], data.idTagInfo, error_code, error_message);
    }
    return ret;
}

/** @copydoc bool IMessageConverter<ocpp::types::ocpp16::AuthorizationData>::toJson(const ocpp::types::ocpp16::AuthorizationData&,
 *                                                                  rapidjson::Document&) */
bool AuthorizationDataConverter::toJson(const ocpp::types::ocpp16::AuthorizationData& data, rapidjson::Document& json)
{
    bool ret = true;
    fill(json, "idTag", data.idTag);
    if (data.idTagInfo.isSet())
    {
        IdTagInfoConverter id_tag_info_converter;
        id_tag_info_converter.setAllocator(allocator);
        rapidjson::Document value(rapidjson::kObjectType);
        ret = id_tag_info_converter.toJson(data.idTagInfo, value);
        json.AddMember(rapidjson::StringRef("idTagInfo"), value.Move(), *allocator);
    }
    return ret;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
