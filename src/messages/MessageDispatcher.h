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

#ifndef OPENOCPP_MESSAGEDISPATCHER_H
#define OPENOCPP_MESSAGEDISPATCHER_H

#include "IMessageDispatcher.h"

#include <memory>
#include <unordered_map>

namespace ocpp
{
namespace json
{
class JsonValidator;
} // namespace json
namespace messages
{

class IMessagesValidator;

/** @brief Messages dispatcher */
class MessageDispatcher : public IMessageDispatcher
{
  public:
    /** @brief Constructor
     *  @param messages_validator JSON schemas needed to validate payloads
     */
    MessageDispatcher(const IMessagesValidator& messages_validator);

    /** @brief Destructor */
    virtual ~MessageDispatcher();

    /** @copydoc bool IMessageDispatcher::registerHandler(const std::string&, IMessageHandler&, bool) */
    bool registerHandler(const std::string& action, IMessageHandler& handler, bool allow_replace = false) override;

    /** @copydoc bool IMessageDispatcher::dispatchMessage(const std::string&,
                                                          const rapidjson::Value&,
                                                          rapidjson::Document&,
                                                          std::string&,
                                                          std::string&) */
    bool dispatchMessage(const std::string&      action,
                         const rapidjson::Value& payload,
                         rapidjson::Document&    response,
                         std::string&            error_code,
                         std::string&            error_message) override;

  private:
    /** @brief JSON schemas needed to validate payloads */
    const IMessagesValidator& m_messages_validator;
    /** @brief Handlers */
    std::unordered_map<std::string, std::pair<ocpp::json::JsonValidator*, IMessageHandler*>> m_handlers;
};

} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_MESSAGEDISPATCHER_H
