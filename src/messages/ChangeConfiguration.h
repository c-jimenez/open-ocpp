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

#ifndef CHANGECONFIGURATION_H
#define CHANGECONFIGURATION_H

#include "CiStringType.h"
#include "Enums.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{

/** @brief Action corresponding to the ChangeConfiguration messages */
static const std::string CHANGECONFIGURATION_ACTION = "ChangeConfiguration";

/** @brief ChangeConfiguration.req message */
struct ChangeConfigurationReq
{
    /** @brief Required. The name of the configuration setting to change */
    ocpp::types::CiStringType<50u> key;
    /** @brief Required. The new value as string for the setting */
    ocpp::types::CiStringType<500u> value;
};

/** @brief ChangeConfiguration.conf message */
struct ChangeConfigurationConf
{
    /** @brief Required. Returns whether configuration change has been accepted */
    ocpp::types::ConfigurationStatus status;
};

// Message converters
MESSAGE_CONVERTERS(ChangeConfiguration)

} // namespace messages
} // namespace ocpp

#endif // CHANGECONFIGURATION_H
