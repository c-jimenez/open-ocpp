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

#include "MessageDispatcher.h"
#include "IRpc.h"
#include "JsonValidator.h"
#include "Logger.h"
#include "MessagesValidator.h"

#include <filesystem>

namespace ocpp
{
namespace messages
{

/** @brief Constructor */
MessageDispatcher::MessageDispatcher(const MessagesValidator& messages_validator) : m_messages_validator(messages_validator), m_handlers()
{
}

/** @brief Destructor */
MessageDispatcher::~MessageDispatcher() { }

/** @copydoc bool IMessageDispatcher::registerHandler(const std::string&, IMessageHandler&, bool) */
bool MessageDispatcher::registerHandler(const std::string& action, IMessageHandler& handler, bool allow_replace)
{
    bool ret = false;

    // Check if handler exists for this action
    if (allow_replace || (m_handlers.find(action) == m_handlers.end()))
    {
        // Get the payload validator
        ocpp::json::JsonValidator* validator = m_messages_validator.getValidator(action, true);
        if (validator)
        {
            LOG_DEBUG << "[" << action << "] Validator loaded";

            // Add handler
            std::pair<ocpp::json::JsonValidator*, IMessageHandler*> handler_data(validator, &handler);

            m_handlers[action] = handler_data;
            ret                = true;
        }
        else
        {
            LOG_ERROR << "[" << action << "] Unable to load validator";
        }
    }

    return ret;
}

/** @copydoc bool IMessageDispatcher::dispatchMessage(const std::string&,
                                                          const rapidjson::Value&,
                                                          rapidjson::Document&,
                                                          std::string&,
                                                          std::string&) */
bool MessageDispatcher::dispatchMessage(const std::string&      action,
                                        const rapidjson::Value& payload,
                                        rapidjson::Document&    response,
                                        std::string&            error_code,
                                        std::string&            error_message)
{
    bool ret = false;

    // Look for a handler
    auto it = m_handlers.find(action);
    if (it != m_handlers.end())
    {
        // Check payload
        auto&                      handler_data = it->second;
        ocpp::json::JsonValidator* validator    = handler_data.first;
        if (validator && validator->isValid(payload))
        {
            // Call handler
            IMessageHandler* handler = handler_data.second;
            ret                      = handler->handle(action, payload, response, error_code, error_message);
        }
        else
        {
            // Invalid payload
            error_code    = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
            error_message = validator->lastError();
        }
    }
    else
    {
        // Not implemented
        error_code = ocpp::rpc::IRpc::RPC_ERROR_NOT_IMPLEMENTED;
    }

    return ret;
}

} // namespace messages
} // namespace ocpp
