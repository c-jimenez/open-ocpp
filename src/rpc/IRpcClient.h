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

#ifndef IRPCCLIENT_H
#define IRPCCLIENT_H

#include "IWebsocketClient.h"
#include "json.h"

#include <string>

namespace ocpp
{
namespace rpc
{

/** @brief Interface for RPC client implementations */
class IRpcClient
{
  public:
    // Forward declarations
    class IRpcClientListener;
    class IRpcClientSpy;

    /** @brief Destructor */
    virtual ~IRpcClient() { }

    /**
     * @brief Start the client
     * @param url URL to connect to
     * @param credentials Credentials to use
     * @param connect_timeout Connection timeout in ms
     * @param retry_interval Retry interval in ms when connection cannot be established (0 = no retry)
     * @param ping_interval Interval between 2 websocket PING messages when the socket is idle
     * @return true if the client has been started, false otherwise
     */
    virtual bool start(const std::string&                                     url,
                       const ocpp::websockets::IWebsocketClient::Credentials& credentials,
                       std::chrono::milliseconds                              connect_timeout = std::chrono::seconds(5),
                       std::chrono::milliseconds                              retry_interval  = std::chrono::seconds(5),
                       std::chrono::milliseconds                              ping_interval   = std::chrono::seconds(5)) = 0;

    /**
     * @brief Stop the client
     * @return true if the client has been stopped, false otherwise
     */
    virtual bool stop() = 0;

    /**
     * @brief Indicate if the connection is active
     * @return true if the connection is active, false otherwise
     */
    virtual bool isConnected() const = 0;

    /**
     * @brief Call a remote action and wait for its response
     * @param action Remote action
     * @param payload JSON payload for the action
     * @param response JSON response received
     * @param timeout Response timeout in ms
     * @return true is a response has been received, false otherwise
     */
    virtual bool call(const std::string&         action,
                      const rapidjson::Document& payload,
                      rapidjson::Document&       response,
                      unsigned int               timeout = 2000u) = 0;

    /**
     * @brief Register a listener to the client events
     * @param listener Listener object
     */
    virtual void registerListener(IRpcClientListener& listener) = 0;

    /**
     * @brief Register a spy to the client exchanges
     * @param spy Spy object
     */
    virtual void registerSpy(IRpcClientSpy& spy) = 0;

    /** @brief Interface for the RPC clients listeners */
    class IRpcClientListener
    {
      public:
        /** @brief Destructor */
        virtual ~IRpcClientListener() { }

        /**
         * @brief Called when connection is successfull
         */
        virtual void rpcClientConnected() = 0;

        /**
         * @brief Called when connection failed
         */
        virtual void rpcClientFailed() = 0;

        /**
         * @brief Called when connection is lost
         */
        virtual void rpcClientDisconnected() = 0;

        /**
         * @brief Called when a critical error occured
         */
        virtual void rpcClientError() = 0;

        /**
         * @brief Called when a CALL message has been received
         * @param action Action
         * @param payload JSON payload for the action
         * @param response JSON response to send
         * @param error_code Standard error code, set to nullptr if no error
         * @param error_msg Additionnal error message, empty if no error
         * @return true if the call is accepted, false otherwise
         */
        virtual bool rpcClientCallReceived(const std::string&      action,
                                           const rapidjson::Value& payload,
                                           rapidjson::Document&    response,
                                           const char*&            error_code,
                                           std::string&            error_message) = 0;
    };

    /** @brief Interface for the RPC clients spies */
    class IRpcClientSpy
    {
      public:
        /** @brief Destructor */
        virtual ~IRpcClientSpy() { }

        /**
         * @brief Called when a message has been received
         * @param msg Received message
         */
        virtual void rcpClientMessageReceived(const std::string& msg) = 0;

        /**
         * @brief Called when a message has been sent
         * @param msg Sent message
         */
        virtual void rcpClientMessageSent(const std::string& msg) = 0;
    };

    /** @brief RPC error code : NotImplemented */
    static constexpr const char* RPC_ERROR_NOT_IMPLEMENTED = "NotImplemented";
    /** @brief RPC error code : NotSupported */
    static constexpr const char* RPC_ERROR_NOT_SUPPORTED = "NotSupported";
    /** @brief RPC error code : InternalError */
    static constexpr const char* RPC_ERROR_INTERNAL = "InternalError";
    /** @brief RPC error code : ProtocolError */
    static constexpr const char* RPC_ERROR_PROTOCOL = "ProtocolError";
    /** @brief RPC error code : SecurityError */
    static constexpr const char* RPC_ERROR_SECURITY = "SecurityError";
    /** @brief RPC error code : FormationViolation */
    static constexpr const char* RPC_ERROR_FORMATION_VIOLATION = "FormationViolation";
    /** @brief RPC error code : PropertyConstraintViolation */
    static constexpr const char* RPC_ERROR_PROPERTY_CONSTRAINT_VIOLATION = "PropertyConstraintViolation";
    /** @brief RPC error code : OccurenceConstraintViolation */
    static constexpr const char* RPC_ERROR_OCCURENCE_CONSTRAINT_VIOLATION = "OccurenceConstraintViolation";
    /** @brief RPC error code : TypeConstraintViolation */
    static constexpr const char* RPC_ERROR_TYPE_CONSTRAINT_VIOLATION = "TypeConstraintViolation";
    /** @brief RPC error code : GenericError */
    static constexpr const char* RPC_ERROR_GENERIC = "GenericError";
};

} // namespace rpc
} // namespace ocpp

#endif // IRPCCLIENT_H
