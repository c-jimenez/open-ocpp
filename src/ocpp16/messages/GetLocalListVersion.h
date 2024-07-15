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

#ifndef OPENOCPP_GETLOCALLISTVERSION_H
#define OPENOCPP_GETLOCALLISTVERSION_H

#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the GetLocalListVersion messages */
static const std::string GET_LOCAL_LIST_VERSION_ACTION = "GetLocalListVersion";

/** @brief GetLocalListVersion.req message */
struct GetLocalListVersionReq
{
    // No fields are defined
};

/** @brief GetLocalListVersion.conf message */
struct GetLocalListVersionConf
{
    /** @brief Required. This contains the current version number of the local authorization list
               in the Charge Point */
    int listVersion;
};

// Message converters
MESSAGE_CONVERTERS(GetLocalListVersion)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_GETLOCALLISTVERSION_H
