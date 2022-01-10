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

#ifndef IWEBSOCKETCLIENT_H
#define IWEBSOCKETCLIENT_H

#include <string>

namespace ocpp
{
namespace websockets
{

/** @brief Interface for websocket client implementations */
class IWebsocketClient
{
  public:
    // Forward declarations
    class IWebsocketClientListener;
    struct Credentials;

    /** @brief Destructor */
    virtual ~IWebsocketClient() { }

    /**
     * @brief Connect the client to the specified URL
     * @param url URL to connect to
     * @param protocol Name of the protocol to use
     * @param credentials Credentials to use
     * @param connect_timeout Connection timeout in ms
     * @param retry_interval Retry interval in ms when connection cannot be established (0 = no retry)
     * @return true if the connexion process has been started, false otherwise
     */
    virtual bool connect(const std::string& url,
                         const std::string& protocol,
                         const Credentials& credentials,
                         unsigned int       connect_timeout = 5000u,
                         unsigned int       retry_interval  = 5000u) = 0;

    /**
     * @brief Disconnect the client
     * @return true if the disconnection is successfull, false otherwise
     */
    virtual bool disconnect() = 0;

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
    virtual void registerListener(IWebsocketClientListener& listener) = 0;

    /** @brief Interface for the websocket clients listeners */
    class IWebsocketClientListener
    {
      public:
        /** @brief Destructor */
        virtual ~IWebsocketClientListener() { }

        /**
         * @brief Called when connection is successfull
         */
        virtual void wsClientConnected() = 0;

        /**
         * @brief Called when connection failed
         */
        virtual void wsClientFailed() = 0;

        /**
         * @brief Called when connection is lost
         */
        virtual void wsClientDisconnected() = 0;

        /**
         * @brief Called when a critical error occured
         */
        virtual void wsClientError() = 0;

        /**
         * @brief Call when data has been received
         * @param buffer Buffer containing received data
         * @param size Size of the buffer in bytes
         */
        virtual void wsClientDataReceived(const void* data, size_t size) = 0;
    };

    /** @brief Connection credentials */
    struct Credentials
    {
        // Basic authentication

        /** @brief User name for HTTP basic authentication (not used if empty) */
        std::string user;
        /** @brief Password for HTTP basic authentication (not used if username is empty) */
        std::string password;

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
        /** @brief Allow TLS connections using self-signed certificates
         *         (Warning : enabling this feature is not recommended in production) */
        bool allow_selfsigned_certificates;
        /** @brief Allow TLS connections using expired certificates
         *         (Warning : enabling this feature is not recommended in production) */
        bool allow_expired_certificates;
        /** @brief Accept non trusted certificates for TLS connections
         *         (Warning : enabling this feature is not recommended in production) */
        bool accept_untrusted_certificates;
        /** @brief Skip server name check in certificates for TLS connections
         *         (Warning : enabling this feature is not recommended in production) */
        bool skip_server_name_check;
    };
};

} // namespace websockets
} // namespace ocpp

#endif // IWEBSOCKETCLIENT_H
