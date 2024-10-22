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

#ifndef OPENOCPP_OCPP20_INTERNALCONFIGKEYS_H
#define OPENOCPP_OCPP20_INTERNALCONFIGKEYS_H

namespace ocpp
{
namespace chargepoint
{
namespace ocpp20
{

/** @brief Configuration key : stack version */
static constexpr const char* STACK_VERSION_KEY = "StackVersion";
/** @brief Configuration key : last start date */
static constexpr const char* START_DATE_KEY = "LastSessionStartDate";
/** @brief Configuration key : uptime */
static constexpr const char* UPTIME_KEY = "LastSessionUpTime";
/** @brief Configuration key : disconnected time */
static constexpr const char* DISCONNECTED_TIME_KEY = "LastSessionDisconnectedTime";
/** @brief Configuration key : total uptime */
static constexpr const char* TOTAL_UPTIME_KEY = "TotalUpTime";
/** @brief Configuration key : total disconnected time */
static constexpr const char* TOTAL_DISCONNECTED_TIME_KEY = "TotalDisconnectedTime";
/** @brief Configuration key : last connection URL */
static constexpr const char* LAST_CONNECTION_URL_KEY = "LastConnectionUrl";

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_OCPP20_INTERNALCONFIGKEYS_H
