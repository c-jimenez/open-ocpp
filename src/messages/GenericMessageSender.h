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

#ifndef OPENOCPP_GENERICMESSAGESENDER_H
#define OPENOCPP_GENERICMESSAGESENDER_H

#include "EnumToStringFromString.h"
#include "GenericMessagesConverter.h"
#include "IMessagesValidator.h"
#include "IRequestFifo.h"
#include "IRpc.h"
#include "Logger.h"

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
    Failed,
    /** @brief A call error message has been received */
    Error
};

/** @brief Helper to convert a CallResult enum to string */
extern const ocpp::types::EnumToStringFromString<CallResult> CallResultHelper;

/** @brief Generic message sender with C++ data type to JSON conversion */
class GenericMessageSender
{
  public:
    /** @brief Constructor */
    GenericMessageSender(ocpp::rpc::IRpc&          rpc,
                         GenericMessagesConverter& messages_converter,
                         const IMessagesValidator& messages_validator,
                         std::chrono::milliseconds timeout)
        : m_rpc(rpc), m_messages_converter(messages_converter), m_messages_validator(messages_validator), m_timeout(timeout)
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
     * @brief Set the call request timeout
     * @param timeout New timeout value
     */
    void setTimeout(std::chrono::milliseconds timeout) { m_timeout = timeout; }

    /**
     * @brief Execute a call request
     * @param action RPC action for the request
     * @param request Request payload
     * @param response Response payload
     * @param request_fifo Optional. Pointer to the request FIFO to use when messages cannot be sent.
     * @param connector_id Optional. Id of the connector associated to the request.
     * @return Result of the call request (See CallResult documentation)
     */
    template <typename RequestType, typename ResponseType>
    CallResult call(const std::string& action,
                    const RequestType& request,
                    ResponseType&      response,
                    IRequestFifo*      request_fifo = nullptr,
                    unsigned int       connector_id = 0)
    {
        std::string error;
        std::string message;
        return call(action, request, response, error, message, request_fifo, connector_id);
    }

    /**
     * @brief Execute a call request
     * @param action RPC action for the request
     * @param request Request payload
     * @param response Response payload
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return Result of the call request (See CallResult documentation)
     */
    template <typename RequestType, typename ResponseType>
    CallResult call(const std::string& action, const RequestType& request, ResponseType& response, std::string& error, std::string& message)
    {
        return call(action, request, response, error, message, nullptr, 0);
    }

    /**
     * @brief Execute a call request
     * @param action RPC action for the request
     * @param request Request payload
     * @param response Response payload
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @param request_fifo Pointer to the request FIFO to use when messages cannot be sent.
     * @param connector_id Id of the connector associated to the request.
     * @return Result of the call request (See CallResult documentation)
     */
    template <typename RequestType, typename ResponseType>
    CallResult call(const std::string& action,
                    const RequestType& request,
                    ResponseType&      response,
                    std::string&       error,
                    std::string&       message,
                    IRequestFifo*      request_fifo,
                    unsigned int       connector_id)
    {
        CallResult ret = CallResult::Failed;

        // Get converters
        auto req_converter  = m_messages_converter.getRequestConverter<RequestType>(action);
        auto resp_converter = m_messages_converter.getResponseConverter<ResponseType>(action);
        if (req_converter && resp_converter)
        {
            // Convert request
            rapidjson::Document payload(rapidjson::kObjectType);
            req_converter->setAllocator(&payload.GetAllocator());
            if (req_converter->toJson(request, payload))
            {
                // Check if request_fifo is empty
                if (!request_fifo || request_fifo->empty())
                {
                    // Execute call
                    rapidjson::Document rpc_frame;
                    rapidjson::Value    resp;
                    if (m_rpc.call(action, payload, rpc_frame, resp, error, message, m_timeout))
                    {
                        // Check error
                        if (error.empty())
                        {
                            // Validate response
                            ocpp::json::JsonValidator* validator = m_messages_validator.getValidator(action, false);
                            if (validator)
                            {
                                if (validator->isValid(resp))
                                {
                                    // Convert response
                                    std::string error_code;
                                    std::string error_message;
                                    resp_converter->setAllocator(&rpc_frame.GetAllocator());
                                    if (resp_converter->fromJson(resp, response, error_code, error_message))
                                    {
                                        ret = CallResult::Ok;
                                    }
                                }
                                else
                                {
                                    LOG_ERROR << "[" << action << "] - Invalid response : " << validator->lastError();
                                }
                            }
                        }
                        else
                        {
                            ret = CallResult::Error;
                        }
                    }
                    else
                    {
                        // Request cannot be sent or timed out, queue the message inside the FIFO
                        if (request_fifo)
                        {
                            request_fifo->push(connector_id, action, payload);
                            ret = CallResult::Delayed;
                        }
                    }
                }
                else
                {
                    // FIFO is not empty, queue the message inside the FIFO to ensure the order of the messages
                    request_fifo->push(connector_id, action, payload);
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
        auto resp_converter = m_messages_converter.getResponseConverter<ResponseType>(action);
        if (resp_converter)
        {
            // Execute call
            rapidjson::Document rpc_frame;
            rapidjson::Value    resp;
            std::string         error;
            std::string         message;
            if (m_rpc.call(action, request, rpc_frame, resp, error, message, m_timeout))
            {
                // Check error
                if (error.empty())
                {
                    // Validate response
                    ocpp::json::JsonValidator* validator = m_messages_validator.getValidator(action, false);
                    if (validator)
                    {
                        if (validator->isValid(resp))
                        {
                            // Convert response
                            std::string error_code;
                            std::string error_message;
                            resp_converter->setAllocator(&rpc_frame.GetAllocator());
                            if (resp_converter->fromJson(resp, response, error_code, error_message))
                            {
                                ret = CallResult::Ok;
                            }
                        }
                        else
                        {
                            LOG_ERROR << "[" << action << "] - Invalid response : " << validator->lastError();
                        }
                    }
                }
                else
                {
                    ret = CallResult::Error;
                }
            }
        }

        return ret;
    }

  private:
    /** @brief RPC */
    ocpp::rpc::IRpc& m_rpc;
    /** @brief Messages converter */
    GenericMessagesConverter& m_messages_converter;
    /** @brief Messages validator */
    const IMessagesValidator& m_messages_validator;
    /** @brief Request timeout */
    std::chrono::milliseconds m_timeout;
};

} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_GENERICMESSAGESENDER_H
