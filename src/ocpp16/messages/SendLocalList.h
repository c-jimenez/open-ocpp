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

#ifndef OPENOCPP_SENDLOCALLIST_H
#define OPENOCPP_SENDLOCALLIST_H

#include "AuthorizationData.h"
#include "Enums.h"
#include "IMessageConverter.h"

#include <vector>

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the SendLocalList messages */
static const std::string SEND_LOCAL_LIST_ACTION = "SendLocalList";

/** @brief SendLocalList.req message */
struct SendLocalListReq
{
    /** @brief Required. In case of a full update this is the version number of the
               full list. In case of a differential update it is the version number of
               the list after the update has been applied */
    int listVersion;
    /** @brief Optional. In case of a full update this contains the list of values
               that form the new local authorization list. In case of a differential
               update it contains the changes to be applied to the local
               authorization list in the Charge Point. Maximum number of
               AuthorizationData elements is available in the configuration key:
               SendLocalListMaxLength */
    std::vector<ocpp::types::ocpp16::AuthorizationData> localAuthorizationList;
    /** @brief Required. This contains the type of update (full or differential) of
               this request */
    ocpp::types::ocpp16::UpdateType updateType;
};

/** @brief SendLocalList.conf message */
struct SendLocalListConf
{
    /** @brief Required. This indicates whether the Charge Point has successfully received and
               applied the update of the local authorization list */
    ocpp::types::ocpp16::UpdateStatus status;
};

// Message converters
MESSAGE_CONVERTERS(SendLocalList)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_SENDLOCALLIST_H
