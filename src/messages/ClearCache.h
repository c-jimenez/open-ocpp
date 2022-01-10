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

#ifndef CLEARCACHE_H
#define CLEARCACHE_H

#include "Enums.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{

/** @brief Action corresponding to the ClearCache messages */
static const std::string CLEARCACHE_ACTION = "ClearCache";

/** @brief ClearCache.req message */
struct ClearCacheReq
{
    // No fields are defined
};

/** @brief ClearCache.conf message */
struct ClearCacheConf
{
    /** @brief Required. Accepted if the Charge Point has executed the request, otherwise
               rejected */
    ocpp::types::ClearCacheStatus status;
};

// Message converters
MESSAGE_CONVERTERS(ClearCache)

} // namespace messages
} // namespace ocpp

#endif // CLEARCACHE_H
