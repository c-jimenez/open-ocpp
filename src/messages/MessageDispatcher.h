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

#ifndef MESSAGEDISPATCHER_H
#define MESSAGEDISPATCHER_H

#include "IMessageDispatcher.h"
#include "JsonValidator.h"

#include <map>
#include <memory>

namespace ocpp
{
namespace messages
{

/** @brief Messages dispatcher */
class MessageDispatcher : public IMessageDispatcher
{
  public:
    /** @brief Constructor
     *  @param schemas_path Path to the JSON schemas needed to validate payloads
     */
    MessageDispatcher(const std::string& schemas_path);

    /** @brief Destructor */
    virtual ~MessageDispatcher();

    /** @copydoc bool IMessageDispatcher::registerHandler(const std::string&, IMessageHandler&) */
    bool registerHandler(const std::string& action, IMessageHandler& handler) override;

    /** @copydoc bool IMessageDispatcher::dispatchMessage(const std::string&,
                                                          const rapidjson::Value&,
                                                          rapidjson::Document&,
                                                          const char*&,
                                                          std::string&) */
    bool dispatchMessage(const std::string&      action,
                         const rapidjson::Value& payload,
                         rapidjson::Document&    response,
                         const char*&            error_code,
                         std::string&            error_message) override;

  private:
    /** @brief Path to the JSON schemas needed to validate payloads */
    const std::string m_schemas_path;
    /** @brief Handlers */
    std::map<std::string, std::pair<std::shared_ptr<ocpp::json::JsonValidator>, IMessageHandler*>> m_handlers;
};

} // namespace messages
} // namespace ocpp

#endif // MESSAGEDISPATCHER_H
