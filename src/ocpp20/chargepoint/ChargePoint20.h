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

#ifndef OPENOCPP_OCPP20_CHARGEPOINT20_H
#define OPENOCPP_OCPP20_CHARGEPOINT20_H

#include "GenericMessageSender.h"
#include "IChargePoint20.h"
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

/** @brief Charge point implementation */
class ChargePoint20 : public IChargePoint20,
                      public ocpp::rpc::IRpc::IListener,
                      public ocpp::rpc::IRpc::ISpy,
                      public ocpp::rpc::RpcClient::IListener
{
  public:
    /** @brief Constructor */
    ChargePoint20(const ocpp::config::IChargePointConfig20&        stack_config,
                  IChargePointEventsHandler20&                     events_handler,
                  std::shared_ptr<ocpp::helpers::ITimerPool>       timer_pool,
                  std::shared_ptr<ocpp::helpers::WorkerThreadPool> worker_pool);

    /** @brief Destructor */
    virtual ~ChargePoint20();

    // IChargePoint20 interface

    /** @copydoc ocpp::helpers::ITimerPool& IChargePoint20::getTimerPool() */
    ocpp::helpers::ITimerPool& getTimerPool() override { return *m_timer_pool.get(); }

    /** @copydoc ocpp::helpers::WorkerThreadPool& IChargePoint20::getWorkerPool() */
    ocpp::helpers::WorkerThreadPool& getWorkerPool() override { return *m_worker_pool.get(); }

    /** @copydoc bool IChargePoint20::resetData() */
    bool resetData() override;

    /** @copydoc bool IChargePoint20::start() */
    bool start() override;

    /** @copydoc bool IChargePoint20::stop() */
    bool stop() override;

    /** @copydoc bool IChargePoint20::reconnect() */
    bool reconnect() override;

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

    // OCPP operations

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::BootNotification20Req&,
     *                                          ocpp::messages::ocpp20::BootNotification20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::BootNotification20Req& request,
              ocpp::messages::ocpp20::BootNotification20Conf&      response,
              std::string&                                         error,
              std::string&                                         message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::Authorize20Req&,
     *                                          ocpp::messages::ocpp20::Authorize20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::Authorize20Req& request,
              ocpp::messages::ocpp20::Authorize20Conf&      response,
              std::string&                                  error,
              std::string&                                  message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::ClearedChargingLimit20Req&,
     *                                          ocpp::messages::ocpp20::ClearedChargingLimit20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::ClearedChargingLimit20Req& request,
              ocpp::messages::ocpp20::ClearedChargingLimit20Conf&      response,
              std::string&                                             error,
              std::string&                                             message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::CostUpdated20Req&,
     *                                          ocpp::messages::ocpp20::CostUpdated20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::CostUpdated20Req& request,
              ocpp::messages::ocpp20::CostUpdated20Conf&      response,
              std::string&                                    error,
              std::string&                                    message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::DataTransfer20Req&,
     *                                          ocpp::messages::ocpp20::DataTransfer20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::DataTransfer20Req& request,
              ocpp::messages::ocpp20::DataTransfer20Conf&      response,
              std::string&                                     error,
              std::string&                                     message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::FirmwareStatusNotification20Req&,
     *                                          ocpp::messages::ocpp20::FirmwareStatusNotification20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::FirmwareStatusNotification20Req& request,
              ocpp::messages::ocpp20::FirmwareStatusNotification20Conf&      response,
              std::string&                                                   error,
              std::string&                                                   message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::Heartbeat20Req&,
     *                                          ocpp::messages::ocpp20::Heartbeat20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::Heartbeat20Req& request,
              ocpp::messages::ocpp20::Heartbeat20Conf&      response,
              std::string&                                  error,
              std::string&                                  message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::LogStatusNotification20Req&,
     *                                          ocpp::messages::ocpp20::LogStatusNotification20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::LogStatusNotification20Req& request,
              ocpp::messages::ocpp20::LogStatusNotification20Conf&      response,
              std::string&                                              error,
              std::string&                                              message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::MeterValues20Req&,
     *                                          ocpp::messages::ocpp20::MeterValues20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::MeterValues20Req& request,
              ocpp::messages::ocpp20::MeterValues20Conf&      response,
              std::string&                                    error,
              std::string&                                    message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::NotifyChargingLimit20Req&,
     *                                          ocpp::messages::ocpp20::NotifyChargingLimit20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::NotifyChargingLimit20Req& request,
              ocpp::messages::ocpp20::NotifyChargingLimit20Conf&      response,
              std::string&                                            error,
              std::string&                                            message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::NotifyCustomerInformation20Req&,
     *                                          ocpp::messages::ocpp20::NotifyCustomerInformation20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::NotifyCustomerInformation20Req& request,
              ocpp::messages::ocpp20::NotifyCustomerInformation20Conf&      response,
              std::string&                                                  error,
              std::string&                                                  message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::NotifyDisplayMessages20Req&,
     *                                          ocpp::messages::ocpp20::NotifyDisplayMessages20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::NotifyDisplayMessages20Req& request,
              ocpp::messages::ocpp20::NotifyDisplayMessages20Conf&      response,
              std::string&                                              error,
              std::string&                                              message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::NotifyEVChargingNeeds20Req&,
     *                                          ocpp::messages::ocpp20::NotifyEVChargingNeeds20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::NotifyEVChargingNeeds20Req& request,
              ocpp::messages::ocpp20::NotifyEVChargingNeeds20Conf&      response,
              std::string&                                              error,
              std::string&                                              message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::NotifyEVChargingSchedule20Req&,
     *                                          ocpp::messages::ocpp20::NotifyEVChargingSchedule20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::NotifyEVChargingSchedule20Req& request,
              ocpp::messages::ocpp20::NotifyEVChargingSchedule20Conf&      response,
              std::string&                                                 error,
              std::string&                                                 message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::NotifyEvent20Req&,
     *                                          ocpp::messages::ocpp20::NotifyEvent20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::NotifyEvent20Req& request,
              ocpp::messages::ocpp20::NotifyEvent20Conf&      response,
              std::string&                                    error,
              std::string&                                    message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::NotifyMonitoringReport20Req&,
     *                                          ocpp::messages::ocpp20::NotifyMonitoringReport20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::NotifyMonitoringReport20Req& request,
              ocpp::messages::ocpp20::NotifyMonitoringReport20Conf&      response,
              std::string&                                               error,
              std::string&                                               message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::NotifyReport20Req&,
     *                                          ocpp::messages::ocpp20::NotifyReport20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::NotifyReport20Req& request,
              ocpp::messages::ocpp20::NotifyReport20Conf&      response,
              std::string&                                     error,
              std::string&                                     message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::PublishFirmwareStatusNotification20Req&,
     *                                          ocpp::messages::ocpp20::PublishFirmwareStatusNotification20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::PublishFirmwareStatusNotification20Req& request,
              ocpp::messages::ocpp20::PublishFirmwareStatusNotification20Conf&      response,
              std::string&                                                          error,
              std::string&                                                          message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::ReportChargingProfiles20Req&,
     *                                          ocpp::messages::ocpp20::ReportChargingProfiles20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::ReportChargingProfiles20Req& request,
              ocpp::messages::ocpp20::ReportChargingProfiles20Conf&      response,
              std::string&                                               error,
              std::string&                                               message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::ReservationStatusUpdate20Req&,
     *                                          ocpp::messages::ocpp20::ReservationStatusUpdate20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::ReservationStatusUpdate20Req& request,
              ocpp::messages::ocpp20::ReservationStatusUpdate20Conf&      response,
              std::string&                                                error,
              std::string&                                                message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::SecurityEventNotification20Req&,
     *                                          ocpp::messages::ocpp20::SecurityEventNotification20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::SecurityEventNotification20Req& request,
              ocpp::messages::ocpp20::SecurityEventNotification20Conf&      response,
              std::string&                                                  error,
              std::string&                                                  message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::SignCertificate20Req&,
     *                                          ocpp::messages::ocpp20::SignCertificate20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::SignCertificate20Req& request,
              ocpp::messages::ocpp20::SignCertificate20Conf&      response,
              std::string&                                        error,
              std::string&                                        message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::StatusNotification20Req&,
     *                                          ocpp::messages::ocpp20::StatusNotification20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::StatusNotification20Req& request,
              ocpp::messages::ocpp20::StatusNotification20Conf&      response,
              std::string&                                           error,
              std::string&                                           message) override;

    /** @copydoc bool ICentralSystemProxy20::call(const ocpp::messages::ocpp20::TransactionEvent20Req&,
     *                                          ocpp::messages::ocpp20::TransactionEvent20Conf&,
     *                                          std::string&,
     *                                          std::string&) */
    bool call(const ocpp::messages::ocpp20::TransactionEvent20Req& request,
              ocpp::messages::ocpp20::TransactionEvent20Conf&      response,
              std::string&                                         error,
              std::string&                                         message) override;

  private:
    /** @brief Stack configuration */
    const ocpp::config::IChargePointConfig20& m_stack_config;
    /** @brief User defined events handler */
    IChargePointEventsHandler20& m_events_handler;

    /** @brief Timer pool */
    std::shared_ptr<ocpp::helpers::ITimerPool> m_timer_pool;
    /** @brief Worker thread pool */
    std::shared_ptr<ocpp::helpers::WorkerThreadPool> m_worker_pool;

    /** @brief Database */
    ocpp::database::Database m_database;
    /** @brief Internal configuration manager */
    ocpp::config::InternalConfigManager m_internal_config;

    /** @brief Messages converter */
    ocpp::messages::ocpp20::MessagesConverter20 m_messages_converter;
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
                ret = false;
            }
        }

        return ret;
    }
};

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_OCPP20_CHARGEPOINT20_H
