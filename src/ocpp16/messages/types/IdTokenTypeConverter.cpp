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

#include "IdTokenTypeConverter.h"
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

/** @bcopydoc bool IMessageConverter<ocpp::types::ocpp16::IdTokenType>::fromJson(const rapidjson::Value&,
 *                                                                     ocpp::types::ocpp16::IdTokenType&,
 *                                                                     std::string&,
 *                                                                     std::string&) */
bool IdTokenTypeConverter::fromJson(const rapidjson::Value&           json,
                                    ocpp::types::ocpp16::IdTokenType& data,
                                    std::string&                      error_code,
                                    std::string&                      error_message)
{
    (void)error_code;
    (void)error_message;
    extract(json, "idToken", data.idToken);
    return true;
}

/** @copydoc bool IMessageConverter<ocpp::types::ocpp16::IdTokenType>::toJson(const ocpp::types::ocpp16::IdTokenType&,
 *                                                                  rapidjson::Document&) */
bool IdTokenTypeConverter::toJson(const ocpp::types::ocpp16::IdTokenType& data, rapidjson::Document& json)
{
    fill(json, "idToken", data.idToken);
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
