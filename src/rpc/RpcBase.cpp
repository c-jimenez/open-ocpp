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

#include "RpcBase.h"
#include "RpcPool.h"

#include <functional>
#include <sstream>

namespace ocpp
{
namespace rpc
{

/** @brief RPC call type */
static constexpr const char* CALL = "2";
/** @brief RPC call result type */
static constexpr const char* CALLRESULT = "3";
/** @brief RPC call error type */
static constexpr const char* CALLERROR = "4";

/** @brief Constructor */
RpcBase::RpcBase(RpcPool* pool)
    : m_pool(pool),
      m_rpc_listener(nullptr),
      m_spies(),
      m_transaction_id(0),
      m_call_mutex(),
      m_requests_queue(),
      m_results_queue(),
      m_rx_thread(nullptr),
      m_rpc_owner()
{
}

/** @brief Destructor */
RpcBase::~RpcBase()
{
    stop();
}

/** @copydoc bool IRpc::call(const std::string&, const rapidjson::Document&, rapidjson::Document&, rapidjson::Value&,
 *                           std::string&, std::string&, std::chrono::milliseconds) */
bool RpcBase::call(const std::string&         action,
                   const rapidjson::Document& payload,
                   rapidjson::Document&       rpc_frame,
                   rapidjson::Value&          response,
                   std::string&               error,
                   std::string&               message,
                   std::chrono::milliseconds  timeout)
{
    bool ret = false;

    // Check connection state
    if (isConnected())
    {
        // Only one RPC request/response at a time
        std::lock_guard<std::mutex> call_lock(m_call_mutex);

        // Serialize message
        rapidjson::StringBuffer                    buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        writer.SetMaxDecimalPlaces(1); // OCPP decimals have 1 digit precision
        payload.Accept(writer);

        std::stringstream serialized_message;
        serialized_message << "[";
        serialized_message << CALL << ", ";
        serialized_message << "\"" << m_transaction_id << "\", ";
        serialized_message << "\"" << action << "\", ";
        serialized_message << buffer.GetString();
        serialized_message << "]";

        // Initialize call context
        std::stringstream id;
        id << m_transaction_id;

        // Send message
        std::string msg = serialized_message.str();
        if (send(msg))
        {
            // Wait for response
            std::stringstream expected_id;
            expected_id << m_transaction_id;
            std::shared_ptr<RpcMessage> rpc_message;
            auto                        wait_time = std::chrono::steady_clock().now() + timeout;
            do
            {
                // Compute timeout
                auto now       = std::chrono::steady_clock().now();
                auto left_time = std::chrono::duration_cast<std::chrono::milliseconds>(wait_time - now);
                if (left_time.count() >= 0)
                {
                    // Wait for a message
                    ret = m_results_queue.pop(rpc_message, static_cast<unsigned int>(left_time.count()));
                    if (ret)
                    {
                        // Check id
                        if (rpc_message->unique_id != expected_id.str())
                        {
                            // Wrong message
                            rpc_message.reset();
                        }
                    }
                }
                else
                {
                    ret = false;
                }
            } while (ret && !rpc_message);

            // Extract response
            if (rpc_message)
            {
                rpc_frame.Swap(rpc_message->rpc_frame);
                response.Swap(rpc_message->payload);
                error.clear();
                message.clear();
                if (!rpc_message->error.IsNull())
                {
                    error = rpc_message->error.GetString();
                }
                if (!rpc_message->message.IsNull())
                {
                    message = rpc_message->message.GetString();
                }
                rpc_message.reset();
            }
            else
            {
                ret = false;
            }
        }

        // Update transaction id
        m_transaction_id++;
    }

    return ret;
}

/** @copydoc void IRpc::registerListener(IListener&) */
void RpcBase::registerListener(IRpc::IListener& listener)
{
    m_rpc_listener = &listener;
}

/** @copydoc void IRpc::registerSpy(ISpy&) */
void RpcBase::registerSpy(IRpc::ISpy& spy)
{
    m_spies.insert(&spy);
}

/** @copydoc void IRpc::unregisterSpy(ISpy&) */
void RpcBase::unregisterSpy(IRpc::ISpy& spy)
{
    m_spies.erase(&spy);
}

// RpcBase interface

/** @brief Process an incoming RPC request */
void RpcBase::processIncomingRequest(std::shared_ptr<RpcMessage>& rpc_message)
{
    // Notify call
    rapidjson::Document response(rapidjson::kObjectType);
    std::string         error;
    std::string         error_code;
    if (m_rpc_listener->rpcCallReceived(rpc_message->action, rpc_message->payload, response, error_code, error))
    {
        // Serialize message
        rapidjson::StringBuffer                    buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        writer.SetMaxDecimalPlaces(1); // OCPP decimals have 1 digit precision
        response.Accept(writer);

        std::stringstream serialized_message;
        serialized_message << "[";
        serialized_message << CALLRESULT << ", ";
        serialized_message << "\"" << rpc_message->unique_id << "\", ";
        serialized_message << buffer.GetString();
        serialized_message << "]";

        // Send message
        std::string msg = serialized_message.str();
        send(msg);
    }
    else
    {
        // Error
        if (!error_code.empty())
        {
            sendCallError(rpc_message->unique_id, error_code.c_str(), error);
        }
    }

    // Free resources
    rpc_message.reset();
}

/** @brief Start RPC operations */
void RpcBase::start()
{
    // Check if already started
    if (!m_rx_thread && !m_rpc_owner)
    {
        // Initialize transaction id sequence
        m_transaction_id = std::rand();

        // Start queues
        m_requests_queue.setEnable(true);
        m_results_queue.setEnable(true);

        // Check if a pool has been configured
        if (m_pool)
        {
            // Initialize message owner structure
            m_rpc_owner = std::make_shared<RpcMessageOwner>(*this);
        }
        else
        {
            // Start reception thread
            m_rx_thread = new std::thread(std::bind(&RpcBase::rxThread, this));
        }
    }
}

/** @brief Stop RPC operations */
void RpcBase::stop()
{
    // Check if already started
    if (m_rx_thread || m_rpc_owner)
    {
        // Stop queues
        m_results_queue.setEnable(false);
        m_requests_queue.setEnable(false);

        // Check if a pool has been configured
        if (m_pool)
        {
            // Disable owner
            m_rpc_owner->lock.lock();
            m_rpc_owner->is_operational = false;
            m_rpc_owner->lock.unlock();

            // Release message owner structure
            m_rpc_owner.reset();
        }
        else
        {
            // Stop reception thread
            m_rx_thread->join();
            delete m_rx_thread;
            m_rx_thread = nullptr;
        }

        // Flush queues
        m_requests_queue.clear();
        m_results_queue.clear();
    }
}

/** @brief Process the websocket disconnection event */
void RpcBase::processDisconnected()
{
    // Disable queues
    m_requests_queue.setEnable(false);
    m_results_queue.setEnable(false);

    // Check if a pool has been configured
    if (m_pool)
    {
        // Disable owner
        m_rpc_owner->lock.lock();
        m_rpc_owner->is_operational = false;
        m_rpc_owner->lock.unlock();
    }
}

/** @brief Process received data */
void RpcBase::processReceivedData(const void* data, size_t size)
{
    // Decode received data
    std::string received_data(reinterpret_cast<const char*>(data), size);
    for (ISpy* spy : m_spies)
    {
        spy->rcpMessageReceived(received_data);
    }

    // RPC frame must be a JSON array
    bool                valid = false;
    rapidjson::Document rpc_frame;
    try
    {
        rpc_frame.Parse(received_data.c_str());
        valid = !rpc_frame.HasParseError();
    }
    catch (const std::exception&)
    {
    }
    if (valid && rpc_frame.IsArray() && (rpc_frame.Size() >= 3))
    {
        // Extract message type
        const rapidjson::Value& msg_type_value = rpc_frame[0];
        if (msg_type_value.IsUint())
        {
            // Check message type
            MessageType  msg_type     = MessageType::INVALID;
            unsigned int msg_type_int = msg_type_value.GetUint();
            switch (msg_type_int)
            {
                case static_cast<unsigned int>(MessageType::CALL):
                    msg_type = MessageType::CALL;
                    valid    = (rpc_frame.Size() == 4u);
                    break;
                case static_cast<unsigned int>(MessageType::CALLRESULT):
                    msg_type = MessageType::CALLRESULT;
                    valid    = (rpc_frame.Size() == 3u);
                    break;
                case static_cast<unsigned int>(MessageType::CALLERROR):
                    msg_type = MessageType::CALLERROR;
                    valid    = (rpc_frame.Size() == 5u);
                    break;
                default:
                    // Unknown type
                    valid = false;
                    break;
            }
            if (valid)
            {
                // Extract unique identifier
                const rapidjson::Value& unique_id_value = rpc_frame[1];
                if (unique_id_value.IsString())
                {
                    // Decode message
                    std::string unique_id = unique_id_value.GetString();
                    switch (msg_type)
                    {
                        case MessageType::CALL:
                            valid = decodeCall(unique_id, rpc_frame, rpc_frame[2], rpc_frame[3]);
                            break;
                        case MessageType::CALLRESULT:
                            valid = decodeCallResult(unique_id, rpc_frame, rpc_frame[2]);
                            break;
                        case MessageType::CALLERROR:
                        default:
                            valid = decodeCallError(unique_id, rpc_frame, rpc_frame[2], rpc_frame[3], rpc_frame[4]);
                            break;
                    }
                    if (!valid)
                    {
                        sendCallError("", RPC_ERROR_PROTOCOL, "");
                    }
                }
                else
                {
                    sendCallError("", RPC_ERROR_PROTOCOL, "");
                }
            }
            else
            {
                sendCallError("", RPC_ERROR_PROTOCOL, "");
            }
        }
        else
        {
            sendCallError("", RPC_ERROR_PROTOCOL, "");
        }
    }
    else
    {
        sendCallError("", RPC_ERROR_PROTOCOL, "");
    }
}

/** @brief Send a message throug the websocket connection */
bool RpcBase::send(const std::string& msg)
{
    // Notify spy
    for (ISpy* spy : m_spies)
    {
        spy->rcpMessageSent(msg);
    }

    // Send message
    return doSend(msg);
}

/** @brief Decode a CALL message */
bool RpcBase::decodeCall(const std::string&      unique_id,
                         rapidjson::Document&    rpc_frame,
                         const rapidjson::Value& action,
                         rapidjson::Value&       payload)
{
    bool ret = false;

    // Check types
    if (action.IsString() && payload.IsObject())
    {
        // Create request
        auto msg = std::make_shared<RpcMessage>(unique_id, action.GetString(), rpc_frame, payload);

        // Check if a pool has been configured
        if (m_pool)
        {
            // Add request to the pool
            msg->owner = m_rpc_owner;
            m_pool->getRequestQueue().push(std::move(msg));
        }
        else
        {
            // Add request to the queue
            m_requests_queue.push(std::move(msg));
        }

        ret = true;
    }

    return ret;
}

/** @brief Decode a CALLRESULT message */
bool RpcBase::decodeCallResult(const std::string& unique_id, rapidjson::Document& rpc_frame, rapidjson::Value& payload)
{
    bool ret = false;

    // Check types
    if (payload.IsObject())
    {
        // Add result to the queue
        auto msg = std::make_shared<RpcMessage>(unique_id, rpc_frame, payload);
        m_results_queue.push(std::move(msg));

        ret = true;
    }

    return ret;
}

/** @brief Decode a CALLERROR message */
bool RpcBase::decodeCallError(const std::string&   unique_id,
                              rapidjson::Document& rpc_frame,
                              rapidjson::Value&    error,
                              rapidjson::Value&    message,
                              rapidjson::Value&    payload)
{
    bool ret = false;

    // Check types
    if (error.IsString() && message.IsString() && payload.IsObject())
    {
        // Add error to the queue
        auto msg = std::make_shared<RpcMessage>(unique_id, rpc_frame, payload, &error, &message);
        m_results_queue.push(std::move(msg));

        ret = true;
    }

    return ret;
}

/** @brief Send a CALLERROR message */
void RpcBase::sendCallError(const std::string& unique_id, const char* error, const std::string& message)
{
    // Serialize message
    std::stringstream serialized_message;
    serialized_message << "[";
    serialized_message << CALLERROR << ", ";
    serialized_message << "\"" << unique_id << "\", ";
    serialized_message << "\"" << error << "\", ";
    serialized_message << "\"" << message << "\", ";
    serialized_message << "{}";
    serialized_message << "]";

    // Send message
    std::string msg = serialized_message.str();
    send(msg);
}

/** @brief Reception thread */
void RpcBase::rxThread()
{
    // Thread loop
    std::shared_ptr<RpcMessage> rpc_message;
    while (m_requests_queue.pop(rpc_message))
    {
        // Process request
        processIncomingRequest(rpc_message);
    }
}

} // namespace rpc
} // namespace ocpp
