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

#ifndef GENERICMESSAGESENDER_H
#define GENERICMESSAGESENDER_H

#include "IChargePointConfig.h"
#include "IRequestFifo.h"
#include "IRpc.h"
#include "MessagesConverter.h"

namespace ocpp
{
namespace messages
{

/** @brief Result of of a call request */
enum class CallResult
{
    /** @brief Message has been sent and a response has been received */
    Ok,
    /** @brief Message will be sent later */
    Delayed,
    /** @brief Message cannot be send or no response has been received */
    Failed
};

/** @brief Generic message sender with C++ data type to JSON conversion */
class GenericMessageSender
{
  public:
    /** @brief Constructor */
    GenericMessageSender(const ocpp::config::IChargePointConfig& stack_config, ocpp::rpc::IRpc& rpc, MessagesConverter& messages_converter)
        : m_stack_config(stack_config), m_rpc(rpc), m_messages_converter(messages_converter)
    {
    }

    /** @brief Destructor */
    virtual ~GenericMessageSender() { }

    /**
     * @brief Indicate if the connection with the central system is active
     * @return true if the connection is active, false otherwise
     */
    bool isConnected() const { return m_rpc.isConnected(); }

    /**
     * @brief Execute a call request
     * @param action RPC action for the request
     * @param request Request payload
     * @param response Response payload
     * @param request_fifo Optional. Pointer to the request FIFO to use when messages cannot be sent.
     * @return Result of the call request (See CallResult documentation)
     */
    template <typename RequestType, typename ResponseType>
    CallResult call(const std::string& action, const RequestType& request, ResponseType& response, IRequestFifo* request_fifo = nullptr)
    {
        CallResult ret = CallResult::Failed;

        // Get converters
        IMessageConverter<RequestType>*  req_converter  = m_messages_converter.getRequestConverter<RequestType>(action);
        IMessageConverter<ResponseType>* resp_converter = m_messages_converter.getResponseConverter<ResponseType>(action);
        if (req_converter && resp_converter)
        {
            // Convert request
            rapidjson::Document payload;
            payload.Parse("{}");
            req_converter->setAllocator(&payload.GetAllocator());
            if (req_converter->toJson(request, payload))
            {
                // Check if request_fifo is empty
                if (!request_fifo || (request_fifo->size() == 0))
                {
                    // Execute call
                    rapidjson::Document resp;
                    resp.Parse("{}");
                    if (m_rpc.call(action, payload, resp, static_cast<unsigned int>(m_stack_config.callRequestTimeout().count())))
                    {
                        // Convert response
                        const char* error_code = nullptr;
                        std::string error_message;
                        resp_converter->setAllocator(&resp.GetAllocator());
                        if (resp_converter->fromJson(resp, response, error_code, error_message))
                        {
                            ret = CallResult::Ok;
                        }
                    }
                    else
                    {
                        // Request cannot be sent or timed out, queue the message inside the FIFO
                        if (request_fifo)
                        {
                            request_fifo->push(action, payload);
                            ret = CallResult::Delayed;
                        }
                    }
                }
                else
                {
                    // FIFO is not empty, queue the message inside the FIFO to ensure the order of the messages
                    request_fifo->push(action, payload);
                    ret = CallResult::Delayed;
                }
            }
        }

        return ret;
    }

    /**
     * @brief Execute a call request on a JSON request
     * @param action RPC action for the request
     * @param request JSON request payload
     * @param response Response payload
     * @return Result of the call request (See CallResult documentation)
     */
    template <typename ResponseType>
    CallResult call(const std::string& action, const rapidjson::Document& request, ResponseType& response)
    {
        CallResult ret = CallResult::Failed;

        // Get converter
        IMessageConverter<ResponseType>* resp_converter = m_messages_converter.getResponseConverter<ResponseType>(action);
        if (resp_converter)
        {
            // Execute call
            rapidjson::Document resp;
            resp.Parse("{}");
            if (m_rpc.call(action, request, resp, static_cast<unsigned int>(m_stack_config.callRequestTimeout().count())))
            {
                // Convert response
                const char* error_code = nullptr;
                std::string error_message;
                resp_converter->setAllocator(&resp.GetAllocator());
                if (resp_converter->fromJson(resp, response, error_code, error_message))
                {
                    ret = CallResult::Ok;
                }
            }
        }

        return ret;
    }

  private:
    /** @brief Stack internal configuration */
    const ocpp::config::IChargePointConfig& m_stack_config;
    /** @brief RPC */
    ocpp::rpc::IRpc& m_rpc;
    /** @brief Messages converter */
    MessagesConverter& m_messages_converter;
};

} // namespace messages
} // namespace ocpp

#endif // GENERICMESSAGESENDER_H
