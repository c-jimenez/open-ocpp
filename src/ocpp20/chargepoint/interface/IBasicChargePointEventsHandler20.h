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

#ifndef OPENOCPP_OCPP20_IBASICCHARGEPOINTEVENTSHANDLERCHARGEPOINT20_H
#define OPENOCPP_OCPP20_IBASICCHARGEPOINTEVENTSHANDLERCHARGEPOINT20_H

#include "DateTime.h"
#include "RegistrationStatusEnumType20.h"

namespace ocpp
{
namespace chargepoint
{
namespace ocpp20
{

/** @brief Interface for charge point event handlers implementations */
class IBasicChargePointEventsHandler
{
  public:
    /** @brief Destructor */
    virtual ~IBasicChargePointEventsHandler() { }

    /**
     * @brief Called when the first attempt to connect to the central system has failed
     * @param status Previous registration status (if Accepted, some offline operations are allowed)
     */
    virtual void connectionFailed(ocpp::types::ocpp20::RegistrationStatusEnumType status) = 0;

    /**
     * @brief Called when the charge point connection status has changed
     * @param isConnected true if the charge point is connected to the central system, false otherwise
     */
    virtual void connectionStateChanged(bool isConnected) = 0;

    /**
     * @brief Called on boot notification response from the central system
     * @param status Registration status
     * @param datetime Date and time of the central system
     */
    virtual void bootNotification(ocpp::types::ocpp20::RegistrationStatusEnumType status, const ocpp::types::DateTime& datetime) = 0;

    /**
     * @brief Called when the date and time must be adjusted with the one of the central system
     */
    virtual void datetimeReceived(const ocpp::types::DateTime& datetime) = 0;
};

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_OCPP20_IBASICCHARGEPOINTEVENTSHANDLERCHARGEPOINT20_H
