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
#include "IRpcClient.h"
#include "Logger.h"

#include <filesystem>

namespace ocpp
{
namespace messages
{

/** @brief Constructor */
MessageDispatcher::MessageDispatcher(const std::string& schemas_path) : m_schemas_path(schemas_path), m_handlers() { }

/** @brief Destructor */
MessageDispatcher::~MessageDispatcher() { }

/** @copydoc bool IMessageDispatcher::registerHandler(const std::string&, IMessageHandler&) */
bool MessageDispatcher::registerHandler(const std::string& action, IMessageHandler& handler)
{
    bool ret = false;

    // Check if handler exists for this action
    if (m_handlers.find(action) == m_handlers.end())
    {
        // Load the payload validator
        std::shared_ptr<ocpp::json::JsonValidator> validator = std::make_shared<ocpp::json::JsonValidator>();
        std::filesystem::path                      filepath(m_schemas_path);
        filepath.append(action + ".json");
        if (validator->init(filepath))
        {
            LOG_DEBUG << "[" << action << "] Validator loaded : " << filepath;

            // Add handler
            std::pair<std::shared_ptr<ocpp::json::JsonValidator>, IMessageHandler*> handler_data(validator, &handler);

            m_handlers[action] = handler_data;
            ret                = true;
        }
        else
        {
            LOG_ERROR << "[" << action << "] Unable to load validator : " << filepath;
        }
    }

    return ret;
}

/** @copydoc bool IMessageDispatcher::dispatchMessage(const std::string&,
                                                          const rapidjson::Value&,
                                                          rapidjson::Document&,
                                                          const char*&,
                                                          std::string&) */
bool MessageDispatcher::dispatchMessage(const std::string&      action,
                                        const rapidjson::Value& payload,
                                        rapidjson::Document&    response,
                                        const char*&            error_code,
                                        std::string&            error_message)
{
    bool ret = false;

    // Look for a handler
    auto it = m_handlers.find(action);
    if (it != m_handlers.end())
    {
        // Check payload
        auto&                                      handler_data = it->second;
        std::shared_ptr<ocpp::json::JsonValidator> validator    = handler_data.first;
        if (validator->isValid(payload))
        {
            // Call handler
            IMessageHandler* handler = handler_data.second;
            ret                      = handler->handle(action, payload, response, error_code, error_message);
        }
        else
        {
            // Invalid payload
            error_code    = ocpp::rpc::IRpcClient::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
            error_message = validator->lastError();
        }
    }
    else
    {
        // Not implemented
        error_code = ocpp::rpc::IRpcClient::RPC_ERROR_NOT_IMPLEMENTED;
    }

    return ret;
}

} // namespace messages
} // namespace ocpp
