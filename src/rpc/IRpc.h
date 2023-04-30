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

#ifndef OPENOCPP_IRPC_H
#define OPENOCPP_IRPC_H

#include "json.h"

#include <chrono>
#include <string>

namespace ocpp
{
namespace rpc
{

/** @brief Interface for RPC objects implementations */
class IRpc
{
  public:
    // Forward declarations
    class IListener;
    class ISpy;

    /** @brief Destructor */
    virtual ~IRpc() { }

    /**
     * @brief Indicate if the connection is active
     * @return true if the connection is active, false otherwise
     */
    virtual bool isConnected() const = 0;

    /**
     * @brief Call a remote action and wait for its response
     * @param action Remote action
     * @param payload JSON payload for the action
     * @param rpc_frame Full JSON response received
     * @param response JSON response received
     * @param error Error code (empty if no error)
     * @param message Error message (empty if no error)
     * @param timeout Response timeout
     * @return true if a response has been received, false otherwise
     */
    virtual bool call(const std::string&         action,
                      const rapidjson::Document& payload,
                      rapidjson::Document&       rpc_frame,
                      rapidjson::Value&          response,
                      std::string&               error,
                      std::string&               message,
                      std::chrono::milliseconds  timeout = std::chrono::seconds(2)) = 0;

    /**
     * @brief Register a listener to the RPC events
     * @param listener Listener object
     */
    virtual void registerListener(IListener& listener) = 0;

    /**
     * @brief Register a spy to the RPC exchanges
     * @param spy Spy object
     */
    virtual void registerSpy(ISpy& spy) = 0;

    /**
     * @brief Unregister a spy from the RPC exchanges
     * @param spy Spy object
     */
    virtual void unregisterSpy(ISpy& spy) = 0;

    /** @brief Interface for the RPC listeners */
    class IListener
    {
      public:
        /** @brief Destructor */
        virtual ~IListener() { }

        /** @brief Called when connection is lost */
        virtual void rpcDisconnected() = 0;

        /** @brief Called when a critical error occured */
        virtual void rpcError() = 0;

        /**
         * @brief Called when a CALL message has been received
         * @param action Action
         * @param payload JSON payload for the action
         * @param response JSON response to send
         * @param error_code Standard error code, empty if no error
         * @param error_msg Additionnal error message, empty if no error
         * @return true if the call is accepted, false otherwise
         */
        virtual bool rpcCallReceived(const std::string&      action,
                                     const rapidjson::Value& payload,
                                     rapidjson::Document&    response,
                                     std::string&            error_code,
                                     std::string&            error_message) = 0;
    };

    /** @brief Interface for the RPC clients spies */
    class ISpy
    {
      public:
        /** @brief Destructor */
        virtual ~ISpy() { }

        /**
         * @brief Called when a message has been received
         * @param msg Received message
         */
        virtual void rcpMessageReceived(const std::string& msg) = 0;

        /**
         * @brief Called when a message has been sent
         * @param msg Sent message
         */
        virtual void rcpMessageSent(const std::string& msg) = 0;
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

#endif // OPENOCPP_IRPC_H
