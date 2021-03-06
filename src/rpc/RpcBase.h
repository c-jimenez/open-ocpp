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

#ifndef RPCBASE_H
#define RPCBASE_H

#include "IRpc.h"
#include "Queue.h"

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

namespace ocpp
{
namespace rpc
{

/** @brief Base class for RPC implementations */
class RpcBase : public IRpc
{
  public:
    /** @brief Constructor */
    RpcBase();

    /** @brief Destructor */
    virtual ~RpcBase();

    // IRpc interface

    /** @copydoc bool IRpc::call(const std::string&, const rapidjson::Document&, rapidjson::Document&, std::chrono::milliseconds) */
    bool call(const std::string&         action,
              const rapidjson::Document& payload,
              rapidjson::Document&       response,
              std::chrono::milliseconds  timeout = std::chrono::seconds(2)) override;

    /** @copydoc void IRpc::registerListener(IListener&) */
    void registerListener(IRpc::IListener& listener) override;

    /** @copydoc void IRpc::registerSpy(ISpy&) */
    void registerSpy(IRpc::ISpy& spy) override;

  protected:
    /** @brief Start RPC operations */
    void start();
    /** @brief Stop RPC operations */
    void stop();
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

    /** @brief RPC message */
    struct RpcMessage
    {
        RpcMessage(const std::string& _unique_id, const char* _action, const rapidjson::Value& _payload)
            : unique_id(_unique_id), action(_action), payload()
        {
            payload.CopyFrom(_payload, payload.GetAllocator());
        }
        RpcMessage(const std::string& _unique_id, const rapidjson::Value& _payload) : unique_id(_unique_id), action(), payload()
        {
            payload.CopyFrom(_payload, payload.GetAllocator());
        }
        const std::string   unique_id;
        const std::string   action;
        rapidjson::Document payload;
    };

    /** @brief RPC listener */
    IRpc::IListener* m_rpc_listener;
    /** @brief RPC spies */
    std::vector<IRpc::ISpy*> m_spies;
    /** @brief Transaction id */
    int m_transaction_id;
    /** @brief Mutex for concurrent call access */
    std::mutex m_call_mutex;
    /** @brief Queue for incomming call requests */
    ocpp::helpers::Queue<RpcMessage*> m_requests_queue;
    /** @brief Queue for incomming call results */
    ocpp::helpers::Queue<RpcMessage*> m_results_queue;
    /** @brief Reception thread */
    std::thread* m_rx_thread;

    /** @brief Send a message through the websocket connection */
    bool send(const std::string& msg);

    /** @brief Decode a CALL message */
    bool decodeCall(const std::string& unique_id, const rapidjson::Value& action, const rapidjson::Value& payload);

    /** @brief Decode a CALLRESULT message */
    bool decodeCallResult(const std::string& unique_id, const rapidjson::Value& payload);

    /** @brief Decode a CALLERROR message */
    bool decodeCallError(const std::string&      unique_id,
                         const rapidjson::Value& error,
                         const rapidjson::Value& message,
                         const rapidjson::Value& payload);

    /** @brief Send a CALLERROR message */
    void sendCallError(const std::string& unique_id, const char* error, const std::string& message);

    /** @brief Reception thread */
    void rxThread();
};

} // namespace rpc
} // namespace ocpp

#endif // RPCBASE_H
