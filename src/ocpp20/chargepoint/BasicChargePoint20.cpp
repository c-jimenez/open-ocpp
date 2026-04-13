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

#include "BasicChargePoint20.h"
#include "InternalConfigKeys.h"
#include "Logger.h"
#include "MessageDispatcher.h"
#include "PrivateKey.h"
#include "StatusManager20.h"
#include "TimerPool.h"
#include "TriggerMessageManager20.h"
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
std::unique_ptr<IBasicChargePoint20> IBasicChargePoint20::create(const ocpp::config::IChargePointConfig20& stack_config,
                                                                 IDeviceModel&                             device_model,
                                                                 IBasicChargePointEventsHandler&           events_handler)
{
    std::shared_ptr<ocpp::helpers::ITimerPool>       timer_pool(new ocpp::helpers::TimerPool());
    std::shared_ptr<ocpp::helpers::WorkerThreadPool> worker_pool =
        std::make_shared<ocpp::helpers::WorkerThreadPool>(2u); // 1 asynchronous timer operations + 1 for asynchronous jobs/responses
    std::unique_ptr<MessagesConverter20> messages_converter(new MessagesConverter20());
    return std::unique_ptr<IBasicChargePoint20>(
        new BasicChargePoint20(stack_config, device_model, events_handler, timer_pool, worker_pool, std::move(messages_converter)));
}

/** @brief Instanciate a charge point with the provided timer and worker pools */
std::unique_ptr<IBasicChargePoint20> IBasicChargePoint20::create(const ocpp::config::IChargePointConfig20&        stack_config,
                                                                 IDeviceModel&                                    device_model,
                                                                 IBasicChargePointEventsHandler&                  events_handler,
                                                                 std::shared_ptr<ocpp::helpers::ITimerPool>       timer_pool,
                                                                 std::shared_ptr<ocpp::helpers::WorkerThreadPool> worker_pool)
{
    std::unique_ptr<MessagesConverter20> messages_converter(new MessagesConverter20());
    return std::unique_ptr<IBasicChargePoint20>(
        new BasicChargePoint20(stack_config, device_model, events_handler, timer_pool, worker_pool, std::move(messages_converter)));
}

/** @brief Constructor */
BasicChargePoint20::BasicChargePoint20(const ocpp::config::IChargePointConfig20&                      stack_config,
                                       IDeviceModel&                                                  device_model,
                                       IBasicChargePointEventsHandler&                                events_handler,
                                       std::shared_ptr<ocpp::helpers::ITimerPool>                     timer_pool,
                                       std::shared_ptr<ocpp::helpers::WorkerThreadPool>               worker_pool,
                                       std::unique_ptr<ocpp::messages::ocpp20::MessagesConverter20>&& messages_converter)
    : m_stack_config(stack_config),
      m_device_model(device_model),
      m_events_handler(events_handler),
      m_timer_pool(timer_pool),
      m_worker_pool(worker_pool),
      m_database(),
      m_internal_config(m_database),
      m_connectors(m_device_model, m_database, *m_timer_pool.get()),
      m_messages_converter(std::move(messages_converter)),
      m_messages_validator(),
      m_stop_in_progress(false),
      m_reconnect_scheduled(false),
      m_ws_client(),
      m_rpc_client(),
      m_msg_dispatcher(),
      m_msg_sender(),
      m_status_manager(),
      m_trigger_manager(),
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
    m_uptime_timer.setCallback(std::bind(&BasicChargePoint20::processUptime, this));

    // Random numbers
    std::srand(static_cast<unsigned int>(time(nullptr)));
}

/** @brief Destructor */
BasicChargePoint20::~BasicChargePoint20()
{
    stop();
}

/** @copydoc bool IBasicChargePoint20::resetData() */
bool BasicChargePoint20::resetData()
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

/** @copydoc bool IBasicChargePoint20::resetConnectorData() */
bool BasicChargePoint20::resetConnectorData()
{
    bool ret = false;

    // Check if it is already started
    if (!m_rpc_client)
    {
        // Reset connector data
        //m_connectors.resetConnectors();
        ret = true;
    }

    return ret;
}

/** @copydoc bool IBasicChargePoint20::start(ocpp::types::ocpp20::BootReasonEnumType) */
bool BasicChargePoint20::start(ocpp::types::ocpp20::BootReasonEnumType boot_reason)
{
    bool ret = false;

    // Check if it is already started
    if (!m_rpc_client)
    {

        LOG_INFO << "Starting OCPP stack v" << OPEN_OCPP_VERSION << " - Central System : " << m_stack_config.connexionUrl()
                 << " - Charge Point identifier : " << m_stack_config.chargePointIdentifier()
                 << " - Boot reason : " << BootReasonEnumTypeHelper.toString(boot_reason);

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

            m_trigger_manager = std::make_unique<TriggerMessageManager>(m_connectors, *m_messages_converter, *m_msg_dispatcher);
            m_status_manager  = std::make_unique<StatusManager>(m_stack_config,
                                                               m_device_model,
                                                               m_events_handler,
                                                               m_internal_config,
                                                               *m_timer_pool,
                                                               *m_worker_pool,
                                                               m_connectors,
                                                               *m_msg_sender,
                                                               *m_trigger_manager,
                                                               boot_reason);

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

/** @copydoc bool IBasicChargePoint20::stop() */
bool BasicChargePoint20::stop()
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

        // Stop managers
        m_trigger_manager.reset();
        m_status_manager.reset();

        // Close database
        m_database.close();
    }
    else
    {
        LOG_ERROR << "Stack already stopped";
    }

    return ret;
}

/** @copydoc bool IBasicChargePoint20::reconnect() */
bool BasicChargePoint20::reconnect()
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

/** @copydoc ocpp::types::ocpp20::RegistrationStatusEnumType IBasicChargePoint20::getRegistrationStatus() */
ocpp::types::ocpp20::RegistrationStatusEnumType BasicChargePoint20::getRegistrationStatus()
{
    RegistrationStatusEnumType status = RegistrationStatusEnumType::Rejected;

    if (m_status_manager)
    {
        status = m_status_manager->getRegistrationStatus();
    }
    else
    {
        LOG_ERROR << "Stack is not started";
    }

    return status;
}

/** @copydoc ocpp::types::ocpp20::ConnectorStatusEnumType IBasicChargePoint20::getConnectorStatus(unsigned int, unsigned int) */
ocpp::types::ocpp20::ConnectorStatusEnumType BasicChargePoint20::getConnectorStatus(unsigned int evse_id, unsigned int connector_id)
{
    ConnectorStatusEnumType status = ConnectorStatusEnumType::Unavailable;

    const Connector* connector = m_connectors.getConnector(evse_id, connector_id);
    if (connector)
    {
        status = connector->status;
    }
    else
    {
        LOG_ERROR << "Invalid connector : EVSE = " << evse_id << ", connector = " << connector_id;
    }

    return status;
}

/** @copydoc bool IBasicChargePoint20::statusNotification(unsigned int, unsigned int, ocpp::types::ocpp20::ConnectorStatusEnumType) */
bool BasicChargePoint20::statusNotification(unsigned int                                 evse_id,
                                            unsigned int                                 connector_id,
                                            ocpp::types::ocpp20::ConnectorStatusEnumType status)
{
    bool ret = false;

    if (m_status_manager)
    {
        ret = m_status_manager->updateConnectorStatus(evse_id, connector_id, status);
    }
    else
    {
        LOG_ERROR << "Stack is not started";
    }

    return ret;
}

// RpcClient::IListener interface

/** @copydoc void RpcClient::IListener::rpcClientConnected() */
void BasicChargePoint20::rpcClientConnected()
{
    LOG_INFO << "Connected to Central System";
    m_status_manager->updateConnectionStatus(true);
    m_events_handler.connectionStateChanged(true);
}

/** @copydoc void RpcClient::IListener::rpcClientFailed() */
void BasicChargePoint20::rpcClientFailed()
{
    LOG_ERROR << "Connection failed with Central System";

    // Get last registration status to determine if offline actions
    // are allowed
    std::string last_status_string;
    m_internal_config.getKey(LAST_REGISTRATION_STATUS_KEY, last_status_string);
    RegistrationStatusEnumType last_status = RegistrationStatusEnumTypeHelper.fromString(last_status_string);
    if (last_status == RegistrationStatusEnumType::Accepted)
    {
        LOG_INFO << "Last registration status was accepted, offline actions are allowed";
        m_status_manager->forceRegistrationStatus(RegistrationStatusEnumType::Accepted);
    }

    // Notify failure
    m_events_handler.connectionFailed(last_status);
}

/** @copydoc void IRpc::IListener::rpcDisconnected() */
void BasicChargePoint20::rpcDisconnected()
{
    // Check if stop is in progress
    if (!m_stop_in_progress)
    {
        LOG_ERROR << "Connection lost with Central System";
        m_status_manager->updateConnectionStatus(false);
        m_events_handler.connectionStateChanged(false);
    }
}

/** @copydoc void IRpc::IListener::rpcError() */
void BasicChargePoint20::rpcError()
{
    LOG_ERROR << "Connection error with Central System";
}

/** @copydoc void IRpc::IListener::rpcCallReceived(const std::string&,
                                                   const rapidjson::Value&,
                                                   rapidjson::Document&,
                                                   std::string&,
                                                   std::string&) */
bool BasicChargePoint20::rpcCallReceived(const std::string&      action,
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
void BasicChargePoint20::rcpMessageReceived(const std::string& msg)
{
    LOG_COM << "RX : " << msg;
}

/** @copydoc void IRpc::ISpy::rcpMessageSent(const std::string&) */
void BasicChargePoint20::rcpMessageSent(const std::string& msg)
{
    LOG_COM << "TX : " << msg;
    m_status_manager->resetHeartBeatTimer();
}

/** @brief Initialize the database */
void BasicChargePoint20::initDatabase()
{
    // Initialize internal configuration
    m_internal_config.initDatabaseTable();
    m_connectors.initDatabaseTable();

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
    if (!m_internal_config.keyExist(LAST_REGISTRATION_STATUS_KEY))
    {
        m_internal_config.createKey(LAST_REGISTRATION_STATUS_KEY,
                                    RegistrationStatusEnumTypeHelper.toString(RegistrationStatusEnumType::Rejected));
    }
}

/** @brief Process uptime */
void BasicChargePoint20::processUptime()
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
        m_worker_pool->run<void>(std::bind(&BasicChargePoint20::saveUptime, this));
    }
}

/** @brief Save the uptime counter in database */
void BasicChargePoint20::saveUptime()
{
    m_internal_config.setKey(UPTIME_KEY, std::to_string(m_uptime));
    m_internal_config.setKey(DISCONNECTED_TIME_KEY, std::to_string(m_disconnected_time));
    m_internal_config.setKey(TOTAL_UPTIME_KEY, std::to_string(m_total_uptime));
    m_internal_config.setKey(TOTAL_DISCONNECTED_TIME_KEY, std::to_string(m_total_disconnected_time));
}

/** @brief Schedule a reconnection to the Central System */
void BasicChargePoint20::scheduleReconnect()
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
bool BasicChargePoint20::doConnect()
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

    // Check if URL has changed since last connection
    std::string last_url;
    m_internal_config.getKey(LAST_CONNECTION_URL_KEY, last_url);
    if (connection_url != last_url)
    {
        // Save URL
        m_internal_config.setKey(LAST_CONNECTION_URL_KEY, connection_url);

        // Reset registration status
        m_internal_config.setKey(LAST_REGISTRATION_STATUS_KEY,
                                 RegistrationStatusEnumTypeHelper.toString(RegistrationStatusEnumType::Rejected));

        LOG_INFO << "Connection URL has changed, reset registration status";
    }

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
