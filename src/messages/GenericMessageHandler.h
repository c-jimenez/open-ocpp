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

#ifndef OPENOCPP_GENERICMESSAGEHANDLER_H
#define OPENOCPP_GENERICMESSAGEHANDLER_H

#include "GenericMessagesConverter.h"
#include "IMessageConverter.h"
#include "IMessageDispatcher.h"

namespace ocpp
{
namespace messages
{

/** @brief Generic message handler with JSON to C++ data type conversion */
template <typename RequestType, typename ResponseType>
class GenericMessageHandler : public IMessageDispatcher::IMessageHandler
{
  public:
    /** @brief Constructor */
    GenericMessageHandler(const std::string& action, const GenericMessagesConverter& messages_converter)
        : m_request_converter(messages_converter.getRequestConverter<RequestType>(action)),
          m_response_converter(messages_converter.getResponseConverter<ResponseType>(action))
    {
    }

    /** @brief Destructor */
    virtual ~GenericMessageHandler() { }

    /** @copydoc bool IMessageHandler::handle(const std::string&
                                              const rapidjson::Value&
                                              rapidjson::Document&
                                              std::string&
                                              std::string&) */
    bool handle(const std::string&      action,
                const rapidjson::Value& payload,
                rapidjson::Document&    response,
                std::string&            error_code,
                std::string&            error_message) override
    {
        bool ret = false;
        // Ignore action
        (void)action;

        // Convert request
        RequestType request;
        if (m_request_converter->fromJson(payload, request, error_code, error_message))
        {
            // Handle message
            ResponseType resp;
            if (handleMessage(request, resp, error_code, error_message))
            {
                // Convert response
                m_response_converter->setAllocator(&response.GetAllocator());
                ret = m_response_converter->toJson(resp, response);
            }
        }

        return ret;
    }

    /**
     * @brief Handle an incoming call request
     * @param request Payload of the request
     * @param response Payload of the response
     * @param error_code Standard error code, empty if no error
     * @param error_msg Additionnal error message, empty if no error
     * @return true if the call is accepted, false otherwise
     */
    virtual bool handleMessage(const RequestType& request, ResponseType& response, std::string& error_code, std::string& error_message) = 0;

  private:
    std::unique_ptr<IMessageConverter<RequestType>>  m_request_converter;
    std::unique_ptr<IMessageConverter<ResponseType>> m_response_converter;
};

} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_GENERICMESSAGEHANDLER_H
