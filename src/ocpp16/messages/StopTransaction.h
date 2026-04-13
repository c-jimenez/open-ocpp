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

#ifndef OPENOCPP_STOPTRANSACTION_H
#define OPENOCPP_STOPTRANSACTION_H

#include "Enums.h"
#include "IMessageConverter.h"
#include "IdTagInfo.h"
#include "MeterValue.h"
#include "Optional.h"

#include <vector>

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the StopTransaction messages */
static const std::string STOP_TRANSACTION_ACTION = "StopTransaction";

/** @brief StopTransaction.req message */
struct StopTransactionReq
{
    /** @brief Optional. This contains the identifier which requested to stop the charging. It is
               optional because a Charge Point may terminate charging without the presence
               of an idTag, e.g. in case of a reset. A Charge Point SHALL send the idTag if known */
    ocpp::types::Optional<ocpp::types::ocpp16::IdToken> idTag;
    /** @brief Required. This contains the meter value in Wh for the connector at end of the
               transaction */
    int meterStop;
    /** @brief Required. This contains the date and time on which the transaction is stopped */
    ocpp::types::DateTime timestamp;
    /** @brief Required. This contains the transaction-id as received by the
               StartTransaction.conf */
    int transactionId;
    /** @brief Optional. This contains the reason why the transaction was stopped. MAY only
               be omitted when the Reason is "Local". */
    ocpp::types::Optional<ocpp::types::ocpp16::Reason> reason;
    /** @brief Optional. This contains transaction usage details relevant for billing purposes */
    std::vector<ocpp::types::ocpp16::MeterValue> transactionData;
};

/** @brief StopTransaction.conf message */
struct StopTransactionConf
{
    /** @brief Optional. This contains information about authorization status, expiry and
               parent id. It is optional, because a transaction may have been stopped without
               an identifier */
    ocpp::types::Optional<ocpp::types::ocpp16::IdTagInfo> idTagInfo;
};

// Message converters
MESSAGE_CONVERTERS(StopTransaction)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_STOPTRANSACTION_H
