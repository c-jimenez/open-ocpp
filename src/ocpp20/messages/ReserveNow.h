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
    Generated by json2cpp from : urn:OCPP:Cp:2:2020:3:ReserveNowRequest
                                 OCPP 2.0.1 FINAL
*/

#ifndef OPENOCPP_RESERVENOW_H
#define OPENOCPP_RESERVENOW_H

#include "IMessageConverter.h"

#include "CustomDataType.h"
#include "ConnectorEnumType.h"
#include "IdTokenType.h"
#include "IdTokenType.h"
#include "ReserveNowStatusEnumType.h"
#include "StatusInfoType.h"

namespace ocpp
{
namespace messages
{
namespace ocpp20
{

/** @brief Action corresponding to the ReserveNow messages */
static const std::string RESERVENOW_ACTION = "ReserveNow";

/** @brief ReserveNowReq message */
struct ReserveNowReq
{
    /** @brief  */
    ocpp::types::Optional<ocpp::types::ocpp20::CustomDataType> customData;
    /** @brief Id of reservation. */
    int id;
    /** @brief Date and time at which the reservation expires. */
    ocpp::types::DateTime expiryDateTime;
    /** @brief  */
    ocpp::types::Optional<ocpp::types::ocpp20::ConnectorEnumType> connectorType;
    /** @brief  */
    ocpp::types::ocpp20::IdTokenType idToken;
    /** @brief This contains ID of the evse to be reserved. */
    ocpp::types::Optional<int> evseId;
    /** @brief  */
    ocpp::types::Optional<ocpp::types::ocpp20::IdTokenType> groupIdToken;
};

/** @brief ReserveNowConf message */
struct ReserveNowConf
{
    /** @brief  */
    ocpp::types::Optional<ocpp::types::ocpp20::CustomDataType> customData;
    /** @brief  */
    ocpp::types::ocpp20::ReserveNowStatusEnumType status;
    /** @brief  */
    ocpp::types::Optional<ocpp::types::ocpp20::StatusInfoType> statusInfo;
};

// Message converters
MESSAGE_CONVERTERS(ReserveNow)

} // namespace ocpp20
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_RESERVENOW_H