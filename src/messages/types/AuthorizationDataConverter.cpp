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

#include "AuthorizationDataConverter.h"
#include "Enums.h"
#include "IRpc.h"
#include "IdTagInfoConverter.h"

using namespace ocpp::types;

namespace ocpp
{
namespace messages
{

/** @bcopydoc bool IMessageConverter<ocpp::types::AuthorizationData>::fromJson(const rapidjson::Value&,
 *                                                                     ocpp::types::AuthorizationData&,
 *                                                                     std::string&,
 *                                                                     std::string&) */
bool AuthorizationDataConverter::fromJson(const rapidjson::Value&         json,
                                          ocpp::types::AuthorizationData& data,
                                          std::string&                    error_code,
                                          std::string&                    error_message)
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

/** @copydoc bool IMessageConverter<ocpp::types::AuthorizationData>::toJson(const ocpp::types::AuthorizationData&,
 *                                                                  rapidjson::Document&) */
bool AuthorizationDataConverter::toJson(const ocpp::types::AuthorizationData& data, rapidjson::Document& json)
{
    bool ret = true;
    fill(json, "idTag", data.idTag);
    if (data.idTagInfo.isSet())
    {
        IdTagInfoConverter id_tag_info_converter;
        id_tag_info_converter.setAllocator(allocator);
        rapidjson::Document value;
        value.Parse("{}");
        ret = id_tag_info_converter.toJson(data.idTagInfo, value);
        json.AddMember(rapidjson::StringRef("idTagInfo"), value.Move(), *allocator);
    }
    return ret;
}

} // namespace messages
} // namespace ocpp
