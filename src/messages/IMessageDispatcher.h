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

#ifndef OPENOCPP_IMESSAGEDISPATCHER_H
#define OPENOCPP_IMESSAGEDISPATCHER_H

#include "json.h"

#include <string>

namespace ocpp
{
namespace messages
{

/** @brief Interface for messages dispatcher implementations */
class IMessageDispatcher
{
  public:
    // Forward declarations
    class IMessageHandler;

    /** @brief Destructor */
    virtual ~IMessageDispatcher() { }

    /**
     * @brief Register a message handler for a specific action
     * @param action Action
     * @param handler Message handler
     * @param allow_replace Indicate if the replacement of an already registered is allowed
     * @return false if a handler is already registered for this action, true otherwise
     */
    virtual bool registerHandler(const std::string& action, IMessageHandler& handler, bool allow_replace = false) = 0;

    /**
     * @brief Dispatch a received action to the registered handler
     * @param action Action
     * @param payload JSON payload for the action
     * @param response JSON response to send
     * @param error_code Standard error code, empty if no error
     * @param error_msg Additionnal error message, empty if no error
     * @return true if the call is accepted, false otherwise
     */
    virtual bool dispatchMessage(const std::string&      action,
                                 const rapidjson::Value& payload,
                                 rapidjson::Document&    response,
                                 std::string&            error_code,
                                 std::string&            error_message) = 0;

    /** @brief Interface for messages handlers implementations */
    class IMessageHandler
    {
      public:
        /** @brief Destructor */
        virtual ~IMessageHandler() { }

        /**
         * @brief Handle a received action
         * @param action Action
         * @param payload JSON payload for the action
         * @param response JSON response to send
         * @param error_code Standard error code, empty if no error
         * @param error_msg Additionnal error message, empty if no error
         * @return true if the call is accepted, false otherwise
         */
        virtual bool handle(const std::string&      action,
                            const rapidjson::Value& payload,
                            rapidjson::Document&    response,
                            std::string&            error_code,
                            std::string&            error_message) = 0;
    };
};

} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_IMESSAGEDISPATCHER_H
