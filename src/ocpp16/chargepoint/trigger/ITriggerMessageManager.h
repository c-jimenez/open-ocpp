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

#ifndef OPENOCPP_ITRIGGERMESSAGEMANAGER_H
#define OPENOCPP_ITRIGGERMESSAGEMANAGER_H

#include "Enums.h"
#include "Optional.h"

namespace ocpp
{
namespace chargepoint
{

/** @brief Interface for TriggerMessage managers implementation */
class ITriggerMessageManager
{
  public:
    // Forward declaration
    class ITriggerMessageHandler;
    class IExtendedTriggerMessageHandler;

    /** @brief Destructor */
    virtual ~ITriggerMessageManager() { }

    /**
     * @brief Register a handler for a specific standard trigger request
     * @param message Type of trigger message requested
     * @param handler Handler to register
     */
    virtual void registerHandler(ocpp::types::ocpp16::MessageTrigger message, ITriggerMessageHandler& handler) = 0;

    /**
     * @brief Register a handler for a specific extended trigger request
     * @param message Type of trigger message requested
     * @param handler Handler to register
     */
    virtual void registerHandler(ocpp::types::ocpp16::MessageTriggerEnumType message, IExtendedTriggerMessageHandler& handler) = 0;

    /** @brief Interface for standard trigger message handlers implementations */
    class ITriggerMessageHandler
    {
      public:
        /** @brief Destructor */
        virtual ~ITriggerMessageHandler() { }

        /**
         * @brief Called on reception of a standard trigger message request
         * @param message Type of trigger message requested
         * @param connector_id Id of the connector concerned by the request
         * @return true if the requested message can be sent, false otherwise
         */
        virtual bool onTriggerMessage(ocpp::types::ocpp16::MessageTrigger        message,
                                      const ocpp::types::Optional<unsigned int>& connector_id) = 0;
    };

    /** @brief Interface for extended trigger message handlers implementations */
    class IExtendedTriggerMessageHandler
    {
      public:
        /** @brief Destructor */
        virtual ~IExtendedTriggerMessageHandler() { }

        /**
         * @brief Called on reception of an extended trigger message request
         * @param message Type of trigger message requested
         * @param connector_id Id of the connector concerned by the request
         * @return true if the requested message can be sent, false otherwise
         */
        virtual bool onTriggerMessage(ocpp::types::ocpp16::MessageTriggerEnumType message,
                                      const ocpp::types::Optional<unsigned int>&  connector_id) = 0;
    };
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_ITRIGGERMESSAGEMANAGER_H
