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

#ifndef OPENOCPP_RESET_H
#define OPENOCPP_RESET_H

#include "Enums.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the Reset messages */
static const std::string RESET_ACTION = "Reset";

/** @brief Reset.req message */
struct ResetReq
{
    /** @brief Required. This contains the type of reset that the Charge Point should perform. */
    ocpp::types::ocpp16::ResetType type;
};

/** @brief Reset.conf message */
struct ResetConf
{
    /** @brief Required. This indicates whether the Charge Point is able to perform the reset */
    ocpp::types::ocpp16::ResetStatus status;
};

// Message converters
MESSAGE_CONVERTERS(Reset)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_RESET_H
