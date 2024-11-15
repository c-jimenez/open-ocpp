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

#ifndef OPENOCPP_OCPP20_ITRIGGERMESSAGEMANAGER20_H
#define OPENOCPP_OCPP20_ITRIGGERMESSAGEMANAGER20_H

#include "EVSEType20.h"
#include "MessageTriggerEnumType20.h"
#include "Optional.h"

namespace ocpp
{
namespace chargepoint
{
namespace ocpp20
{

/** @brief Interface for TriggerMessage managers implementation */
class ITriggerMessageManager
{
  public:
    // Forward declaration
    class ITriggerMessageHandler;

    /** @brief Destructor */
    virtual ~ITriggerMessageManager() { }

    /**
     * @brief Register a handler for a specific message trigger request
     * @param message Type of trigger message requested
     * @param handler Handler to register
     */
    virtual void registerHandler(ocpp::types::ocpp20::MessageTriggerEnumType message, ITriggerMessageHandler& handler) = 0;

    /** @brief Interface for trigger message handlers implementations */
    class ITriggerMessageHandler
    {
      public:
        /** @brief Destructor */
        virtual ~ITriggerMessageHandler() { }

        /**
         * @brief Called on reception of a trigger message request
         * @param message Type of trigger message requested
         * @param evse EVSE concerned by the request
         * @return true if the requested message can be sent, false otherwise
         */
        virtual bool onTriggerMessage(ocpp::types::ocpp20::MessageTriggerEnumType                 message,
                                      const ocpp::types::Optional<ocpp::types::ocpp20::EVSEType>& evse) = 0;
    };
};

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_OCPP20_ITRIGGERMESSAGEMANAGER20_H
