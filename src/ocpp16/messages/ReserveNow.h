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

#ifndef OPENOCPP_RESERVENOW_H
#define OPENOCPP_RESERVENOW_H

#include "DateTime.h"
#include "Enums.h"
#include "IMessageConverter.h"
#include "IdToken.h"

#include <string>

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the ReserveNow messages */
static const std::string RESERVE_NOW_ACTION = "ReserveNow";

/** @brief ReserveNow.req message */
struct ReserveNowReq
{
    /** @brief Required. This contains the id of the connector to be reserved. A value of 0
               means that the reservation is not for a specific connector */
    unsigned int connectorId;
    /** @brief Required. This contains the id of the connector to be reserved. A value of 0
                 means that the reservation is not for a specific connector */
    ocpp::types::DateTime expiryDate;
    /** @brief Required. The identifier for which the Charge Point has to reserve a connector */
    ocpp::types::ocpp16::IdToken idTag;
    /** @brief Optional. The parent idTag */
    ocpp::types::Optional<ocpp::types::ocpp16::IdToken> parentIdTag;
    /** @brief Required. Unique id for this reservation */
    int reservationId;
};

/** @brief ReserveNow.conf message */
struct ReserveNowConf
{
    /** @brief Required. This indicates the success or failure of the reservation */
    ocpp::types::ocpp16::ReservationStatus status;
};

// Message converters
MESSAGE_CONVERTERS(ReserveNow)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_RESERVENOW_H
