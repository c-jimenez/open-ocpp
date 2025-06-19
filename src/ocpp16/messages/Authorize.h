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

#ifndef OPENOCPP_AUTHORIZE_H
#define OPENOCPP_AUTHORIZE_H

#include "IMessageConverter.h"
#include "IdTagInfo.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the Authorize messages */
static const std::string AUTHORIZE_ACTION = "Authorize";

/** @brief Authorize.req message */
struct AuthorizeReq
{
    /** @brief Required. This contains the identifier that needs to be authorized */
    ocpp::types::ocpp16::IdToken idTag;
};

/** @brief Authorize.conf message */
struct AuthorizeConf
{
    /** @brief Required. This contains information about authorization status, expiry and
                parent id */
    ocpp::types::ocpp16::IdTagInfo idTagInfo;
};

// Message converters
MESSAGE_CONVERTERS(Authorize)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_AUTHORIZE_H
