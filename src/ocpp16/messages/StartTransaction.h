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

#ifndef OPENOCPP_STARTTRANSACTION_H
#define OPENOCPP_STARTTRANSACTION_H

#include "Enums.h"
#include "IMessageConverter.h"
#include "IdTagInfo.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the StartTransaction messages */
static const std::string START_TRANSACTION_ACTION = "StartTransaction";

/** @brief StartTransaction.req message */
struct StartTransactionReq
{
    /** @brief Required. This identifies which connector of the Charge Point is used */
    unsigned int connectorId;
    /** @brief Required. This contains the identifier for which a transaction has to be started */
    ocpp::types::ocpp16::IdToken idTag;
    /** @brief Required. This contains the meter value in Wh for the connector at start of the
               transaction */
    int meterStart;
    /** @brief Optional. This contains the id of the reservation that terminates as a result of
               this transaction */
    ocpp::types::Optional<int> reservationId;
    /** @brief Required. This contains the date and time on which the transaction is started */
    ocpp::types::DateTime timestamp;
};

/** @brief StartTransaction.conf message */
struct StartTransactionConf
{
    /** @brief Required. This contains information about authorization status, expiry and
               parent id */
    ocpp::types::ocpp16::IdTagInfo idTagInfo;
    /** @brief Required. This contains the transaction id supplied by the Central System */
    int transactionId;
};

// Message converters
MESSAGE_CONVERTERS(StartTransaction)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_STARTTRANSACTION_H
