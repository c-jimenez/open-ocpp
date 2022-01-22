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

#include "IWebsocketClient.h"
#include "Queue.h"
#include "RpcBase.h"

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

namespace ocpp
{
namespace rpc
{

/** @brief RPC client implementation */
class RpcClient : public RpcBase, public ocpp::websockets::IWebsocketClient::IListener
{
  public:
    // Forward declaration
    class IListener;

    /** @brief Constructor */
    RpcClient(ocpp::websockets::IWebsocketClient& websocket, const std::string& protocol);

    /** @brief Destructor */
    virtual ~RpcClient();

    /**
     * @brief Start the client
     * @param url URL to connect to
     * @param credentials Credentials to use
     * @param connect_timeout Connection timeout in ms
     * @param retry_interval Retry interval in ms when connection cannot be established (0 = no retry)
     * @param ping_interval Interval between 2 websocket PING messages when the socket is idle
     * @return true if the client has been started, false otherwise
     */
    bool start(const std::string&                                     url,
               const ocpp::websockets::IWebsocketClient::Credentials& credentials,
               std::chrono::milliseconds                              connect_timeout = std::chrono::seconds(5),
               std::chrono::milliseconds                              retry_interval  = std::chrono::seconds(5),
               std::chrono::milliseconds                              ping_interval   = std::chrono::seconds(5));

    /**
     * @brief Stop the client
     * @return true if the client has been stopped, false otherwise
     */
    bool stop();

    /** @brief Register a listener to RPC client events
     *  @param listener Listener object
     */
    void registerClientListener(IListener& listener);

    // IRpc interface

    /** @copydoc bool IRpc::isConnected() */
    bool isConnected() const override;

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

    /** @brief Interface for the RPC clients listeners */
    class IListener
    {
      public:
        /** @brief Destructor */
        virtual ~IListener() { }

        /** @brief Called when connection is successfull */
        virtual void rpcClientConnected() = 0;

        /** @brief Called when connection failed */
        virtual void rpcClientFailed() = 0;
    };

  protected:
    /** @copydoc bool RpcBase::doSend(const std::string&) */
    bool doSend(const std::string& msg) override;

  private:
    /** @brief Protocol version */
    const std::string m_protocol;
    /** @brief Websocket connexion */
    ocpp::websockets::IWebsocketClient& m_websocket;
    /** @brief Listener */
    IListener* m_listener;
    /** @brief Started state */
    bool m_started;
};

} // namespace rpc
} // namespace ocpp

#endif // RPCCLIENT_H
