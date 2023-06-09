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

#include "RpcClient.h"

#include <functional>
#include <sstream>

using namespace ocpp::websockets;

namespace ocpp
{
namespace rpc
{

/** @brief Constructor */
RpcClient::RpcClient(ocpp::websockets::IWebsocketClient& websocket, const std::string& protocol, RpcPool* pool)
    : RpcBase(pool), m_protocol(protocol), m_websocket(websocket), m_listener(nullptr), m_started(false), m_stop_mutex()
{
    m_websocket.registerListener(*this);
}

/** @brief Destructor */
RpcClient::~RpcClient()
{
    stop();
}

/** @brief Start the client */
bool RpcClient::start(const std::string&                                     url,
                      const ocpp::websockets::IWebsocketClient::Credentials& credentials,
                      std::chrono::milliseconds                              connect_timeout,
                      std::chrono::milliseconds                              retry_interval,
                      std::chrono::milliseconds                              ping_interval)
{
    bool ret = false;

    // Check if already started
    if (!m_started && m_listener && rpcListener())
    {
        // Connect to websocket
        ret = m_websocket.connect(url, m_protocol, credentials, connect_timeout, retry_interval, ping_interval);
        if (ret)
        {
            // Start processing
            RpcBase::start();
            m_started = true;
        }
    }

    return ret;
}

/** @brief Stop the client */
bool RpcClient::stop()
{
    bool ret = false;

    // Check if someone is already stopping the client
    // May happen in local controller mode when disconnection
    // can be triggered from central system side and charge point side
    if (m_stop_mutex.try_lock())
    {
        // Check if already started
        if (m_started)
        {
            // Disconnect from websocket
            ret = m_websocket.disconnect();

            // Stop processing
            RpcBase::stop();
            m_started = false;
        }
        m_stop_mutex.unlock();
    }

    return ret;
}

/** @brief Register a listener to RPC client events */
void RpcClient::registerClientListener(IListener& listener)
{
    m_listener = &listener;
}

// IRpc interface

/** @copydoc bool IRpc::isConnected() */
bool RpcClient::isConnected() const
{
    return m_websocket.isConnected();
}

// IWebsocketClient::IListener interface

/** @copydoc void IWebsocketClient::IListener::wsClientConnected() */
void RpcClient::wsClientConnected()
{
    m_listener->rpcClientConnected();
}

/** @copydoc void IWebsocketClient::IListener::wsClientFailed() */
void RpcClient::wsClientFailed()
{
    m_listener->rpcClientFailed();
}

/** @copydoc void IWebsocketClient::IListener::wsClientDisconnected() */
void RpcClient::wsClientDisconnected()
{
    rpcListener()->rpcDisconnected();
}

/** @copydoc void IWebsocketClient::IListener::wsClientError() */
void RpcClient::wsClientError()
{
    rpcListener()->rpcError();
}

/** @copydoc void IWebsocketClient::IListener::wsClientDataReceived(const void*, size_t) */
void RpcClient::wsClientDataReceived(const void* data, size_t size)
{
    // Process data
    processReceivedData(data, size);
}

// RpcBase interface

/** @copydoc bool RpcBase::doSend(const std::string&) */
bool RpcClient::doSend(const std::string& msg)
{
    // Send message
    return m_websocket.send(msg.c_str(), msg.size());
}

} // namespace rpc
} // namespace ocpp
