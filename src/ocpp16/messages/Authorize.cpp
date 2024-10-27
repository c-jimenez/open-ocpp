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

#include "Authorize.h"
#include "IRpc.h"
#include "IdTagInfoConverter.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{
/** @brief Helper to convert a AuthorizationStatus enum to string */
const EnumToStringFromString<AuthorizationStatus> AuthorizationStatusHelper = {{AuthorizationStatus::Accepted, "Accepted"},
                                                                               {AuthorizationStatus::Blocked, "Blocked"},
                                                                               {AuthorizationStatus::ConcurrentTx, "ConcurrentTx"},
                                                                               {AuthorizationStatus::Expired, "Expired"},
                                                                               {AuthorizationStatus::Invalid, "Invalid"}};
} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool AuthorizeReqConverter::fromJson(const rapidjson::Value& json, AuthorizeReq& data, std::string& error_code, std::string& error_message)
{
    (void)error_code;
    (void)error_message;
    extract(json, "idTag", data.idTag);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool AuthorizeReqConverter::toJson(const AuthorizeReq& data, rapidjson::Document& json)
{
    fill(json, "idTag", data.idTag);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool AuthorizeConfConverter::fromJson(const rapidjson::Value& json,
                                      AuthorizeConf&          data,
                                      std::string&            error_code,
                                      std::string&            error_message)
{
    IdTagInfoConverter id_tag_info_converter;
    bool               ret = id_tag_info_converter.fromJson(json["idTagInfo"], data.idTagInfo, error_code, error_message);
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool AuthorizeConfConverter::toJson(const AuthorizeConf& data, rapidjson::Document& json)
{
    IdTagInfoConverter id_tag_info_converter;
    id_tag_info_converter.setAllocator(allocator);
    rapidjson::Document id_tag_info(rapidjson::kObjectType);
    bool                ret = id_tag_info_converter.toJson(data.idTagInfo, id_tag_info);
    json.AddMember(rapidjson::StringRef("idTagInfo"), id_tag_info.Move(), *allocator);
    return ret;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
