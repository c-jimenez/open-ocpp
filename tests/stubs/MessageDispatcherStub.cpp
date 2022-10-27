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

#include "MessageDispatcherStub.h"

namespace ocpp
{
namespace messages
{

/** @brief Constructor */
MessageDispatcherStub::MessageDispatcherStub() : m_handlers() { }
/** @brief Destructor */
MessageDispatcherStub::~MessageDispatcherStub() { }

/** @copydoc bool IMessageDispatcher::registerHandler(const std::string&, IMessageHandler&, bool) */
bool MessageDispatcherStub::registerHandler(const std::string& action, IMessageHandler& handler, bool allow_replace)
{
    bool ret = false;

    if (allow_replace || !hasHandler(action))
    {
        m_handlers[action] = &handler;
        ret                = true;
    }

    return ret;
}

/** @copydoc bool IMessageDispatcher::dispatchMessage(const std::string&,
                                                          const rapidjson::Value&,
                                                          rapidjson::Document&,
                                                          const char*&,
                                                          std::string&) */
bool MessageDispatcherStub::dispatchMessage(const std::string&      action,
                                            const rapidjson::Value& payload,
                                            rapidjson::Document&    response,
                                            const char*&            error_code,
                                            std::string&            error_message)
{
    bool ret          = false;
    auto iter_handler = m_handlers.find(action);
    if (iter_handler != m_handlers.end())
    {
        ret = iter_handler->second->handle(action, payload, response, error_code, error_message);
    }
    return ret;
}

/** @brief Check if a specific action as a registered handler */
bool MessageDispatcherStub::hasHandler(const std::string& action) const
{
    auto iter_handler = m_handlers.find(action);
    return (iter_handler != m_handlers.end());
}

} // namespace messages
} // namespace ocpp
