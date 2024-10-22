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

#include "ChargePoint20.h"
#include "InternalConfigKeys.h"
#include "Logger.h"
#include "MessageDispatcher.h"
#include "PrivateKey.h"
#include "TimerPool.h"
#include "Url.h"
#include "Version.h"
#include "WebsocketFactory.h"
#include "WorkerThreadPool.h"

#include <filesystem>
#include <iostream>
#include <random>

using namespace ocpp::types;
using namespace ocpp::types::ocpp20;
using namespace ocpp::messages;
using namespace ocpp::messages::ocpp20;

namespace ocpp
{
namespace chargepoint
{
namespace ocpp20
{

/** @brief Instanciate a charge point */
std::unique_ptr<IChargePoint20> IChargePoint20::create(const ocpp::config::IChargePointConfig20& stack_config,
                                                       IChargePointEventsHandler20&              events_handler)
{
    std::shared_ptr<ocpp::helpers::ITimerPool>       timer_pool(new ocpp::helpers::TimerPool());
    std::shared_ptr<ocpp::helpers::WorkerThreadPool> worker_pool =
        std::make_shared<ocpp::helpers::WorkerThreadPool>(2u); // 1 asynchronous timer operations + 1 for asynchronous jobs/responses
    return std::unique_ptr<IChargePoint20>(new ChargePoint20(stack_config, events_handler, timer_pool, worker_pool));
}

/** @brief Instanciate a charge point with the provided timer and worker pools */
std::unique_ptr<IChargePoint20> IChargePoint20::create(const ocpp::config::IChargePointConfig20&        stack_config,
                                                       IChargePointEventsHandler20&                     events_handler,
                                                       std::shared_ptr<ocpp::helpers::ITimerPool>       timer_pool,
                                                       std::shared_ptr<ocpp::helpers::WorkerThreadPool> worker_pool)
{
    return std::unique_ptr<IChargePoint20>(new ChargePoint20(stack_config, events_handler, timer_pool, worker_pool));
}

/** @brief Constructor */
ChargePoint20::ChargePoint20(const ocpp::config::IChargePointConfig20&        stack_config,
                             IChargePointEventsHandler20&                     events_handler,
                             std::shared_ptr<ocpp::helpers::ITimerPool>       timer_pool,
                             std::shared_ptr<ocpp::helpers::WorkerThreadPool> worker_pool)
    : m_stack_config(stack_config),
      m_events_handler(events_handler),
      m_timer_pool(timer_pool),
      m_worker_pool(worker_pool),
      m_database(),
      m_internal_config(m_database),
      m_messages_converter(),
      m_messages_validator(),
      m_stop_in_progress(false),
      m_reconnect_scheduled(false),
      m_ws_client(),
      m_rpc_client(),
      m_msg_dispatcher(),
      m_msg_sender(),
      m_uptime_timer(*m_timer_pool.get(), "Uptime timer"),
      m_uptime(0),
      m_disconnected_time(0),
      m_total_uptime(0),
      m_total_disconnected_time(0)
{
    // Open database
    if (m_stack_config.databasePath().empty() || m_database.open(m_stack_config.databasePath()))
    {
        // Register logger
        if (m_stack_config.logMaxEntriesCount() != 0)
        {
            ocpp::log::Logger::registerDefaultLogger(m_database, m_stack_config.logMaxEntriesCount());
        }

        // Initialize the database
        if (!m_stack_config.databasePath().empty())
        {
            initDatabase();
        }
    }
    else
    {
        LOG_ERROR << "Unable to open database";
    }

    // Uptime timer
    m_uptime_timer.setCallback(std::bind(&ChargePoint20::processUptime, this));

    // Random numbers
    std::srand(static_cast<unsigned int>(time(nullptr)));
}

/** @brief Destructor */
ChargePoint20::~ChargePoint20()
{
    stop();
}

/** @copydoc bool IChargePoint20::resetData() */
bool ChargePoint20::resetData()
{
    bool ret = false;

    // Check if it is already started
    if (!m_rpc_client)
    {
        LOG_INFO << "Reset all data";
        if (!m_stack_config.databasePath().empty())
        {
            // Unregister logger
            if (m_stack_config.logMaxEntriesCount() != 0)
            {
                ocpp::log::Logger::unregisterDefaultLogger();
            }

            // Close database to invalid existing connexions
            m_database.close();

            // Delete database
            if (std::filesystem::remove(m_stack_config.databasePath()))
            {
                // Open database
                if (m_database.open(m_stack_config.databasePath()))
                {
                    // Register logger
                    if (m_stack_config.logMaxEntriesCount() != 0)
                    {
                        ocpp::log::Logger::registerDefaultLogger(m_database, m_stack_config.logMaxEntriesCount());
                    }

                    // Re-initialize with default values
                    m_total_uptime            = 0;
                    m_total_disconnected_time = 0;
                    initDatabase();
                }
                else
                {
                    LOG_ERROR << "Unable to open database";
                }
            }
            else
            {
                LOG_ERROR << "Unable to delete database";
            }
        }
        else
        {
            ret = true;
        }
    }

    return ret;
}

/** @copydoc bool IChargePoint20::start() */
bool ChargePoint20::start()
{
    bool ret = false;

    // Check if it is already started
    if (!m_rpc_client)
    {

        LOG_INFO << "Starting OCPP stack v" << OPEN_OCPP_VERSION << " - Central System : " << m_stack_config.connexionUrl()
                 << " - Charge Point identifier : " << m_stack_config.chargePointIdentifier();

        // Load validator
        ret = m_messages_validator.load(m_stack_config.jsonSchemasPath());
        if (ret)
        {
            // Start uptime counter
            if (!m_stack_config.databasePath().empty())
            {
                m_uptime            = 0;
                m_disconnected_time = 0;
                m_internal_config.setKey(START_DATE_KEY, DateTime::now().str());
                m_uptime_timer.start(std::chrono::seconds(1u));
            }

            // Allocate resources
            m_ws_client  = std::unique_ptr<ocpp::websockets::IWebsocketClient>(ocpp::websockets::WebsocketFactory::newClient());
            m_rpc_client = std::make_unique<ocpp::rpc::RpcClient>(*m_ws_client, "ocpp2.0.1");
            m_rpc_client->registerListener(*this);
            m_rpc_client->registerClientListener(*this);
            m_rpc_client->registerSpy(*this);
            m_msg_dispatcher = std::make_unique<ocpp::messages::MessageDispatcher>(m_messages_validator);
            m_msg_sender     = std::make_unique<ocpp::messages::GenericMessageSender>(
                *m_rpc_client, m_messages_converter, m_messages_validator, m_stack_config.callRequestTimeout());

            // Start connection
            m_stop_in_progress = false;
            ret                = doConnect();
        }
        else
        {
            LOG_ERROR << "Unable to load all the messages validators";
        }
    }
    else
    {
        LOG_ERROR << "Stack already started";
    }

    return ret;
}

/** @copydoc bool IChargePoint20::stop() */
bool ChargePoint20::stop()
{
    bool ret = false;

    // Check if it is already started
    if (m_rpc_client && !m_stop_in_progress)
    {
        LOG_INFO << "Stopping OCPP stack";
        m_stop_in_progress = true;

        // Stop uptime counter
        if (!m_stack_config.databasePath().empty())
        {
            m_uptime_timer.stop();
            saveUptime();
        }

        // Stop connection
        ret = m_rpc_client->stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        // Free resources
        m_ws_client.reset();
        m_rpc_client.reset();
        m_msg_dispatcher.reset();
        m_msg_sender.reset();

        // Close database
        m_database.close();
    }
    else
    {
        LOG_ERROR << "Stack already stopped";
    }

    return ret;
}

/** @copydoc bool IChargePoint20::reconnect() */
bool ChargePoint20::reconnect()
{
    bool ret = false;

    // Check if it is started
    if (m_rpc_client)
    {
        // Schedule of reconnexion
        LOG_INFO << "Reconnect triggered";
        scheduleReconnect();
        ret = true;
    }
    else
    {
        LOG_ERROR << "Stack stopped";
    }

    return ret;
}

/** @copydoc void RpcClient::IListener::rpcClientConnected() */
void ChargePoint20::rpcClientConnected()
{
    LOG_INFO << "Connected to Central System";
    m_events_handler.connectionStateChanged(true);
}

/** @copydoc void RpcClient::IListener::rpcClientFailed() */
void ChargePoint20::rpcClientFailed()
{
    LOG_ERROR << "Connection failed with Central System";
    m_events_handler.connectionFailed();
}

/** @copydoc void IRpc::IListener::rpcDisconnected() */
void ChargePoint20::rpcDisconnected()
{
    // Check if stop is in progress
    if (!m_stop_in_progress)
    {
        LOG_ERROR << "Connection lost with Central System";
        m_events_handler.connectionStateChanged(false);
    }
}

/** @copydoc void IRpc::IListener::rpcError() */
void ChargePoint20::rpcError()
{
    LOG_ERROR << "Connection error with Central System";
}

/** @copydoc void IRpc::IListener::rpcCallReceived(const std::string&,
                                                   const rapidjson::Value&,
                                                   rapidjson::Document&,
                                                   std::string&,
                                                   std::string&) */
bool ChargePoint20::rpcCallReceived(const std::string&      action,
                                    const rapidjson::Value& payload,
                                    rapidjson::Document&    response,
                                    std::string&            error_code,
                                    std::string&            error_message)
{
    bool ret = false;

    // Check if stop is in progress
    if (!m_stop_in_progress)
    {
        ret = m_msg_dispatcher->dispatchMessage(action, payload, response, error_code, error_message);
    }

    return ret;
}

/** @copydoc void IRpc::ISpy::rcpMessageReceived(const std::string&) */
void ChargePoint20::rcpMessageReceived(const std::string& msg)
{
    LOG_COM << "RX : " << msg;
}

/** @copydoc void IRpc::ISpy::rcpMessageSent(const std::string&) */
void ChargePoint20::rcpMessageSent(const std::string& msg)
{
    LOG_COM << "TX : " << msg;
}

// OCPP operations

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::BootNotification20Req&,
 *                                          ocpp::messages::ocpp20::BootNotification20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::BootNotification20Req& request,
                         ocpp::messages::ocpp20::BootNotification20Conf&      response,
                         std::string&                                         error,
                         std::string&                                         message)
{
    return call(BOOTNOTIFICATION20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::Authorize20Req&,
 *                                          ocpp::messages::ocpp20::Authorize20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::Authorize20Req& request,
                         ocpp::messages::ocpp20::Authorize20Conf&      response,
                         std::string&                                  error,
                         std::string&                                  message)
{
    return call(AUTHORIZE20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::ClearedChargingLimit20Req&,
 *                                          ocpp::messages::ocpp20::ClearedChargingLimit20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::ClearedChargingLimit20Req& request,
                         ocpp::messages::ocpp20::ClearedChargingLimit20Conf&      response,
                         std::string&                                             error,
                         std::string&                                             message)
{
    return call(CLEAREDCHARGINGLIMIT20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::CostUpdated20Req&,
 *                                          ocpp::messages::ocpp20::CostUpdated20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::CostUpdated20Req& request,
                         ocpp::messages::ocpp20::CostUpdated20Conf&      response,
                         std::string&                                    error,
                         std::string&                                    message)
{
    return call(COSTUPDATED20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::DataTransfer20Req&,
 *                                          ocpp::messages::ocpp20::DataTransfer20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::DataTransfer20Req& request,
                         ocpp::messages::ocpp20::DataTransfer20Conf&      response,
                         std::string&                                     error,
                         std::string&                                     message)
{
    return call(DATATRANSFER20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::FirmwareStatusNotification20Req&,
 *                                          ocpp::messages::ocpp20::FirmwareStatusNotification20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::FirmwareStatusNotification20Req& request,
                         ocpp::messages::ocpp20::FirmwareStatusNotification20Conf&      response,
                         std::string&                                                   error,
                         std::string&                                                   message)
{
    return call(FIRMWARESTATUSNOTIFICATION20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::Heartbeat20Req&,
 *                                          ocpp::messages::ocpp20::Heartbeat20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::Heartbeat20Req& request,
                         ocpp::messages::ocpp20::Heartbeat20Conf&      response,
                         std::string&                                  error,
                         std::string&                                  message)
{
    return call(HEARTBEAT20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::LogStatusNotification20Req&,
 *                                          ocpp::messages::ocpp20::LogStatusNotification20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::LogStatusNotification20Req& request,
                         ocpp::messages::ocpp20::LogStatusNotification20Conf&      response,
                         std::string&                                              error,
                         std::string&                                              message)
{
    return call(LOGSTATUSNOTIFICATION20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::MeterValues20Req&,
 *                                          ocpp::messages::ocpp20::MeterValues20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::MeterValues20Req& request,
                         ocpp::messages::ocpp20::MeterValues20Conf&      response,
                         std::string&                                    error,
                         std::string&                                    message)
{
    return call(METERVALUES20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::NotifyChargingLimit20Req&,
 *                                          ocpp::messages::ocpp20::NotifyChargingLimit20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::NotifyChargingLimit20Req& request,
                         ocpp::messages::ocpp20::NotifyChargingLimit20Conf&      response,
                         std::string&                                            error,
                         std::string&                                            message)
{
    return call(NOTIFYCHARGINGLIMIT20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::NotifyCustomerInformation20Req&,
 *                                          ocpp::messages::ocpp20::NotifyCustomerInformation20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::NotifyCustomerInformation20Req& request,
                         ocpp::messages::ocpp20::NotifyCustomerInformation20Conf&      response,
                         std::string&                                                  error,
                         std::string&                                                  message)
{
    return call(NOTIFYCUSTOMERINFORMATION20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::NotifyDisplayMessages20Req&,
 *                                          ocpp::messages::ocpp20::NotifyDisplayMessages20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::NotifyDisplayMessages20Req& request,
                         ocpp::messages::ocpp20::NotifyDisplayMessages20Conf&      response,
                         std::string&                                              error,
                         std::string&                                              message)
{
    return call(NOTIFYDISPLAYMESSAGES20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::NotifyEVChargingNeeds20Req&,
 *                                          ocpp::messages::ocpp20::NotifyEVChargingNeeds20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::NotifyEVChargingNeeds20Req& request,
                         ocpp::messages::ocpp20::NotifyEVChargingNeeds20Conf&      response,
                         std::string&                                              error,
                         std::string&                                              message)
{
    return call(NOTIFYEVCHARGINGNEEDS20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::NotifyEVChargingSchedule20Req&,
 *                                          ocpp::messages::ocpp20::NotifyEVChargingSchedule20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::NotifyEVChargingSchedule20Req& request,
                         ocpp::messages::ocpp20::NotifyEVChargingSchedule20Conf&      response,
                         std::string&                                                 error,
                         std::string&                                                 message)
{
    return call(NOTIFYEVCHARGINGSCHEDULE20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::NotifyEvent20Req&,
 *                                          ocpp::messages::ocpp20::NotifyEvent20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::NotifyEvent20Req& request,
                         ocpp::messages::ocpp20::NotifyEvent20Conf&      response,
                         std::string&                                    error,
                         std::string&                                    message)
{
    return call(NOTIFYEVENT20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::NotifyMonitoringReport20Req&,
 *                                          ocpp::messages::ocpp20::NotifyMonitoringReport20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::NotifyMonitoringReport20Req& request,
                         ocpp::messages::ocpp20::NotifyMonitoringReport20Conf&      response,
                         std::string&                                               error,
                         std::string&                                               message)
{
    return call(NOTIFYMONITORINGREPORT20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::NotifyReport20Req&,
 *                                          ocpp::messages::ocpp20::NotifyReport20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::NotifyReport20Req& request,
                         ocpp::messages::ocpp20::NotifyReport20Conf&      response,
                         std::string&                                     error,
                         std::string&                                     message)
{
    return call(NOTIFYREPORT20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::PublishFirmwareStatusNotification20Req&,
 *                                          ocpp::messages::ocpp20::PublishFirmwareStatusNotification20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::PublishFirmwareStatusNotification20Req& request,
                         ocpp::messages::ocpp20::PublishFirmwareStatusNotification20Conf&      response,
                         std::string&                                                          error,
                         std::string&                                                          message)
{
    return call(PUBLISHFIRMWARESTATUSNOTIFICATION20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::ReportChargingProfiles20Req&,
 *                                          ocpp::messages::ocpp20::ReportChargingProfiles20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::ReportChargingProfiles20Req& request,
                         ocpp::messages::ocpp20::ReportChargingProfiles20Conf&      response,
                         std::string&                                               error,
                         std::string&                                               message)
{
    return call(REPORTCHARGINGPROFILES20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::ReservationStatusUpdate20Req&,
 *                                          ocpp::messages::ocpp20::ReservationStatusUpdate20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::ReservationStatusUpdate20Req& request,
                         ocpp::messages::ocpp20::ReservationStatusUpdate20Conf&      response,
                         std::string&                                                error,
                         std::string&                                                message)
{
    return call(RESERVATIONSTATUSUPDATE20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::SecurityEventNotification20Req&,
 *                                          ocpp::messages::ocpp20::SecurityEventNotification20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::SecurityEventNotification20Req& request,
                         ocpp::messages::ocpp20::SecurityEventNotification20Conf&      response,
                         std::string&                                                  error,
                         std::string&                                                  message)
{
    return call(SECURITYEVENTNOTIFICATION20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::SignCertificate20Req&,
 *                                          ocpp::messages::ocpp20::SignCertificate20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::SignCertificate20Req& request,
                         ocpp::messages::ocpp20::SignCertificate20Conf&      response,
                         std::string&                                        error,
                         std::string&                                        message)
{
    return call(SIGNCERTIFICATE20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::StatusNotification20Req&,
 *                                          ocpp::messages::ocpp20::StatusNotification20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::StatusNotification20Req& request,
                         ocpp::messages::ocpp20::StatusNotification20Conf&      response,
                         std::string&                                           error,
                         std::string&                                           message)
{
    return call(STATUSNOTIFICATION20_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::TransactionEvent20Req&,
 *                                          ocpp::messages::ocpp20::TransactionEvent20Conf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::TransactionEvent20Req& request,
                         ocpp::messages::ocpp20::TransactionEvent20Conf&      response,
                         std::string&                                         error,
                         std::string&                                         message)
{
    return call(TRANSACTIONEVENT20_ACTION, request, response, error, message);
}

/** @brief Initialize the database */
void ChargePoint20::initDatabase()
{
    // Initialize internal configuration
    m_internal_config.initDatabaseTable();

    // Internal keys
    if (!m_internal_config.keyExist(STACK_VERSION_KEY))
    {
        m_internal_config.createKey(STACK_VERSION_KEY, OPEN_OCPP_VERSION);
    }
    else
    {
        m_internal_config.setKey(STACK_VERSION_KEY, OPEN_OCPP_VERSION);
    }
    if (!m_internal_config.keyExist(START_DATE_KEY))
    {
        m_internal_config.createKey(START_DATE_KEY, "");
    }
    if (!m_internal_config.keyExist(UPTIME_KEY))
    {
        m_internal_config.createKey(UPTIME_KEY, "0");
    }
    if (!m_internal_config.keyExist(DISCONNECTED_TIME_KEY))
    {
        m_internal_config.createKey(DISCONNECTED_TIME_KEY, "0");
    }
    if (!m_internal_config.keyExist(TOTAL_UPTIME_KEY))
    {
        m_internal_config.createKey(TOTAL_UPTIME_KEY, "0");
    }
    else
    {
        std::string value;
        m_internal_config.getKey(TOTAL_UPTIME_KEY, value);
        m_total_uptime = static_cast<unsigned int>(std::atoi(value.c_str()));
    }
    if (!m_internal_config.keyExist(TOTAL_DISCONNECTED_TIME_KEY))
    {
        m_internal_config.createKey(TOTAL_DISCONNECTED_TIME_KEY, "0");
    }
    else
    {
        std::string value;
        m_internal_config.getKey(TOTAL_DISCONNECTED_TIME_KEY, value);
        m_total_disconnected_time = static_cast<unsigned int>(std::atoi(value.c_str()));
    }
    if (!m_internal_config.keyExist(LAST_CONNECTION_URL_KEY))
    {
        m_internal_config.createKey(LAST_CONNECTION_URL_KEY, "");
    }
}

/** @brief Process uptime */
void ChargePoint20::processUptime()
{
    // Increase counters
    m_uptime++;
    m_total_uptime++;
    if (m_rpc_client && !m_rpc_client->isConnected())
    {
        m_disconnected_time++;
        m_total_disconnected_time++;
    }

    // Save counters
    if ((m_uptime % 15u) == 0)
    {
        m_worker_pool->run<void>(std::bind(&ChargePoint20::saveUptime, this));
    }
}

/** @brief Save the uptime counter in database */
void ChargePoint20::saveUptime()
{
    m_internal_config.setKey(UPTIME_KEY, std::to_string(m_uptime));
    m_internal_config.setKey(DISCONNECTED_TIME_KEY, std::to_string(m_disconnected_time));
    m_internal_config.setKey(TOTAL_UPTIME_KEY, std::to_string(m_total_uptime));
    m_internal_config.setKey(TOTAL_DISCONNECTED_TIME_KEY, std::to_string(m_total_disconnected_time));
}

/** @brief Schedule a reconnection to the Central System */
void ChargePoint20::scheduleReconnect()
{
    // Check if a reconnection is not already scheduled
    if (!m_reconnect_scheduled)
    {
        m_reconnect_scheduled = true;
        m_worker_pool->run<void>(
            [this]
            {
                // Wait to let some time to configure other parameters
                // => Needed when switching security profiles
                std::this_thread::sleep_for(std::chrono::seconds(1u));
                doConnect();
            });
    }
}

/** @brief Start the connection process to the Central System */
bool ChargePoint20::doConnect()
{
    // Check if already connected
    if (m_rpc_client->isConnected())
    {
        // Close connection
        m_rpc_client->stop();
    }

    // Get current security profile
    unsigned int security_profile = m_stack_config.securityProfile();

    // Build connection URL
    std::string connection_url = m_stack_config.connexionUrl();
    if (connection_url[connection_url.size() - 1] != '/')
    {
        connection_url += "/";
    }
    connection_url += ocpp::websockets::Url::encode(m_stack_config.chargePointIdentifier());

    // Configure websocket link
    ocpp::websockets::IWebsocketClient::Credentials credentials;

    std::string basic_auth_password = m_stack_config.basicAuthPassword();
    if (!basic_auth_password.empty() && (security_profile <= 2))
    {
        credentials.user     = m_stack_config.chargePointIdentifier();
        credentials.password = basic_auth_password;
    }
    if (security_profile != 1)
    {
        credentials.tls12_cipher_list = m_stack_config.tlsv12CipherList();
        credentials.tls13_cipher_list = m_stack_config.tlsv13CipherList();

        // Use certificates prodivided by the user application
        credentials.server_certificate_ca = m_stack_config.tlsServerCertificateCa();
        if ((security_profile == 0) || (security_profile == 3))
        {
            credentials.client_certificate                        = m_stack_config.tlsClientCertificate();
            credentials.client_certificate_private_key            = m_stack_config.tlsClientCertificatePrivateKey();
            credentials.client_certificate_private_key_passphrase = m_stack_config.tlsClientCertificatePrivateKeyPassphrase();
        }
        credentials.allow_selfsigned_certificates = m_stack_config.tlsAllowSelfSignedCertificates();
        credentials.allow_expired_certificates    = m_stack_config.tlsAllowExpiredCertificates();
        credentials.accept_untrusted_certificates = m_stack_config.tlsAcceptNonTrustedCertificates();
        credentials.skip_server_name_check        = m_stack_config.tlsSkipServerNameCheck();
        credentials.encoded_pem_certificates      = false;
    }

    // Start connection process
    m_reconnect_scheduled = false;
    return m_rpc_client->start(connection_url,
                               credentials,
                               m_stack_config.connectionTimeout(),
                               m_stack_config.retryInterval(),
                               m_stack_config.webSocketPingInterval());
}

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp
