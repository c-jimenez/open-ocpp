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

#ifndef OPENOCPP_REMOTESTOPTRANSACTION_H
#define OPENOCPP_REMOTESTOPTRANSACTION_H

#include "Enums.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the RemoteStopTransaction messages */
static const std::string REMOTE_STOP_TRANSACTION_ACTION = "RemoteStopTransaction";

/** @brief RemoteStopTransaction.req message */
struct RemoteStopTransactionReq
{
    /** @brief Required. The identifier of the transaction which Charge Point is requested to
               stop */
    int transactionId;
};

/** @brief RemoteStopTransaction.conf message */
struct RemoteStopTransactionConf
{
    /** @brief Required. Status indicating whether Charge Point accepts the
               request to stop a transaction */
    ocpp::types::ocpp16::RemoteStartStopStatus status;
};

// Message converters
MESSAGE_CONVERTERS(RemoteStopTransaction)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_REMOTESTOPTRANSACTION_H
