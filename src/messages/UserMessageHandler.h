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

#ifndef OPENOCPP_USERMESSAGEHANDLER_H
#define OPENOCPP_USERMESSAGEHANDLER_H

#include "GenericMessageHandler.h"

#include <functional>

namespace ocpp
{
namespace messages
{

/** @brief User message handler */
template <typename RequestType, typename ResponseType>
class UserMessageHandler : public GenericMessageHandler<RequestType, ResponseType>
{
  public:
    /** @brief Message handler function*/
    typedef std::function<bool(const RequestType&, ResponseType&, std::string&, std::string&)> HandlerFunc;

    /** @brief Constructor */
    UserMessageHandler(const std::string& action, const GenericMessagesConverter& messages_converter, HandlerFunc user_handler)
        : GenericMessageHandler<RequestType, ResponseType>(action, messages_converter), m_user_handler(user_handler)
    {
    }

    /** @brief Destructor */
    virtual ~UserMessageHandler() { }

    /**
     * @brief Handle an incoming call request
     * @param request Payload of the request
     * @param response Payload of the response
     * @param error_code Standard error code, empty if no error
     * @param error_msg Additionnal error message, empty if no error
     * @return true if the call is accepted, false otherwise
     */
    bool handleMessage(const RequestType& request, ResponseType& response, std::string& error_code, std::string& error_message) override
    {
        return m_user_handler(request, response, error_code, error_message);
    }

  private:
    /** @brief User handler */
    HandlerFunc m_user_handler;
};

} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_USERMESSAGEHANDLER_H
