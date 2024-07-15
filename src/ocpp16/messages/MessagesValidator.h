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

#ifndef OPENOCPP_MESSAGESVALIDATOR_H
#define OPENOCPP_MESSAGESVALIDATOR_H

#include "JsonValidator.h"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace ocpp
{
namespace messages
{

/** @brief Stores all the needed message validators */
class MessagesValidator
{
  public:
    /** @brief Constructor */
    MessagesValidator();
    /** @brief Destructor */
    virtual ~MessagesValidator();

    /**
     * @brief Load the messages validators
     * @return true if all the messages validators have been loaded, false otherwise
     */
    bool load(const std::string& schemas_path);

    /**
     * @brief Get the message validator corresponding to a given action
     * @param action Action corresponding to the message
     * @param is_req Indicate if the validator if for the request or the response
     * @return Message validator if it exists, nullptr otherwise
    */
    ocpp::json::JsonValidator* getValidator(const std::string& action, bool is_req) const;

  private:
    /** @brief Messages validators for requests */
    std::unordered_map<std::string, std::shared_ptr<ocpp::json::JsonValidator>> m_req_validators;
    /** @brief Messages validators for responses */
    std::unordered_map<std::string, std::shared_ptr<ocpp::json::JsonValidator>> m_resp_validators;

    /** @brief Add a message validator bfor both request and response */
    bool addValidator(const std::string& schemas_path, const std::string& action);
    /** @brief Add a message validator */
    bool addValidator(const std::filesystem::path& validator_path, const std::string& action, bool is_req);
};

} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_MESSAGESVALIDATOR_H
