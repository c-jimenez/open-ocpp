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

#ifndef GETCONFIGURATION_H
#define GETCONFIGURATION_H

#include "CiStringType.h"
#include "IMessageConverter.h"
#include "KeyValue.h"

#include <vector>

namespace ocpp
{
namespace messages
{

/** @brief Action corresponding to the GetConfiguration messages */
static const std::string GETCONFIGURATION_ACTION = "GetConfiguration";

/** @brief GetConfiguration.req message */
struct GetConfigurationReq
{
    /** @brief Optional. List of keys for which the configuration value is requested */
    ocpp::types::Optional<std::vector<ocpp::types::CiStringType<50u>>> key;
};

/** @brief GetConfiguration.conf message */
struct GetConfigurationConf
{
    /** @brief Optional. List of requested or known keys */
    ocpp::types::Optional<std::vector<ocpp::types::KeyValue>> configurationKey;
    /** @brief Optional. Requested keys that are unknown */
    ocpp::types::Optional<std::vector<ocpp::types::CiStringType<50u>>> unknownKey;
};

// Message converters
MESSAGE_CONVERTERS(GetConfiguration)

} // namespace messages
} // namespace ocpp

#endif // GETCONFIGURATION_H
