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

#include "CentralSystem.h"
#include "ChargePointProxy.h"
#include "DateTime.h"
#include "ICentralSystemEventsHandler.h"
#include "InternalConfigKeys.h"
#include "Logger.h"
#include "Version.h"
#include "WebsocketFactory.h"

#include <filesystem>
#include <iostream>
#include <random>

using namespace ocpp::types;

namespace ocpp
{
namespace centralsystem
{

/** @brief Instanciate a central system */
std::unique_ptr<ICentralSystem> ICentralSystem::create(const ocpp::config::ICentralSystemConfig& stack_config,
                                                       ICentralSystemEventsHandler&              events_handler)
{
    return std::unique_ptr<ICentralSystem>(new CentralSystem(stack_config, events_handler));
}

/** @brief Constructor */
CentralSystem::CentralSystem(const ocpp::config::ICentralSystemConfig& stack_config, ICentralSystemEventsHandler& events_handler)
    : m_stack_config(stack_config),
      m_events_handler(events_handler),
      m_timer_pool(),
      m_worker_pool(2u), // 1 asynchronous timer operations + 1 for asynchronous responses
      m_database(),
      m_internal_config(m_database),
      m_messages_converter(),
      m_ws_server(),
      m_rpc_server(),
      m_uptime_timer(m_timer_pool, "Uptime timer"),
      m_uptime(0),
      m_total_uptime(0)
{
    // Open database
    if (m_database.open(m_stack_config.databasePath()))
    {
        // Register logger
        if (m_stack_config.logMaxEntriesCount() != 0)
        {
            ocpp::log::Logger::registerDefaultLogger(m_database, m_stack_config.logMaxEntriesCount());
        }

        // Initialize the database
        initDatabase();
    }
    else
    {
        LOG_ERROR << "Unable to open database";
    }

    // Uptime timer
    m_uptime_timer.setCallback(std::bind(&CentralSystem::processUptime, this));

    // Random numbers
    std::srand(time(nullptr));
}

/** @brief Destructor */
CentralSystem::~CentralSystem()
{
    stop();
}

/** @copydoc bool ICentralSystem::resetData() */
bool CentralSystem::resetData()
{
    bool ret = false;

    // Check if it is already started
    if (!m_rpc_server)
    {
        LOG_INFO << "Reset all data";

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
                m_total_uptime = 0;
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

    return ret;
}

/** @copydoc bool ICentralSystem::start() */
bool CentralSystem::start()
{
    bool ret = false;

    // Check if it is already started
    if (!m_rpc_server)
    {
        LOG_INFO << "Starting OCPP stack v" << OPEN_OCPP_VERSION << " - Listen URL : " << m_stack_config.listenUrl();

        // Start uptime counter
        m_uptime = 0;
        m_internal_config.setKey(START_DATE_KEY, DateTime::now().str());
        m_uptime_timer.start(std::chrono::seconds(1u));

        // Allocate resources
        m_ws_server  = std::unique_ptr<ocpp::websockets::IWebsocketServer>(ocpp::websockets::WebsocketFactory::newServer());
        m_rpc_server = std::make_unique<ocpp::rpc::RpcServer>(*m_ws_server, "ocpp1.6");
        m_rpc_server->registerServerListener(*this);

        // Configure websocket link
        ocpp::websockets::IWebsocketServer::Credentials credentials;
        credentials.http_basic_authent                        = m_stack_config.httpBasicAuthent();
        credentials.tls12_cipher_list                         = m_stack_config.tlsv12CipherList();
        credentials.tls13_cipher_list                         = m_stack_config.tlsv13CipherList();
        credentials.ecdh_curve                                = m_stack_config.tlsEcdhCurve();
        credentials.server_certificate                        = m_stack_config.tlsServerCertificate();
        credentials.server_certificate_private_key            = m_stack_config.tlsServerCertificatePrivateKey();
        credentials.server_certificate_private_key_passphrase = m_stack_config.tlsServerCertificatePrivateKeyPassphrase();
        credentials.server_certificate_ca                     = m_stack_config.tlsServerCertificateCa();
        credentials.client_certificate_authent                = m_stack_config.tlsClientCertificateAuthent();
        credentials.encoded_pem_certificates                  = false;

        // Start listening
        ret = m_rpc_server->start(m_stack_config.listenUrl(), credentials, m_stack_config.webSocketPingInterval());
    }
    else
    {
        LOG_ERROR << "Stack already started";
    }

    return ret;
}

/** @copydoc bool ICentralSystem::stop() */
bool CentralSystem::stop()
{
    bool ret = false;

    // Check if it is already started
    if (m_rpc_server)
    {
        LOG_INFO << "Stopping OCPP stack";

        // Stop uptime counter
        m_uptime_timer.stop();
        saveUptime();

        // Stop connection
        ret = m_rpc_server->stop();

        // Free resources
        m_ws_server.reset();
        m_rpc_server.reset();

        // Close database
        m_database.close();
    }
    else
    {
        LOG_ERROR << "Stack already stopped";
    }

    return ret;
}

/** @copydoc bool RpcServer::IListener::rpcCheckCredentials(const std::string&, const std::string&, const std::string&) */
bool CentralSystem::rpcCheckCredentials(const std::string& chargepoint_id, const std::string& user, const std::string& password)
{
    bool ret = false;
    LOG_INFO << "Check credentials for Charge Point [" << chargepoint_id << "]";

    // OCPP protocol force to have user = chargepoint_id
    if (user == chargepoint_id)
    {
        // Check password
        ret = m_events_handler.checkCredentials(chargepoint_id, password);
    }
    return ret;
}

/** @copydoc void RpcServer::IListener::rpcClientConnected(const std::string&, std::shared_ptr<Client>) */
void CentralSystem::rpcClientConnected(const std::string& chargepoint_id, std::shared_ptr<ocpp::rpc::RpcServer::Client> client)
{
    LOG_INFO << "Connection from Charge Point [" << chargepoint_id << "]";

    // Instanciate proxy
    std::shared_ptr<ICentralSystem::IChargePoint> chargepoint(
        new ChargePointProxy(chargepoint_id, client, m_stack_config.jsonSchemasPath(), m_messages_converter, m_stack_config));

    // Notify connection
    m_events_handler.chargePointConnected(chargepoint);
}

/** @copydoc void RpcServer::IListener::rpcServerError() */
void CentralSystem::rpcServerError()
{
    LOG_ERROR << "Critical server error";
}

/** @brief Initialize the database */
void CentralSystem::initDatabase()
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
    if (!m_internal_config.keyExist(TOTAL_UPTIME_KEY))
    {
        m_internal_config.createKey(TOTAL_UPTIME_KEY, "0");
    }
    else
    {
        std::string value;
        m_internal_config.getKey(TOTAL_UPTIME_KEY, value);
        m_total_uptime = std::atoi(value.c_str());
    }
}

/** @brief Process uptime */
void CentralSystem::processUptime()
{
    // Increase counters
    m_uptime++;
    m_total_uptime++;

    // Save counters
    if ((m_uptime % 15u) == 0)
    {
        m_worker_pool.run<void>(std::bind(&CentralSystem::saveUptime, this));
    }
}

/** @brief Save the uptime counter in database */
void CentralSystem::saveUptime()
{
    m_internal_config.setKey(UPTIME_KEY, std::to_string(m_uptime));
    m_internal_config.setKey(TOTAL_UPTIME_KEY, std::to_string(m_total_uptime));
}

} // namespace centralsystem
} // namespace ocpp
