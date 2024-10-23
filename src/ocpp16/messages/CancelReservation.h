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

#ifndef OPENOCPP_CANCELRESERVATION_H
#define OPENOCPP_CANCELRESERVATION_H

#include "Enums.h"
#include "IMessageConverter.h"

#include <string>

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the CancelReservation messages */
static const std::string CANCEL_RESERVATION_ACTION = "CancelReservation";

/** @brief CancelReservation.req message */
struct CancelReservationReq
{
    /** @brief Required. Id of the reservation to cancel */
    int reservationId;
};

/** @brief CancelReservation.conf message */
struct CancelReservationConf
{
    /** @brief Required. This indicates the success or failure of the cancelling of
               a reservation by Central System */
    ocpp::types::ocpp16::CancelReservationStatus status;
};

// Message converters
MESSAGE_CONVERTERS(CancelReservation)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_CANCELRESERVATION_H
