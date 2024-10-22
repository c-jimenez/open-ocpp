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
    std::unique_ptr<MessagesConverter20> messages_converter(new MessagesConverter20());
    return std::unique_ptr<IChargePoint20>(
        new ChargePoint20(stack_config, events_handler, timer_pool, worker_pool, std::move(messages_converter)));
}

/** @brief Instanciate a charge point with the provided timer and worker pools */
std::unique_ptr<IChargePoint20> IChargePoint20::create(const ocpp::config::IChargePointConfig20&        stack_config,
                                                       IChargePointEventsHandler20&                     events_handler,
                                                       std::shared_ptr<ocpp::helpers::ITimerPool>       timer_pool,
                                                       std::shared_ptr<ocpp::helpers::WorkerThreadPool> worker_pool)
{
    std::unique_ptr<MessagesConverter20> messages_converter(new MessagesConverter20());
    return std::unique_ptr<IChargePoint20>(
        new ChargePoint20(stack_config, events_handler, timer_pool, worker_pool, std::move(messages_converter)));
}

/** @brief Constructor */
ChargePoint20::ChargePoint20(const ocpp::config::IChargePointConfig20&                      stack_config,
                             IChargePointEventsHandler20&                                   events_handler,
                             std::shared_ptr<ocpp::helpers::ITimerPool>                     timer_pool,
                             std::shared_ptr<ocpp::helpers::WorkerThreadPool>               worker_pool,
                             std::unique_ptr<ocpp::messages::ocpp20::MessagesConverter20>&& messages_converter)
    : GenericMessageHandler<CancelReservation20Req, CancelReservation20Conf>(CANCELRESERVATION20_ACTION, *messages_converter),
      GenericMessageHandler<CertificateSigned20Req, CertificateSigned20Conf>(CERTIFICATESIGNED20_ACTION, *messages_converter),
      GenericMessageHandler<ChangeAvailability20Req, ChangeAvailability20Conf>(CHANGEAVAILABILITY20_ACTION, *messages_converter),
      GenericMessageHandler<ClearCache20Req, ClearCache20Conf>(CLEARCACHE20_ACTION, *messages_converter),
      GenericMessageHandler<ClearChargingProfile20Req, ClearChargingProfile20Conf>(CLEARCHARGINGPROFILE20_ACTION, *messages_converter),
      GenericMessageHandler<ClearDisplayMessage20Req, ClearDisplayMessage20Conf>(CLEARDISPLAYMESSAGE20_ACTION, *messages_converter),
      GenericMessageHandler<ClearVariableMonitoring20Req, ClearVariableMonitoring20Conf>(CLEARVARIABLEMONITORING20_ACTION,
                                                                                         *messages_converter),
      GenericMessageHandler<CustomerInformation20Req, CustomerInformation20Conf>(CUSTOMERINFORMATION20_ACTION, *messages_converter),
      GenericMessageHandler<DataTransfer20Req, DataTransfer20Conf>(DATATRANSFER20_ACTION, *messages_converter),
      GenericMessageHandler<DeleteCertificate20Req, DeleteCertificate20Conf>(DELETECERTIFICATE20_ACTION, *messages_converter),
      GenericMessageHandler<Get15118EVCertificate20Req, Get15118EVCertificate20Conf>(GET15118EVCERTIFICATE20_ACTION, *messages_converter),
      GenericMessageHandler<GetBaseReport20Req, GetBaseReport20Conf>(GETBASEREPORT20_ACTION, *messages_converter),
      GenericMessageHandler<GetCertificateStatus20Req, GetCertificateStatus20Conf>(GETCERTIFICATESTATUS20_ACTION, *messages_converter),
      GenericMessageHandler<GetChargingProfiles20Req, GetChargingProfiles20Conf>(GETCHARGINGPROFILES20_ACTION, *messages_converter),
      GenericMessageHandler<GetCompositeSchedule20Req, GetCompositeSchedule20Conf>(GETCOMPOSITESCHEDULE20_ACTION, *messages_converter),
      GenericMessageHandler<GetDisplayMessages20Req, GetDisplayMessages20Conf>(GETDISPLAYMESSAGES20_ACTION, *messages_converter),
      GenericMessageHandler<GetInstalledCertificateIds20Req, GetInstalledCertificateIds20Conf>(GETINSTALLEDCERTIFICATEIDS20_ACTION,
                                                                                               *messages_converter),
      GenericMessageHandler<GetLocalListVersion20Req, GetLocalListVersion20Conf>(GETLOCALLISTVERSION20_ACTION, *messages_converter),
      GenericMessageHandler<GetLog20Req, GetLog20Conf>(GETLOG20_ACTION, *messages_converter),
      GenericMessageHandler<GetMonitoringReport20Req, GetMonitoringReport20Conf>(GETMONITORINGREPORT20_ACTION, *messages_converter),
      GenericMessageHandler<GetReport20Req, GetReport20Conf>(GETREPORT20_ACTION, *messages_converter),
      GenericMessageHandler<GetTransactionStatus20Req, GetTransactionStatus20Conf>(GETTRANSACTIONSTATUS20_ACTION, *messages_converter),
      GenericMessageHandler<GetVariables20Req, GetVariables20Conf>(GETVARIABLES20_ACTION, *messages_converter),
      GenericMessageHandler<InstallCertificate20Req, InstallCertificate20Conf>(INSTALLCERTIFICATE20_ACTION, *messages_converter),
      GenericMessageHandler<PublishFirmware20Req, PublishFirmware20Conf>(PUBLISHFIRMWARE20_ACTION, *messages_converter),
      GenericMessageHandler<RequestStartTransaction20Req, RequestStartTransaction20Conf>(REQUESTSTARTTRANSACTION20_ACTION,
                                                                                         *messages_converter),
      GenericMessageHandler<RequestStopTransaction20Req, RequestStopTransaction20Conf>(REQUESTSTOPTRANSACTION20_ACTION,
                                                                                       *messages_converter),
      GenericMessageHandler<ReserveNow20Req, ReserveNow20Conf>(RESERVENOW20_ACTION, *messages_converter),
      GenericMessageHandler<Reset20Req, Reset20Conf>(RESET20_ACTION, *messages_converter),
      GenericMessageHandler<SendLocalList20Req, SendLocalList20Conf>(SENDLOCALLIST20_ACTION, *messages_converter),
      GenericMessageHandler<SetChargingProfile20Req, SetChargingProfile20Conf>(SETCHARGINGPROFILE20_ACTION, *messages_converter),
      GenericMessageHandler<SetDisplayMessage20Req, SetDisplayMessage20Conf>(SETDISPLAYMESSAGE20_ACTION, *messages_converter),
      GenericMessageHandler<SetMonitoringBase20Req, SetMonitoringBase20Conf>(SETMONITORINGBASE20_ACTION, *messages_converter),
      GenericMessageHandler<SetMonitoringLevel20Req, SetMonitoringLevel20Conf>(SETMONITORINGLEVEL20_ACTION, *messages_converter),
      GenericMessageHandler<SetNetworkProfile20Req, SetNetworkProfile20Conf>(SETNETWORKPROFILE20_ACTION, *messages_converter),
      GenericMessageHandler<SetVariableMonitoring20Req, SetVariableMonitoring20Conf>(SETVARIABLEMONITORING20_ACTION, *messages_converter),
      GenericMessageHandler<SetVariables20Req, SetVariables20Conf>(SETVARIABLES20_ACTION, *messages_converter),
      GenericMessageHandler<TriggerMessage20Req, TriggerMessage20Conf>(TRIGGERMESSAGE20_ACTION, *messages_converter),
      GenericMessageHandler<UnlockConnector20Req, UnlockConnector20Conf>(UNLOCKCONNECTOR20_ACTION, *messages_converter),
      GenericMessageHandler<UnpublishFirmware20Req, UnpublishFirmware20Conf>(UNPUBLISHFIRMWARE20_ACTION, *messages_converter),
      GenericMessageHandler<UpdateFirmware20Req, UpdateFirmware20Conf>(UPDATEFIRMWARE20_ACTION, *messages_converter),

      m_stack_config(stack_config),
      m_events_handler(events_handler),
      m_timer_pool(timer_pool),
      m_worker_pool(worker_pool),
      m_database(),
      m_internal_config(m_database),
      m_messages_converter(std::move(messages_converter)),
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
                *m_rpc_client, *m_messages_converter, m_messages_validator, m_stack_config.callRequestTimeout());

            // Register to Central System messages
            m_msg_dispatcher->registerHandler(CANCELRESERVATION20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<CancelReservation20Req, CancelReservation20Conf>*>(this));
            m_msg_dispatcher->registerHandler(CERTIFICATESIGNED20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<CertificateSigned20Req, CertificateSigned20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                CHANGEAVAILABILITY20_ACTION,
                *dynamic_cast<GenericMessageHandler<ChangeAvailability20Req, ChangeAvailability20Conf>*>(this));
            m_msg_dispatcher->registerHandler(CLEARCACHE20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<ClearCache20Req, ClearCache20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                CLEARCHARGINGPROFILE20_ACTION,
                *dynamic_cast<GenericMessageHandler<ClearChargingProfile20Req, ClearChargingProfile20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                CLEARDISPLAYMESSAGE20_ACTION,
                *dynamic_cast<GenericMessageHandler<ClearDisplayMessage20Req, ClearDisplayMessage20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                CLEARVARIABLEMONITORING20_ACTION,
                *dynamic_cast<GenericMessageHandler<ClearVariableMonitoring20Req, ClearVariableMonitoring20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                CUSTOMERINFORMATION20_ACTION,
                *dynamic_cast<GenericMessageHandler<CustomerInformation20Req, CustomerInformation20Conf>*>(this));
            m_msg_dispatcher->registerHandler(DATATRANSFER20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<DataTransfer20Req, DataTransfer20Conf>*>(this));
            m_msg_dispatcher->registerHandler(DELETECERTIFICATE20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<DeleteCertificate20Req, DeleteCertificate20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                GET15118EVCERTIFICATE20_ACTION,
                *dynamic_cast<GenericMessageHandler<Get15118EVCertificate20Req, Get15118EVCertificate20Conf>*>(this));
            m_msg_dispatcher->registerHandler(GETBASEREPORT20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<GetBaseReport20Req, GetBaseReport20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                GETCERTIFICATESTATUS20_ACTION,
                *dynamic_cast<GenericMessageHandler<GetCertificateStatus20Req, GetCertificateStatus20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                GETCHARGINGPROFILES20_ACTION,
                *dynamic_cast<GenericMessageHandler<GetChargingProfiles20Req, GetChargingProfiles20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                GETCOMPOSITESCHEDULE20_ACTION,
                *dynamic_cast<GenericMessageHandler<GetCompositeSchedule20Req, GetCompositeSchedule20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                GETDISPLAYMESSAGES20_ACTION,
                *dynamic_cast<GenericMessageHandler<GetDisplayMessages20Req, GetDisplayMessages20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                GETINSTALLEDCERTIFICATEIDS20_ACTION,
                *dynamic_cast<GenericMessageHandler<GetInstalledCertificateIds20Req, GetInstalledCertificateIds20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                GETLOCALLISTVERSION20_ACTION,
                *dynamic_cast<GenericMessageHandler<GetLocalListVersion20Req, GetLocalListVersion20Conf>*>(this));
            m_msg_dispatcher->registerHandler(GETLOG20_ACTION, *dynamic_cast<GenericMessageHandler<GetLog20Req, GetLog20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                GETMONITORINGREPORT20_ACTION,
                *dynamic_cast<GenericMessageHandler<GetMonitoringReport20Req, GetMonitoringReport20Conf>*>(this));
            m_msg_dispatcher->registerHandler(GETREPORT20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<GetReport20Req, GetReport20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                GETTRANSACTIONSTATUS20_ACTION,
                *dynamic_cast<GenericMessageHandler<GetTransactionStatus20Req, GetTransactionStatus20Conf>*>(this));
            m_msg_dispatcher->registerHandler(GETVARIABLES20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<GetVariables20Req, GetVariables20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                INSTALLCERTIFICATE20_ACTION,
                *dynamic_cast<GenericMessageHandler<InstallCertificate20Req, InstallCertificate20Conf>*>(this));
            m_msg_dispatcher->registerHandler(PUBLISHFIRMWARE20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<PublishFirmware20Req, PublishFirmware20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                REQUESTSTARTTRANSACTION20_ACTION,
                *dynamic_cast<GenericMessageHandler<RequestStartTransaction20Req, RequestStartTransaction20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                REQUESTSTOPTRANSACTION20_ACTION,
                *dynamic_cast<GenericMessageHandler<RequestStopTransaction20Req, RequestStopTransaction20Conf>*>(this));
            m_msg_dispatcher->registerHandler(RESERVENOW20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<ReserveNow20Req, ReserveNow20Conf>*>(this));
            m_msg_dispatcher->registerHandler(RESET20_ACTION, *dynamic_cast<GenericMessageHandler<Reset20Req, Reset20Conf>*>(this));
            m_msg_dispatcher->registerHandler(SENDLOCALLIST20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<SendLocalList20Req, SendLocalList20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                SETCHARGINGPROFILE20_ACTION,
                *dynamic_cast<GenericMessageHandler<SetChargingProfile20Req, SetChargingProfile20Conf>*>(this));
            m_msg_dispatcher->registerHandler(SETDISPLAYMESSAGE20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<SetDisplayMessage20Req, SetDisplayMessage20Conf>*>(this));
            m_msg_dispatcher->registerHandler(SETMONITORINGBASE20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<SetMonitoringBase20Req, SetMonitoringBase20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                SETMONITORINGLEVEL20_ACTION,
                *dynamic_cast<GenericMessageHandler<SetMonitoringLevel20Req, SetMonitoringLevel20Conf>*>(this));
            m_msg_dispatcher->registerHandler(SETNETWORKPROFILE20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<SetNetworkProfile20Req, SetNetworkProfile20Conf>*>(this));
            m_msg_dispatcher->registerHandler(
                SETVARIABLEMONITORING20_ACTION,
                *dynamic_cast<GenericMessageHandler<SetVariableMonitoring20Req, SetVariableMonitoring20Conf>*>(this));
            m_msg_dispatcher->registerHandler(SETVARIABLES20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<SetVariables20Req, SetVariables20Conf>*>(this));
            m_msg_dispatcher->registerHandler(TRIGGERMESSAGE20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<TriggerMessage20Req, TriggerMessage20Conf>*>(this));
            m_msg_dispatcher->registerHandler(UNLOCKCONNECTOR20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<UnlockConnector20Req, UnlockConnector20Conf>*>(this));
            m_msg_dispatcher->registerHandler(UNPUBLISHFIRMWARE20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<UnpublishFirmware20Req, UnpublishFirmware20Conf>*>(this));
            m_msg_dispatcher->registerHandler(UPDATEFIRMWARE20_ACTION,
                                              *dynamic_cast<GenericMessageHandler<UpdateFirmware20Req, UpdateFirmware20Conf>*>(this));

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

// OCPP handlers

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::CancelReservation20Req& request,
                                  ocpp::messages::ocpp20::CancelReservation20Conf&      response,
                                  std::string&                                          error_code,
                                  std::string&                                          error_message)
{
    return m_events_handler.onCancelReservation20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::CertificateSigned20Req& request,
                                  ocpp::messages::ocpp20::CertificateSigned20Conf&      response,
                                  std::string&                                          error_code,
                                  std::string&                                          error_message)
{
    return m_events_handler.onCertificateSigned20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::ChangeAvailability20Req& request,
                                  ocpp::messages::ocpp20::ChangeAvailability20Conf&      response,
                                  std::string&                                           error_code,
                                  std::string&                                           error_message)
{
    return m_events_handler.onChangeAvailability20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::ClearCache20Req& request,
                                  ocpp::messages::ocpp20::ClearCache20Conf&      response,
                                  std::string&                                   error_code,
                                  std::string&                                   error_message)
{
    return m_events_handler.onClearCache20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::ClearChargingProfile20Req& request,
                                  ocpp::messages::ocpp20::ClearChargingProfile20Conf&      response,
                                  std::string&                                             error_code,
                                  std::string&                                             error_message)
{
    return m_events_handler.onClearChargingProfile20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::ClearDisplayMessage20Req& request,
                                  ocpp::messages::ocpp20::ClearDisplayMessage20Conf&      response,
                                  std::string&                                            error_code,
                                  std::string&                                            error_message)
{
    return m_events_handler.onClearDisplayMessage20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::ClearVariableMonitoring20Req& request,
                                  ocpp::messages::ocpp20::ClearVariableMonitoring20Conf&      response,
                                  std::string&                                                error_code,
                                  std::string&                                                error_message)
{
    return m_events_handler.onClearVariableMonitoring20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::CustomerInformation20Req& request,
                                  ocpp::messages::ocpp20::CustomerInformation20Conf&      response,
                                  std::string&                                            error_code,
                                  std::string&                                            error_message)
{
    return m_events_handler.onCustomerInformation20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::DataTransfer20Req& request,
                                  ocpp::messages::ocpp20::DataTransfer20Conf&      response,
                                  std::string&                                     error_code,
                                  std::string&                                     error_message)
{
    return m_events_handler.onDataTransfer20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::DeleteCertificate20Req& request,
                                  ocpp::messages::ocpp20::DeleteCertificate20Conf&      response,
                                  std::string&                                          error_code,
                                  std::string&                                          error_message)
{
    return m_events_handler.onDeleteCertificate20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::Get15118EVCertificate20Req& request,
                                  ocpp::messages::ocpp20::Get15118EVCertificate20Conf&      response,
                                  std::string&                                              error_code,
                                  std::string&                                              error_message)
{
    return m_events_handler.onGet15118EVCertificate20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetBaseReport20Req& request,
                                  ocpp::messages::ocpp20::GetBaseReport20Conf&      response,
                                  std::string&                                      error_code,
                                  std::string&                                      error_message)
{
    return m_events_handler.onGetBaseReport20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetCertificateStatus20Req& request,
                                  ocpp::messages::ocpp20::GetCertificateStatus20Conf&      response,
                                  std::string&                                             error_code,
                                  std::string&                                             error_message)
{
    return m_events_handler.onGetCertificateStatus20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetChargingProfiles20Req& request,
                                  ocpp::messages::ocpp20::GetChargingProfiles20Conf&      response,
                                  std::string&                                            error_code,
                                  std::string&                                            error_message)
{
    return m_events_handler.onGetChargingProfiles20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetCompositeSchedule20Req& request,
                                  ocpp::messages::ocpp20::GetCompositeSchedule20Conf&      response,
                                  std::string&                                             error_code,
                                  std::string&                                             error_message)
{
    return m_events_handler.onGetCompositeSchedule20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetDisplayMessages20Req& request,
                                  ocpp::messages::ocpp20::GetDisplayMessages20Conf&      response,
                                  std::string&                                           error_code,
                                  std::string&                                           error_message)
{
    return m_events_handler.onGetDisplayMessages20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetInstalledCertificateIds20Req& request,
                                  ocpp::messages::ocpp20::GetInstalledCertificateIds20Conf&      response,
                                  std::string&                                                   error_code,
                                  std::string&                                                   error_message)
{
    return m_events_handler.onGetInstalledCertificateIds20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetLocalListVersion20Req& request,
                                  ocpp::messages::ocpp20::GetLocalListVersion20Conf&      response,
                                  std::string&                                            error_code,
                                  std::string&                                            error_message)
{
    return m_events_handler.onGetLocalListVersion20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetLog20Req& request,
                                  ocpp::messages::ocpp20::GetLog20Conf&      response,
                                  std::string&                               error_code,
                                  std::string&                               error_message)
{
    return m_events_handler.onGetLog20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetMonitoringReport20Req& request,
                                  ocpp::messages::ocpp20::GetMonitoringReport20Conf&      response,
                                  std::string&                                            error_code,
                                  std::string&                                            error_message)
{
    return m_events_handler.onGetMonitoringReport20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetReport20Req& request,
                                  ocpp::messages::ocpp20::GetReport20Conf&      response,
                                  std::string&                                  error_code,
                                  std::string&                                  error_message)
{
    return m_events_handler.onGetReport20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetTransactionStatus20Req& request,
                                  ocpp::messages::ocpp20::GetTransactionStatus20Conf&      response,
                                  std::string&                                             error_code,
                                  std::string&                                             error_message)
{
    return m_events_handler.onGetTransactionStatus20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetVariables20Req& request,
                                  ocpp::messages::ocpp20::GetVariables20Conf&      response,
                                  std::string&                                     error_code,
                                  std::string&                                     error_message)
{
    return m_events_handler.onGetVariables20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::InstallCertificate20Req& request,
                                  ocpp::messages::ocpp20::InstallCertificate20Conf&      response,
                                  std::string&                                           error_code,
                                  std::string&                                           error_message)
{
    return m_events_handler.onInstallCertificate20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::PublishFirmware20Req& request,
                                  ocpp::messages::ocpp20::PublishFirmware20Conf&      response,
                                  std::string&                                        error_code,
                                  std::string&                                        error_message)
{
    return m_events_handler.onPublishFirmware20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::RequestStartTransaction20Req& request,
                                  ocpp::messages::ocpp20::RequestStartTransaction20Conf&      response,
                                  std::string&                                                error_code,
                                  std::string&                                                error_message)
{
    return m_events_handler.onRequestStartTransaction20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::RequestStopTransaction20Req& request,
                                  ocpp::messages::ocpp20::RequestStopTransaction20Conf&      response,
                                  std::string&                                               error_code,
                                  std::string&                                               error_message)
{
    return m_events_handler.onRequestStopTransaction20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::ReserveNow20Req& request,
                                  ocpp::messages::ocpp20::ReserveNow20Conf&      response,
                                  std::string&                                   error_code,
                                  std::string&                                   error_message)
{
    return m_events_handler.onReserveNow20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::Reset20Req& request,
                                  ocpp::messages::ocpp20::Reset20Conf&      response,
                                  std::string&                              error_code,
                                  std::string&                              error_message)
{
    return m_events_handler.onReset20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::SendLocalList20Req& request,
                                  ocpp::messages::ocpp20::SendLocalList20Conf&      response,
                                  std::string&                                      error_code,
                                  std::string&                                      error_message)
{
    return m_events_handler.onSendLocalList20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::SetChargingProfile20Req& request,
                                  ocpp::messages::ocpp20::SetChargingProfile20Conf&      response,
                                  std::string&                                           error_code,
                                  std::string&                                           error_message)
{
    return m_events_handler.onSetChargingProfile20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::SetDisplayMessage20Req& request,
                                  ocpp::messages::ocpp20::SetDisplayMessage20Conf&      response,
                                  std::string&                                          error_code,
                                  std::string&                                          error_message)
{
    return m_events_handler.onSetDisplayMessage20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::SetMonitoringBase20Req& request,
                                  ocpp::messages::ocpp20::SetMonitoringBase20Conf&      response,
                                  std::string&                                          error_code,
                                  std::string&                                          error_message)
{
    return m_events_handler.onSetMonitoringBase20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::SetMonitoringLevel20Req& request,
                                  ocpp::messages::ocpp20::SetMonitoringLevel20Conf&      response,
                                  std::string&                                           error_code,
                                  std::string&                                           error_message)
{
    return m_events_handler.onSetMonitoringLevel20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::SetNetworkProfile20Req& request,
                                  ocpp::messages::ocpp20::SetNetworkProfile20Conf&      response,
                                  std::string&                                          error_code,
                                  std::string&                                          error_message)
{
    return m_events_handler.onSetNetworkProfile20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::SetVariableMonitoring20Req& request,
                                  ocpp::messages::ocpp20::SetVariableMonitoring20Conf&      response,
                                  std::string&                                              error_code,
                                  std::string&                                              error_message)
{
    return m_events_handler.onSetVariableMonitoring20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::SetVariables20Req& request,
                                  ocpp::messages::ocpp20::SetVariables20Conf&      response,
                                  std::string&                                     error_code,
                                  std::string&                                     error_message)
{
    return m_events_handler.onSetVariables20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::TriggerMessage20Req& request,
                                  ocpp::messages::ocpp20::TriggerMessage20Conf&      response,
                                  std::string&                                       error_code,
                                  std::string&                                       error_message)
{
    return m_events_handler.onTriggerMessage20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::UnlockConnector20Req& request,
                                  ocpp::messages::ocpp20::UnlockConnector20Conf&      response,
                                  std::string&                                        error_code,
                                  std::string&                                        error_message)
{
    return m_events_handler.onUnlockConnector20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::UnpublishFirmware20Req& request,
                                  ocpp::messages::ocpp20::UnpublishFirmware20Conf&      response,
                                  std::string&                                          error_code,
                                  std::string&                                          error_message)
{
    return m_events_handler.onUnpublishFirmware20(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::UpdateFirmware20Req& request,
                                  ocpp::messages::ocpp20::UpdateFirmware20Conf&      response,
                                  std::string&                                       error_code,
                                  std::string&                                       error_message)
{
    return m_events_handler.onUpdateFirmware20(request, response, error_code, error_message);
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
