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

/*
    Generated by json2cpp from : urn:OCPP:Cp:2:2020:3:GetLocalListVersionRequest
                                 OCPP 2.0.1 FINAL
*/

#ifndef OPENOCPP_GETLOCALLISTVERSION_H
#define OPENOCPP_GETLOCALLISTVERSION_H

#include "IMessageConverter.h"

#include "CustomDataType.h"

namespace ocpp
{
namespace messages
{
namespace ocpp20
{

/** @brief Action corresponding to the GetLocalListVersion messages */
static const std::string GETLOCALLISTVERSION_ACTION = "GetLocalListVersion";

/** @brief GetLocalListVersionReq message */
struct GetLocalListVersionReq
{
    /** @brief  */
    ocpp::types::Optional<ocpp::types::ocpp20::CustomDataType> customData;
};

/** @brief GetLocalListVersionConf message */
struct GetLocalListVersionConf
{
    /** @brief  */
    ocpp::types::Optional<ocpp::types::ocpp20::CustomDataType> customData;
    /** @brief This contains the current version number of the local authorization list in the Charging Station. */
    int versionNumber;
};

// Message converters
MESSAGE_CONVERTERS(GetLocalListVersion)

} // namespace ocpp20
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_GETLOCALLISTVERSION_H