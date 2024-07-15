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

#ifndef OPENOCPP_IMESSAGESVALIDATOR_H
#define OPENOCPP_IMESSAGESVALIDATOR_H

#include "JsonValidator.h"

#include <string>

namespace ocpp
{
namespace messages
{

/** @brief Interface for the message validators implementations */
class IMessagesValidator
{
  public:
    /** @brief Destructor */
    virtual ~IMessagesValidator() { }

    /**
     * @brief Get the message validator corresponding to a given action
     * @param action Action corresponding to the message
     * @param is_req Indicate if the validator if for the request or the response
     * @return Message validator if it exists, nullptr otherwise
    */
    virtual ocpp::json::JsonValidator* getValidator(const std::string& action, bool is_req) const = 0;
};

} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_IMESSAGESVALIDATOR_H
