/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ICENTRALSYSTEMEVENTSHANDLER_H
#define ICENTRALSYSTEMEVENTSHANDLER_H

#include "ICentralSystem.h"

namespace ocpp
{
namespace centralsystem
{

/** @brief Interface for central system event handlers implementations */
class ICentralSystemEventsHandler
{
  public:
    /** @brief Destructor */
    virtual ~ICentralSystemEventsHandler() { }

    /**
     * @brief Called to check the charge point credentials for HTTP basic authentication
     * @param chargepoint_id Charge Point identifier
     * @param password Password
     * @return true if the credentials are valid, false otherwise
     */
    virtual bool checkCredentials(const std::string& chargepoint_id, const std::string& password) = 0;

    /**
     * @brief Called when a charge point is connected
     * @param chargepoint Charge point connection
     */
    virtual void chargePointConnected(std::shared_ptr<ICentralSystem::IChargePoint> chargepoint) = 0;
};

} // namespace centralsystem
} // namespace ocpp

#endif // ICENTRALSYSTEMEVENTSHANDLER_H
