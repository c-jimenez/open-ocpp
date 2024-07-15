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

#ifndef OPENOCPP_REMOTESTARTTRANSACTION_H
#define OPENOCPP_REMOTESTARTTRANSACTION_H

#include "ChargingProfile.h"
#include "Enums.h"
#include "IMessageConverter.h"
#include "IdToken.h"
#include "Optional.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the RemoteStartTransaction messages */
static const std::string REMOTE_START_TRANSACTION_ACTION = "RemoteStartTransaction";

/** @brief RemoteStartTransaction.req message */
struct RemoteStartTransactionReq
{
    /** @brief Optional. Number of the connector on which to start the transaction.
               connectorId SHALL be > 0 */
    ocpp::types::Optional<unsigned int> connectorId;
    /** @brief Required. The identifier that Charge Point must use to start a transaction */
    ocpp::types::ocpp16::IdToken idTag;
    /** @brief Optional. Charging Profile to be used by the Charge Point for the requested
               transaction. ChargingProfilePurpose MUST be set to TxProfile */
    ocpp::types::Optional<ocpp::types::ocpp16::ChargingProfile> chargingProfile;
};

/** @brief RemoteStopTransaction.conf message */
struct RemoteStartTransactionConf
{
    /** @brief Required. Status indicating whether Charge Point accepts the
               request to start a transaction */
    ocpp::types::ocpp16::RemoteStartStopStatus status;
};

// Message converters
MESSAGE_CONVERTERS(RemoteStartTransaction)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_REMOTESTARTTRANSACTION_H
