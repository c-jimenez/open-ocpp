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

#ifndef RPCCLIENT_H
#define RPCCLIENT_H

#include "IRpcClient.h"
#include "IWebsocketClient.h"
#include "Queue.h"

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

namespace ocpp
{
namespace rpc
{

/** @brief RPC client implementation */
class RpcClient : public IRpcClient, public ocpp::websockets::IWebsocketClient::IListener
{
  public:
    /** @brief Constructor */
    RpcClient(ocpp::websockets::IWebsocketClient& websocket, const std::string& protocol, int initial_transaction_id = 0);

    /** @brief Destructor */
    virtual ~RpcClient();

    // IRpcClient interface

    /** @copydoc bool IRpcClient::start(const std::string&, const std::string&, const Credentials&,
     *                                  std::chrono::milliseconds, std::chrono::milliseconds, std::chrono::milliseconds) */
    bool start(const std::string&                                     url,
               const ocpp::websockets::IWebsocketClient::Credentials& credentials,
               std::chrono::milliseconds                              connect_timeout = std::chrono::seconds(5),
               std::chrono::milliseconds                              retry_interval  = std::chrono::seconds(5),
               std::chrono::milliseconds                              ping_interval   = std::chrono::seconds(5)) override;

    /** @copydoc bool IRpcClient::stop() */
    bool stop() override;

    /** @copydoc bool IRpcClient::isConnected() */
    bool isConnected() const override;

    /** @copydoc bool IRpcClient::call(const std::string&, const rapidjson::Document&, rapidjson::Document&, unsigned int) */
    bool call(const std::string&         action,
              const rapidjson::Document& payload,
              rapidjson::Document&       response,
              unsigned int               timeout = 2000u) override;

    /** @copydoc void IRpcClient::registerListener(IRpcClientListener&) */
    void registerListener(IRpcClientListener& listener) override;

    /** @copydoc void IRpcClient::registerSpy(IRpcClientSpy&) */
    void registerSpy(IRpcClientSpy& spy) override;

    // IWebsocketClientListener interface

    /** @copydoc void IWebsocketClientListener::wsClientConnected() */
    void wsClientConnected() override;

    /** @copydoc void IWebsocketClientListener::wsClientFailed() */
    void wsClientFailed() override;

    /** @copydoc void IWebsocketClientListener::wsClientDisconnected() */
    void wsClientDisconnected() override;

    /** @copydoc void IWebsocketClientListener::wsClientError() */
    void wsClientError() override;

    /** @copydoc void IWebsocketClientListener::wsClientDataReceived(const void*, size_t) */
    void wsClientDataReceived(const void* data, size_t size) override;

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

    /** @brief Protocol version */
    const std::string m_protocol;
    /** @brief Websocket connexion */
    ocpp::websockets::IWebsocketClient& m_websocket;
    /** @brief Listener */
    IRpcClientListener* m_listener;
    /** @brief Spies */
    std::vector<IRpcClientSpy*> m_spies;
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

    /** @brief Send a message throug the websocket connection */
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

#endif // RPCCLIENT_H
