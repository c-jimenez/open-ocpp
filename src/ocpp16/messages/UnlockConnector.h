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

#ifndef OPENOCPP_UNLOCKCONNECTOR_H
#define OPENOCPP_UNLOCKCONNECTOR_H

#include "Enums.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the UnlockConnector messages */
static const std::string UNLOCK_CONNECTOR_ACTION = "UnlockConnector";

/** @brief UnlockConnector.req message */
struct UnlockConnectorReq
{
    /** @brief Required. This contains the identifier of the connector to be unlocked */
    unsigned int connectorId;
};

/** @brief UnlockConnector.conf message */
struct UnlockConnectorConf
{
    /** @brief Required. This indicates whether the Charge Point has unlocked the connector */
    ocpp::types::ocpp16::UnlockStatus status;
};

// Message converters
MESSAGE_CONVERTERS(UnlockConnector)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_UNLOCKCONNECTOR_H
