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

#ifndef OPENOCPP_RPCBASE_H
#define OPENOCPP_RPCBASE_H

#include "IRpc.h"
#include "Queue.h"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_set>

namespace ocpp
{
namespace rpc
{

class RpcPool;

/** @brief Base class for RPC implementations */
class RpcBase : public IRpc
{
  public:
    /** @brief Constructor */
    RpcBase(RpcPool* pool = nullptr);

    /** @brief Destructor */
    virtual ~RpcBase();

    // IRpc interface

    /** @copydoc bool IRpc::call(const std::string&, const rapidjson::Document&, rapidjson::Document&, rapidjson::Value&,
     *                           std::string&, std::string&, std::chrono::milliseconds) */
    bool call(const std::string&         action,
              const rapidjson::Document& payload,
              rapidjson::Document&       rpc_frame,
              rapidjson::Value&          response,
              std::string&               error,
              std::string&               message,
              std::chrono::milliseconds  timeout = std::chrono::seconds(2)) override;

    /** @copydoc void IRpc::registerListener(IListener&) */
    void registerListener(IRpc::IListener& listener) override;

    /** @copydoc void IRpc::registerSpy(ISpy&) */
    void registerSpy(IRpc::ISpy& spy) override;

    /** @copydoc void IRpc::unregisterSpy(ISpy&) */
    void unregisterSpy(IRpc::ISpy& spy) override;

    /** @brief RPC message owner */
    struct RpcMessageOwner
    {
        /** @brief Constructor */
        RpcMessageOwner(RpcBase& _rpc_base) : rpc_base(_rpc_base), is_operational(true) { }
        /** @brief Lock associated to the owner */
        std::mutex lock;
        /** @brief Underlying RPC handler */
        RpcBase& rpc_base;
        /** @brief Indicate if the owner is operational to handle the message */
        bool is_operational;
    };

    /** @brief RPC message */
    struct RpcMessage
    {
        RpcMessage(const std::string& _unique_id, const char* _action, rapidjson::Document& _rpc_frame, rapidjson::Value& _payload)
            : unique_id(_unique_id), action(_action), rpc_frame(std::move(_rpc_frame)), payload(), error(), message()
        {
            payload.Swap(_payload);
        }
        RpcMessage(const std::string&   _unique_id,
                   rapidjson::Document& _rpc_frame,
                   rapidjson::Value&    _payload,
                   rapidjson::Value*    _error   = nullptr,
                   rapidjson::Value*    _message = nullptr)
            : unique_id(_unique_id), action(), rpc_frame(std::move(_rpc_frame)), payload(), error(), message()
        {
            payload.Swap(_payload);
            if (_error)
            {
                error.Swap(*_error);
            }
            if (_message)
            {
                message.Swap(*_message);
            }
        }
        const std::string              unique_id;
        const std::string              action;
        rapidjson::Document            rpc_frame;
        rapidjson::Value               payload;
        rapidjson::Value               error;
        rapidjson::Value               message;
        std::weak_ptr<RpcMessageOwner> owner;
    };

    /** 
     * @brief Process an incoming RPC request 
     * @param rpc_message RPC request to process
     */
    void processIncomingRequest(std::shared_ptr<RpcMessage>& rpc_message);

  protected:
    /** @brief Start RPC operations */
    void start();
    /** @brief Stop RPC operations */
    void stop();
    /** @brief Process the websocket disconnection event */
    void processDisconnected();
    /** @brief Process received data */
    void processReceivedData(const void* data, size_t size);
    /** @brief Get the RPC listener */
    IRpc::IListener* rpcListener() { return m_rpc_listener; }

    /**
     * @brief Send data through the websocket connection
     * @param msg Message to send
     * @return true if the message has been sent, false otherwise
     */
    virtual bool doSend(const std::string& msg) = 0;

  private:
    /** @brief Message types */
    enum class MessageType : unsigned int
    {
        CALL       = 2,
        CALLRESULT = 3,
        CALLERROR  = 4,
        INVALID    = 5
    };

    /** @brief Associated RPC pool */
    RpcPool* m_pool;
    /** @brief RPC listener */
    IRpc::IListener* m_rpc_listener;
    /** @brief RPC spies */
    std::unordered_set<IRpc::ISpy*> m_spies;
    /** @brief Transaction id */
    int m_transaction_id;
    /** @brief Mutex for concurrent call access */
    std::mutex m_call_mutex;
    /** @brief Queue for incomming call requests */
    ocpp::helpers::Queue<std::shared_ptr<RpcMessage>> m_requests_queue;
    /** @brief Queue for incomming call results */
    ocpp::helpers::Queue<std::shared_ptr<RpcMessage>> m_results_queue;
    /** @brief Reception thread */
    std::thread* m_rx_thread;
    /** @brief RPC message owner */
    std::shared_ptr<RpcMessageOwner> m_rpc_owner;

    /** @brief Send a message through the websocket connection */
    bool send(const std::string& msg);

    /** @brief Decode a CALL message */
    bool decodeCall(const std::string&      unique_id,
                    rapidjson::Document&    rpc_frame,
                    const rapidjson::Value& action,
                    rapidjson::Value&       payload);

    /** @brief Decode a CALLRESULT message */
    bool decodeCallResult(const std::string& unique_id, rapidjson::Document& rpc_frame, rapidjson::Value& payload);

    /** @brief Decode a CALLERROR message */
    bool decodeCallError(const std::string&   unique_id,
                         rapidjson::Document& rpc_frame,
                         rapidjson::Value&    error,
                         rapidjson::Value&    message,
                         rapidjson::Value&    payload);

    /** @brief Send a CALLERROR message */
    void sendCallError(const std::string& unique_id, const char* error, const std::string& message);

    /** @brief Reception thread */
    void rxThread();
};

} // namespace rpc
} // namespace ocpp

#endif // OPENOCPP_RPCBASE_H
