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

#ifndef OPENOCPP_IWEBSOCKETSERVER_H
#define OPENOCPP_IWEBSOCKETSERVER_H

#include <chrono>
#include <memory>
#include <string>

namespace ocpp
{
namespace websockets
{

/** @brief Interface for websocket server implementations */
class IWebsocketServer
{
  public:
    // Forward declarations
    class IListener;
    class IClient;
    struct Credentials;

    /** @brief Destructor */
    virtual ~IWebsocketServer() { }

    /**
     * @brief Start the server
     * @param url URL to listen to
     * @param protocol Name of the protocol to use
     * @param credentials Credentials to use
     * @param ping_interval Interval between 2 websocket PING messages when the socket is idle
     * @return true if the server has been started, false otherwise
     */
    virtual bool start(const std::string&        url,
                       const std::string&        protocol,
                       const Credentials&        credentials,
                       std::chrono::milliseconds ping_interval = std::chrono::seconds(5)) = 0;

    /**
     * @brief Stop the server
     * @return true if the server has been stopped, false otherwise
     */
    virtual bool stop() = 0;

    /**
     * @brief Register a listener to the websocket events
     * @param listener Listener object
     */
    virtual void registerListener(IListener& listener) = 0;

    /** @brief Interface for the websocket server listeners */
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
        virtual bool wsAcceptConnection(const char* ip_address) = 0;

        /**
         * @brief Called to check the user credentials for HTTP basic authentication
         * @param uri Requested URI
         * @param user User name
         * @param password Password
         * @return true if the credentials are valid, false otherwise
         */
        virtual bool wsCheckCredentials(const char* uri, const std::string& user, const std::string& password) = 0;

        /**
         * @brief Called when connection is successfull
         * @param uri Requested URI
         * @param client Client connection
         */
        virtual void wsClientConnected(const char* uri, std::shared_ptr<IClient> client) = 0;

        /**
         * @brief Called when connection fails to established
         * @param ip_address IP address of the client
         */
        virtual void wsClientFailedToConnect(const char* ip_address) = 0;

        /** @brief Called on critical error */
        virtual void wsServerError() = 0;
    };

    /** @brief Interface for websocket client connection */
    class IClient
    {
      public:
        // Forward declarations
        class IListener;

        /** @brief Destructor */
        virtual ~IClient() { }

        /**
         * @brief Get the IP address of the client
         * @return IP address of the client
         */
        virtual const std::string& ipAddress() const = 0;

        /**
         * @brief Disconnect the client
         * @param notify_disconnected Indicate if the listener must be notified owhen disconnected
         * @return true if the disconnection is successfull, false otherwise
         */
        virtual bool disconnect(bool notify_disconnected = true) = 0;

        /**
         * @brief Indicate if the client is connected
         * @return true if the client is connected, false otherwise
         */
        virtual bool isConnected() = 0;

        /**
         * @brief Send data through the websocket connection
         * @param buffer Buffer containing the data to send
         * @param size Size of the buffer in bytes
         * @return true is the data has been sent, false otherwise
         */
        virtual bool send(const void* data, size_t size) = 0;

        /**
         * @brief Register a listener to the websocket events
         * @param listener Listener object
         */
        virtual void registerListener(IListener& listener) = 0;

        /** @brief Interface for the websocket clients listeners */
        class IListener
        {
          public:
            /** @brief Destructor */
            virtual ~IListener() { }

            /** @brief Called when connection is lost */
            virtual void wsClientDisconnected() = 0;

            /** @brief Called when a critical error occured */
            virtual void wsClientError() = 0;

            /**
             * @brief Call when data has been received
             * @param buffer Buffer containing received data
             * @param size Size of the buffer in bytes
             */
            virtual void wsClientDataReceived(const void* data, size_t size) = 0;
        };
    };

    /** @brief Connection credentials */
    struct Credentials
    {
        // Basic authentication

        /** @bool Enable HTTP basic authentication */
        bool http_basic_authent;

        // TLS connections (wss URLs only)

        /** @brief Cipher list for TLSv1.2 connections, leave empty for default
         *         (OpenSSL format, default = system dependent) */
        std::string tls12_cipher_list;
        /** @brief Cipher list for TLSv1.3 connections, leave empty for default
         *         (OpenSSL format, default = system dependent) */
        std::string tls13_cipher_list;
        /** @brief ECDH curve, leave empty for default
         *         (OpenSSL format, default = system dependent) */
        std::string ecdh_curve;
        /** @brief Indicate if the below certificates parameters are path to PEM encoded certificate files (false)
         *         or if they contain directly PEM encoded certificates (true) */
        bool encoded_pem_certificates;
        /** @brief Server certificate */
        std::string server_certificate;
        /** @brief Server certificate's private key */
        std::string server_certificate_private_key;
        /** @brief Server certificate's private key passphrase */
        std::string server_certificate_private_key_passphrase;
        /** @brief Certification Authority signing chain for the server certificate */
        std::string server_certificate_ca;
        /** @bool Enable client authentication using certificate */
        bool client_certificate_authent;
    };
};

} // namespace websockets
} // namespace ocpp

#endif // OPENOCPP_IWEBSOCKETSERVER_H
