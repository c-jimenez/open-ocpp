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

#ifndef CENTRALSYSTEM_H
#define CENTRALSYSTEM_H

#include "Database.h"
#include "ICentralSystem.h"
#include "InternalConfigManager.h"
#include "RpcServer.h"
#include "Timer.h"
#include "TimerPool.h"
#include "WorkerThreadPool.h"

#include <memory>

namespace ocpp
{
namespace centralsystem
{

/** @brief Central system implementation */
class CentralSystem : public ICentralSystem, public ocpp::rpc::RpcServer::IListener
{
  public:
    /** @brief Constructor */
    CentralSystem(const ocpp::config::ICentralSystemConfig& stack_config, ICentralSystemEventsHandler& events_handler);

    /** @brief Destructor */
    virtual ~CentralSystem();

    // ICentralSystem interface

    /** @copydoc ocpp::helpers::TimerPool& ICentralSystem::getTimerPool() */
    ocpp::helpers::TimerPool& getTimerPool() override { return m_timer_pool; }

    /** @copydoc bool ICentralSystem::resetData() */
    bool resetData() override;

    /** @copydoc bool ICentralSystem::start() */
    bool start() override;

    /** @copydoc bool ICentralSystem::stop() */
    bool stop() override;

    // RpcServer::IListener interface

    /** @copydoc bool RpcServer::IListener::rpcCheckCredentials(const std::string&, const std::string&, const std::string&) */
    bool rpcCheckCredentials(const std::string& chargepoint_id, const std::string& user, const std::string& password) override;

    /** @copydoc void RpcServer::IListener::rpcClientConnected(const std::string&, std::shared_ptr<IClient>) */
    void rpcClientConnected(const std::string& chargepoint_id, std::shared_ptr<ocpp::rpc::RpcServer::IClient> client) override;

    /** @copydoc void RpcServer::IListener::rpcServerError() */
    void rpcServerError() override;

  private:
    /** @brief Stack configuration */
    const ocpp::config::ICentralSystemConfig& m_stack_config;
    /** @brief User defined events handler */
    ICentralSystemEventsHandler& m_events_handler;

    /** @brief Timer pool */
    ocpp::helpers::TimerPool m_timer_pool;
    /** @brief Worker thread pool */
    ocpp::helpers::WorkerThreadPool m_worker_pool;

    /** @brief Database */
    ocpp::database::Database m_database;
    /** @brief Internal configuration manager */
    ocpp::config::InternalConfigManager m_internal_config;

    /** @brief Websocket server */
    std::unique_ptr<ocpp::websockets::IWebsocketServer> m_ws_server;
    /** @brief RPC server */
    std::unique_ptr<ocpp::rpc::RpcServer> m_rpc_server;

    /** @brief Uptime timer */
    ocpp::helpers::Timer m_uptime_timer;
    /** @brief Uptime in seconds */
    unsigned int m_uptime;
    /** @brief Total uptime in seconds */
    unsigned int m_total_uptime;

    /** @brief Initialize the database */
    void initDatabase();
    /** @brief Process uptime */
    void processUptime();
    /** @brief Save the uptime counter in database */
    void saveUptime();
};

} // namespace centralsystem
} // namespace ocpp

#endif // CENTRALSYSTEM_H
