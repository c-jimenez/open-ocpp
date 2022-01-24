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

#ifndef CHARGEPOINTPROXY_H
#define CHARGEPOINTPROXY_H

#include "ICentralSystem.h"
#include "MessageDispatcher.h"
#include "RpcServer.h"

#include <memory>

namespace ocpp
{
namespace centralsystem
{

/** @brief Interface for charge point proxy implementations */
class ChargePointProxy : public ICentralSystem::IChargePoint, public ocpp::rpc::IRpc::IListener, public ocpp::rpc::IRpc::ISpy
{
  public:
    /**
     * @brief Constructor
     * @param identifier Charge point's identifier
     * @param rpc RPC connection with the charge point
     * @param schemas_path Path to the JSON schemas needed to validate payloads
     */
    ChargePointProxy(const std::string& identifier, std::shared_ptr<ocpp::rpc::RpcServer::Client> rpc, const std::string& schemas_path);
    /** @brief Destructor */
    virtual ~ChargePointProxy();

    // ICentralSystem::IChargePoint interface

    /** @copydoc const std::string& ICentralSystem::IChargePoint::identifier() const */
    const std::string& identifier() const override { return m_identifier; }

    /** @copydoc void ICentralSystem::IChargePoint::disconnect() const */
    void disconnect() override;

    // IRpc::IListener interface

    /** @copydoc void IRpc::IListener::rpcDisconnected() */
    void rpcDisconnected() override;

    /** @copydoc void IRpc::IListener::rpcError() */
    void rpcError() override;

    /** @copydoc bool IRpc::IListener::rpcCallReceived(const std::string&,
                                                       const rapidjson::Value&,
                                                       rapidjson::Document&,
                                                       const char*&,
                                                       std::string&) */
    bool rpcCallReceived(const std::string&      action,
                         const rapidjson::Value& payload,
                         rapidjson::Document&    response,
                         const char*&            error_code,
                         std::string&            error_message) override;

    // IRpc::ISpy interface

    /** @copydoc void IRpc::ISpy::rcpMessageReceived(const std::string& msg) */
    void rcpMessageReceived(const std::string& msg) override;

    /** @copydoc void IRpc::ISpy::rcpMessageSent(const std::string& msg) */
    void rcpMessageSent(const std::string& msg) override;

  private:
    /** @brief Charge point's identifier */
    std::string m_identifier;
    /** @brief RPC connection */
    std::shared_ptr<ocpp::rpc::RpcServer::Client> m_rpc;
    /** @brief Message dispatcher */
    ocpp::messages::MessageDispatcher m_msg_dispatcher;
};

} // namespace centralsystem
} // namespace ocpp

#endif // CHARGEPOINTPROXY_H
