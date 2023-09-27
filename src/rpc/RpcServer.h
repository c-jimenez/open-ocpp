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

#ifndef OPENOCPP_RPCSERVER_H
#define OPENOCPP_RPCSERVER_H

#include "IWebsocketServer.h"
#include "Queue.h"
#include "RpcBase.h"
#include "RpcPool.h"

namespace ocpp
{
namespace rpc
{

/** @brief RPC server implementation */
class RpcServer : public ocpp::websockets::IWebsocketServer::IListener
{
  public:
    // Forward declarations
    class Client;
    class IListener;

    /** @brief Constructor */
    RpcServer(ocpp::websockets::IWebsocketServer& websocket, const std::string& protocol);

    /** @brief Destructor */
    virtual ~RpcServer();

    /**
     * @brief Start the server
     * @param url URL to listen to
     * @param credentials Credentials to use
     * @param ping_interval Interval between 2 websocket PING messages when the socket is idle
     * @param incoming_req_thread_pool_size Size of the thread pool to handle incoming requests from the clients 
     * @return true if the server has been started, false otherwise
     */
    bool start(const std::string&                                     url,
               const ocpp::websockets::IWebsocketServer::Credentials& credentials,
               std::chrono::milliseconds                              ping_interval                 = std::chrono::seconds(5),
               unsigned int                                           incoming_req_thread_pool_size = 10u);

    /**
     * @brief Stop the server
     * @return true if the server has been stopped, false otherwise
     */
    bool stop();

    /** @brief Register a listener to RPC server events
     *  @param listener Listener object
     */
    void registerServerListener(IListener& listener);

    // IWebsocketServer::IListener interface

    /** @copydoc bool IWebsocketServer::IListener::wsAcceptConnection(const char*) */
    bool wsAcceptConnection(const char* ip_address) override;

    /** @copydoc bool IWebsocketServer::IListener::wsCheckCredentials(const char*, const std::string&, const std::string&) */
    bool wsCheckCredentials(const char* uri, const std::string& user, const std::string& password) override;

    /** @copydoc void IWebsocketServer::IListener::wsClientConnected(const char*, std::shared_ptr<IClient>) */
    void wsClientConnected(const char* uri, std::shared_ptr<ocpp::websockets::IWebsocketServer::IClient> client) override;

    /** @copydoc void IWebsocketServer::IListener::wsClientFailedToConnect(const char*) */
    void wsClientFailedToConnect(const char* ip_address) override;

    /** @copydoc void IWebsocketServer::IListener::wsServerError() */
    void wsServerError() override;

    /** @brief Interface for the RPC server listeners */
    class IListener
    {
      public:
        /** @brief Destructor */
        virtual ~IListener() { }

        /**
         * @brief Called to accept an incoming connection
         * @param ip_address IP address of the client
         * @return true if the incoming connection must be accepted, false otherwise
         */
        virtual bool rpcAcceptConnection(const char* ip_address) = 0;

        /**
         * @brief Called to check the user credentials for HTTP basic authentication
         * @param chargepoint_id Charge Point identifier
         * @param user User name
         * @param password Password
         * @return true if the credentials are valid, false otherwise
         */
        virtual bool rpcCheckCredentials(const std::string& chargepoint_id, const std::string& user, const std::string& password) = 0;

        /**
         * @brief Called when connection is successfull
         * @param chargepoint_id Charge Point identifier
         * @param client Client connection
         */
        virtual void rpcClientConnected(const std::string& chargepoint_id, std::shared_ptr<Client> client) = 0;

        /**
         * @brief Called when connection fails to established
         * @param ip_address IP address of the client
         */
        virtual void rpcClientFailedToConnect(const char* ip_address) = 0;

        /** @brief Called on critical error */
        virtual void rpcServerError() = 0;
    };

    /** @brief RPC server's client connection */
    class Client : public RpcBase, public ocpp::websockets::IWebsocketServer::IClient::IListener
    {
      public:
        /** @brief Constructor */
        Client(std::shared_ptr<ocpp::websockets::IWebsocketServer::IClient> websocket, RpcPool& pool);
        /** @brief Destructor */
        virtual ~Client();

        /**
         * @brief Get the IP address of the client
         * @return IP address of the client
         */
        const std::string& ipAddress() const;

        /**
         * @brief Disconnect the client
         * @param notify_disconnected Indicate if the listener must be notified when disconnected
         * @return true if the client has been disconnected, false otherwise
         */
        bool disconnect(bool notify_disconnected = true);

        // IRpc interface

        /** @copydoc bool IRpc::isConnected() */
        bool isConnected() const override;

        // IWebsocketServer::IClient::IListener interface

        /** @brief void IWebsocketServer::IClient::IListener::wsClientDisconnected() */
        void wsClientDisconnected() override;

        /** @brief void IWebsocketServer::IClient::IListener::wsClientError() */
        void wsClientError() override;

        /** @brief void IWebsocketServer::IClient::IListener::wsClientDataReceived(const void*, size_t) */
        void wsClientDataReceived(const void* data, size_t size) override;

      protected:
        /** @copydoc bool RpcBase::doSend(const std::string&) */
        bool doSend(const std::string& msg) override;

      private:
        /** @brief Websocket connection */
        std::shared_ptr<ocpp::websockets::IWebsocketServer::IClient> m_websocket;
    };

  private:
    /** @brief Protocol version */
    const std::string m_protocol;
    /** @brief Websocket connection */
    ocpp::websockets::IWebsocketServer& m_websocket;
    /** @brief RPC message handler pool */
    RpcPool m_pool;
    /** @brief Listener */
    IListener* m_listener;
    /** @brief Started state */
    bool m_started;
};

} // namespace rpc
} // namespace ocpp

#endif // OPENOCPP_RPCSERVER_H
