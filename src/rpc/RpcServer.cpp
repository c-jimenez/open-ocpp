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

#include "RpcServer.h"

#include <filesystem>

namespace ocpp
{
namespace rpc
{

/** @brief Constructor */
RpcServer::RpcServer(ocpp::websockets::IWebsocketServer& websocket, const std::string& protocol)
    : m_protocol(protocol), m_websocket(websocket), m_listener(nullptr), m_started(false)
{
    m_websocket.registerListener(*this);
}

/** @brief Destructor */
RpcServer::~RpcServer()
{
    stop();
}

/** @brief Start the server */
bool RpcServer::start(const std::string&                                     url,
                      const ocpp::websockets::IWebsocketServer::Credentials& credentials,
                      std::chrono::milliseconds                              ping_interval)

{
    bool ret = false;

    // Check if already started
    if (!m_started && m_listener)
    {
        // Start websocket server
        ret = m_websocket.start(url, m_protocol, credentials, ping_interval);
        if (ret)
        {
            m_started = true;
        }
    }

    return ret;
}

/** @brief Stop the server */
bool RpcServer::stop()
{
    bool ret = false;

    // Check if already started
    if (m_started)
    {
        // Disconnect from websocket
        ret       = m_websocket.stop();
        m_started = false;
    }

    return ret;
}

/** @brief Register a listener to RPC server events */
void RpcServer::registerServerListener(IListener& listener)
{
    m_listener = &listener;
}

// IWebsocketServer::IListener interface

/** @copydoc bool IWebsocketServer::IListener::wsCheckCredentials(const char*, const std::string&, const std::string&) */
bool RpcServer::wsCheckCredentials(const char* uri, const std::string& user, const std::string& password)
{
    // Extract Charge Point identifier from URI
    std::filesystem::path uri_path(uri);
    std::string           chargepoint_id = uri_path.filename();

    // Check credentials
    return m_listener->rpcCheckCredentials(chargepoint_id, user, password);
}

/** @copydoc void IWebsocketServer::IListener::wsClientConnected(const char*, std::shared_ptr<IClient>) */
void RpcServer::wsClientConnected(const char* uri, std::shared_ptr<ocpp::websockets::IWebsocketServer::IClient> client)
{
    // Extract Charge Point identifier from URI
    std::filesystem::path uri_path(uri);
    std::string           chargepoint_id = uri_path.filename();

    // Instanciate client
    std::shared_ptr<IClient> rpc_client(new IClient(client));

    // Notify connection
    m_listener->rpcClientConnected(chargepoint_id, rpc_client);
}

/** @copydoc void IWebsocketServer::IListener::wsServerError() */
void RpcServer::wsServerError()
{
    m_listener->rpcServerError();
}

/** @brief Constructor */
RpcServer::IClient::IClient(std::shared_ptr<ocpp::websockets::IWebsocketServer::IClient> websocket) : RpcBase(), m_websocket(websocket)
{
    // Start processing
    m_websocket->registerListener(*this);
    RpcBase::start();
}

/** @brief Destructor */
RpcServer::IClient::~IClient()
{
    // Disconnect from websocket
    m_websocket->disconnect();

    // Stop processing
    RpcBase::stop();
}

// IRpc interface

/** @copydoc bool IRpc::isConnected() */
bool RpcServer::IClient::isConnected() const
{
    return m_websocket->isConnected();
}

// IWebsocketServer::IClient::IListener interface

/** @brief void IWebsocketServer::IClient::IListener::wsClientDisconnected() */
void RpcServer::IClient::wsClientDisconnected()
{
    rpcListener()->rpcDisconnected();
}

/** @brief void IWebsocketServer::IClient::IListener::wsClientError() */
void RpcServer::IClient::wsClientError()
{
    rpcListener()->rpcError();
}

/** @brief void IWebsocketServer::IClient::IListener::wsClientDataReceived(const void*, size_t) */
void RpcServer::IClient::wsClientDataReceived(const void* data, size_t size)
{
    // Process data
    processReceivedData(data, size);
}

// RpxBase interface

/** @copydoc bool RpcBase::doSend(const std::string&) */
bool RpcServer::IClient::doSend(const std::string& msg)
{
    // Send message
    return m_websocket->send(msg.c_str(), msg.size());
}

} // namespace rpc
} // namespace ocpp
