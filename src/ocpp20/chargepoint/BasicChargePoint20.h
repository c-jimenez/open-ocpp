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

#ifndef OPENOCPP_OCPP20_BASICCHARGEPOINT20_H
#define OPENOCPP_OCPP20_BASICCHARGEPOINT20_H

#include "Connectors20.h"
#include "GenericMessageHandler.h"
#include "GenericMessageSender.h"
#include "IBasicChargePoint20.h"
#include "IDeviceModel20.h"
#include "InternalConfigManager.h"
#include "Logger.h"
#include "MessagesConverter20.h"
#include "MessagesValidator20.h"
#include "RpcClient.h"
#include "Timer.h"

#include <atomic>
#include <memory>

namespace ocpp
{
// Forward declarations
namespace messages
{
class MessageDispatcher;
class GenericMessageSender;
} // namespace messages
namespace websockets
{
class IWebsocketClient;
}
namespace chargepoint
{
namespace ocpp20
{

// Forward declarations
class StatusManager;
class TriggerMessageManager;

/** @brief Basic charge point implementation (only use cases defined in OCPP 2.0.1 Part 0 Specification §4) */
class BasicChargePoint20 : public IBasicChargePoint20,
                           public ocpp::rpc::IRpc::IListener,
                           public ocpp::rpc::IRpc::ISpy,
                           public ocpp::rpc::RpcClient::IListener
{
  public:
    /** @brief Constructor */
    BasicChargePoint20(const ocpp::config::IChargePointConfig20&                      stack_config,
                       IDeviceModel&                                                  device_model,
                       IBasicChargePointEventsHandler&                                events_handler,
                       std::shared_ptr<ocpp::helpers::ITimerPool>                     timer_pool,
                       std::shared_ptr<ocpp::helpers::WorkerThreadPool>               worker_pool,
                       std::unique_ptr<ocpp::messages::ocpp20::MessagesConverter20>&& messages_converter);

    /** @brief Destructor */
    virtual ~BasicChargePoint20();

    // IBasicChargePoint20 interface

    /** @copydoc ocpp::helpers::ITimerPool& IBasicChargePoint20::getTimerPool() */
    ocpp::helpers::ITimerPool& getTimerPool() override { return *m_timer_pool.get(); }

    /** @copydoc ocpp::helpers::WorkerThreadPool& IBasicChargePoint20::getWorkerPool() */
    ocpp::helpers::WorkerThreadPool& getWorkerPool() override { return *m_worker_pool.get(); }

    /** @copydoc bool IBasicChargePoint20::resetData() */
    bool resetData() override;

    /** @copydoc bool IBasicChargePoint20::resetConnectorData() */
    bool resetConnectorData() override;

    /** @copydoc bool IBasicChargePoint20::start(ocpp::types::ocpp20::BootReasonEnumType) */
    bool start(ocpp::types::ocpp20::BootReasonEnumType boot_reason) override;

    /** @copydoc bool IBasicChargePoint20::stop() */
    bool stop() override;

    /** @copydoc bool IBasicChargePoint20::reconnect() */
    bool reconnect() override;

    /** @copydoc ocpp::types::ocpp20::RegistrationStatusEnumType IBasicChargePoint20::getRegistrationStatus() */
    ocpp::types::ocpp20::RegistrationStatusEnumType getRegistrationStatus() override;

    /** @copydoc ocpp::types::ocpp20::ConnectorStatusEnumType IBasicChargePoint20::getConnectorStatus(unsigned int, unsigned int) */
    ocpp::types::ocpp20::ConnectorStatusEnumType getConnectorStatus(unsigned int evse_id, unsigned int connector_id) override;

    /** @copydoc bool IBasicChargePoint20::statusNotification(unsigned int, unsigned int, ocpp::types::ocpp20::ConnectorStatusEnumType) */
    bool statusNotification(unsigned int evse_id, unsigned int connector_id, ocpp::types::ocpp20::ConnectorStatusEnumType status) override;

    // RpcClient::IListener interface

    /** @copydoc void RpcClient::IListener::rpcClientConnected() */
    void rpcClientConnected() override;

    /** @copydoc void RpcClient::IListener::rpcClientFailed() */
    void rpcClientFailed() override;

    // IRpc::IListener interface

    /** @copydoc void IRpc::IListener::rpcDisconnected() */
    void rpcDisconnected() override;

    /** @copydoc void IRpc::IListener::rpcError() */
    void rpcError() override;

    /** @copydoc void IRpc::IListener::rpcCallReceived(const std::string&,
                                                       const rapidjson::Value&,
                                                       rapidjson::Document&,
                                                       std::string&,
                                                       std::string&) */
    bool rpcCallReceived(const std::string&      action,
                         const rapidjson::Value& payload,
                         rapidjson::Document&    response,
                         std::string&            error_code,
                         std::string&            error_message) override;

    // IRpc::ISpy interface

    /** @copydoc void IRpc::ISpy::rcpMessageReceived(const std::string&) */
    void rcpMessageReceived(const std::string& msg) override;

    /** @copydoc void IRpc::ISpy::rcpMessageSent(const std::string&) */
    void rcpMessageSent(const std::string& msg) override;

  private:
    /** @brief Stack configuration */
    const ocpp::config::IChargePointConfig20& m_stack_config;
    /** @brief Device model */
    IDeviceModel& m_device_model;
    /** @brief User defined events handler */
    IBasicChargePointEventsHandler& m_events_handler;

    /** @brief Timer pool */
    std::shared_ptr<ocpp::helpers::ITimerPool> m_timer_pool;
    /** @brief Worker thread pool */
    std::shared_ptr<ocpp::helpers::WorkerThreadPool> m_worker_pool;

    /** @brief Database */
    ocpp::database::Database m_database;
    /** @brief Internal configuration manager */
    ocpp::config::InternalConfigManager m_internal_config;
    /** @brief Connectors */
    Connectors m_connectors;

    /** @brief Messages converter */
    std::unique_ptr<ocpp::messages::ocpp20::MessagesConverter20> m_messages_converter;
    /** @brief Messaes validator */
    ocpp::messages::ocpp20::MessagesValidator20 m_messages_validator;
    /** @brief Indicate that a stop process is in progress */
    std::atomic<bool> m_stop_in_progress;
    /** @brief Indicate that a reconnection process has been scheduled */
    std::atomic<bool> m_reconnect_scheduled;

    /** @brief Websocket s*/
    std::unique_ptr<ocpp::websockets::IWebsocketClient> m_ws_client;
    /** @brief RPC client */
    std::unique_ptr<ocpp::rpc::RpcClient> m_rpc_client;
    /** @brief Message dispatcher */
    std::unique_ptr<ocpp::messages::MessageDispatcher> m_msg_dispatcher;
    /** @brief Message sender */
    std::unique_ptr<ocpp::messages::GenericMessageSender> m_msg_sender;

    /** @brief Status manager */
    std::unique_ptr<StatusManager> m_status_manager;
    /** @brief Trigger messages manager */
    std::unique_ptr<TriggerMessageManager> m_trigger_manager;

    /** @brief Uptime timer */
    ocpp::helpers::Timer m_uptime_timer;
    /** @brief Uptime in seconds */
    unsigned int m_uptime;
    /** @brief Disconnected time in seconds */
    unsigned int m_disconnected_time;
    /** @brief Total uptime in seconds */
    unsigned int m_total_uptime;
    /** @brief Total disconnected time in seconds */
    unsigned int m_total_disconnected_time;

    /** @brief Initialize the database */
    void initDatabase();
    /** @brief Process uptime */
    void processUptime();
    /** @brief Save the uptime counter in database */
    void saveUptime();

    /** @brief Schedule a reconnection to the Central System */
    void scheduleReconnect();
    /** @brief Start the connection process to the Central System */
    bool doConnect();

    /**
     * @brief Execute a call request
     * @param action RPC action for the request
     * @param request Request payload
     * @param response Response payload
     * @param error Error (Empty if not a CallError)
     * @param message Error message (Empty if not a CallError)
     * @return true if the request has been sent and a response has been received, false otherwise
     */
    template <typename RequestType, typename ResponseType>
    bool call(const std::string& action, const RequestType& request, ResponseType& response, std::string& error, std::string& message)
    {
        bool ret = false;

        std::string identifier = m_stack_config.chargePointIdentifier();
        LOG_DEBUG << "[" << identifier << "] - " << action;

        if (m_msg_sender && !m_stop_in_progress)
        {
            ocpp::messages::CallResult res = m_msg_sender->call(action, request, response, error, message);
            if (res != ocpp::messages::CallResult::Ok)
            {
                LOG_ERROR << "[" << identifier << "] - " << action << " => "
                          << (res == ocpp::messages::CallResult::Failed ? "Timeout" : "Error");
            }
            else
            {
                ret = true;
            }
        }

        return ret;
    }
};

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_OCPP20_BASICCHARGEPOINT20_H
