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

#ifndef RPCSTUB_H
#define RPCSTUB_H

#include "IRpc.h"

#include <memory>
#include <vector>

namespace ocpp
{
namespace rpc
{

/** @brief RPC stub for unit tests */
class RpcStub : public IRpc
{
  public:
    /** @brief Constructor */
    RpcStub();
    /** @brief Destructor */
    virtual ~RpcStub();

    /** @copydoc bool IRpc::isConnected() */
    bool isConnected() const override { return m_connected; }

    /** @copydoc bool IRpc::call(const std::string&, const rapidjson::Document&, rapidjson::Document&, std::chrono::milliseconds) */
    bool call(const std::string&         action,
              const rapidjson::Document& payload,
              rapidjson::Document&       response,
              std::chrono::milliseconds  timeout) override;

    /** @copydoc void IRpc::registerListener(IListener&) */
    void registerListener(IRpc::IListener& listener) override { m_listener = &listener; }

    /** @copydoc void IRpc::registerSpy(ISpy&) */
    void registerSpy(IRpc::ISpy& spy) override { m_spy = &spy; }

    // API

    /** @brief Set the connectivity state */
    void setConnected(bool is_connected) { m_connected = is_connected; }
    /** @brief Indicate if the next call will fail */
    void setCallWilFail(bool call_will_fail) { m_call_will_fail = call_will_fail; }
    /** @brief Set the next response */
    void setResponse(const rapidjson::Document& response);
    /** @brief Get the listener */
    IRpc::IListener* getListener() { return m_listener; }
    /** @brief Get the spy */
    IRpc::ISpy* getSpy() { return m_spy; }
    /** @brief Get the list of calls */
    const std::vector<std::pair<std::string, std::unique_ptr<rapidjson::Document>>>& getCalls() const { return m_calls; }
    /** @brief Clear the list of calls */
    void clearCalls() { m_calls.clear(); }

  private:
    /** @brief Connectivity state */
    bool m_connected;
    /** @brief RPC listener */
    IRpc::IListener* m_listener;
    /** @brief RPC spy */
    IRpc::ISpy* m_spy;
    /** @brief Indicate if the next call will fail */
    bool m_call_will_fail;
    /** @brief Next response */
    rapidjson::Document m_response;
    /** @brief Calls */
    std::vector<std::pair<std::string, std::unique_ptr<rapidjson::Document>>> m_calls;
};

} // namespace rpc
} // namespace ocpp

#endif // RPCSTUB_H
