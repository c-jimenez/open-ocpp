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

#ifndef OPENOCPP_OCPP20_LOCALCONTROLLER20_H
#define OPENOCPP_OCPP20_LOCALCONTROLLER20_H

#include "Database.h"
#include "ILocalController20.h"
#include "InternalConfigManager.h"
#include "MessagesConverter20.h"
#include "MessagesValidator20.h"
#include "RpcServer.h"
#include "Timer.h"

#include <memory>

namespace ocpp
{
namespace localcontroller
{
namespace ocpp20
{

/** @brief Local controller implementation */
class LocalController20 : public ILocalController20, public ocpp::rpc::RpcServer::IListener
{
  public:
    /** @brief Constructor */
    LocalController20(const ocpp::config::ILocalControllerConfig20&    stack_config,
                      ILocalControllerEventsHandler20&                 events_handler,
                      std::shared_ptr<ocpp::helpers::ITimerPool>       timer_pool,
                      std::shared_ptr<ocpp::helpers::WorkerThreadPool> worker_pool);

    /** @brief Destructor */
    virtual ~LocalController20();

    // ILocalController20 interface

    /** @copydoc const ocpp::config::ILocalControllerConfig20& ILocalController20::getConfig() */
    const ocpp::config::ILocalControllerConfig20& getConfig() override { return m_stack_config; }

    /** @copydoc ocpp::helpers::ITimerPool& ILocalController20::getTimerPool() */
    ocpp::helpers::ITimerPool& getTimerPool() override { return *m_timer_pool.get(); }

    /** @copydoc ocpp::helpers::WorkerThreadPool& ILocalController20::getWorkerPool() */
    ocpp::helpers::WorkerThreadPool& getWorkerPool() override { return *m_worker_pool.get(); }

    /** @copydoc bool ILocalController20::resetData() */
    bool resetData() override;

    /** @copydoc bool ILocalController20::start() */
    bool start() override;

    /** @copydoc bool ILocalController20::stop() */
    bool stop() override;

    // RpcServer::IListener interface

    /** @copydoc bool RpcServer::IListener::rpcAcceptConnection(const char*) */
    bool rpcAcceptConnection(const char* ip_address) override;

    /** @copydoc bool RpcServer::IListener::rpcCheckCredentials(const std::string&, const std::string&, const std::string&) */
    bool rpcCheckCredentials(const std::string& chargepoint_id, const std::string& user, const std::string& password) override;

    /** @copydoc void RpcServer::IListener::rpcClientConnected(const std::string&, std::shared_ptr<Client>) */
    void rpcClientConnected(const std::string& chargepoint_id, std::shared_ptr<ocpp::rpc::RpcServer::Client> client) override;

    /** @copydoc void RpcServer::IListener::rpcClientFailedToConnect(const char*) */
    void rpcClientFailedToConnect(const char* ip_address) override;

    /** @copydoc void RpcServer::IListener::rpcServerError() */
    void rpcServerError() override;

  private:
    /** @brief Stack configuration */
    const ocpp::config::ILocalControllerConfig20& m_stack_config;
    /** @brief User defined events handler */
    ILocalControllerEventsHandler20& m_events_handler;

    /** @brief Timer pool */
    std::shared_ptr<ocpp::helpers::ITimerPool> m_timer_pool;
    /** @brief Worker thread pool */
    std::shared_ptr<ocpp::helpers::WorkerThreadPool> m_worker_pool;

    /** @brief Database */
    std::unique_ptr<ocpp::database::Database> m_database;
    /** @brief Internal configuration manager */
    std::unique_ptr<ocpp::config::InternalConfigManager> m_internal_config;

    /** @brief Messages converter */
    ocpp::messages::ocpp20::MessagesConverter20 m_messages_converter;
    /** @brief Messages validator */
    ocpp::messages::ocpp20::MessagesValidator20 m_messages_validator;

    /** @brief Websocket server */
    std::unique_ptr<ocpp::websockets::IWebsocketServer> m_ws_server;
    /** @brief RPC server */
    std::unique_ptr<ocpp::rpc::RpcServer> m_rpc_server;
    /** @brief RPC pool */
    std::unique_ptr<ocpp::rpc::RpcPool> m_rpc_pool;

    /** @brief Uptime timer */
    std::unique_ptr<ocpp::helpers::Timer> m_uptime_timer;
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

} // namespace ocpp20
} // namespace localcontroller
} // namespace ocpp

#endif // OPENOCPP_OCPP20_LOCALCONTROLLER20_H
