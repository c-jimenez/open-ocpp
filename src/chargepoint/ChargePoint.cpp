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

#include "ChargePoint.h"
#include "AuthentManager.h"
#include "ConfigManager.h"
#include "DataTransferManager.h"
#include "GenericMessageSender.h"
#include "InternalConfigKeys.h"
#include "Logger.h"
#include "MaintenanceManager.h"
#include "MessageDispatcher.h"
#include "MeterValuesManager.h"
#include "RequestFifoManager.h"
#include "ReservationManager.h"
#include "SmartChargingManager.h"
#include "StatusManager.h"
#include "TimerPool.h"
#include "TransactionManager.h"
#include "TriggerMessageManager.h"
#include "Version.h"
#include "WebsocketFactory.h"
#include "WorkerThreadPool.h"

#include <filesystem>
#include <iostream>
#include <random>

using namespace ocpp::types;

namespace ocpp
{
namespace chargepoint
{

/** @brief Instanciate a charge point */
std::unique_ptr<IChargePoint> IChargePoint::create(const ocpp::config::IChargePointConfig& stack_config,
                                                   ocpp::config::IOcppConfig&              ocpp_config,
                                                   IChargePointEventsHandler&              events_handler)
{
    std::shared_ptr<ocpp::helpers::TimerPool>        timer_pool = std::make_shared<ocpp::helpers::TimerPool>();
    std::shared_ptr<ocpp::helpers::WorkerThreadPool> worker_pool =
        std::make_shared<ocpp::helpers::WorkerThreadPool>(2u); // 1 asynchronous timer operations + 1 for asynchronous jobs/responses
    return std::unique_ptr<IChargePoint>(new ChargePoint(stack_config, ocpp_config, events_handler, timer_pool, worker_pool));
}

/** @brief Instanciate a charge point with the provided timer and worker pools */
std::unique_ptr<IChargePoint> IChargePoint::create(const ocpp::config::IChargePointConfig&          stack_config,
                                                   ocpp::config::IOcppConfig&                       ocpp_config,
                                                   IChargePointEventsHandler&                       events_handler,
                                                   std::shared_ptr<ocpp::helpers::TimerPool>        timer_pool,
                                                   std::shared_ptr<ocpp::helpers::WorkerThreadPool> worker_pool)
{
    return std::unique_ptr<IChargePoint>(new ChargePoint(stack_config, ocpp_config, events_handler, timer_pool, worker_pool));
}

/** @brief Constructor */
ChargePoint::ChargePoint(const ocpp::config::IChargePointConfig&          stack_config,
                         ocpp::config::IOcppConfig&                       ocpp_config,
                         IChargePointEventsHandler&                       events_handler,
                         std::shared_ptr<ocpp::helpers::TimerPool>        timer_pool,
                         std::shared_ptr<ocpp::helpers::WorkerThreadPool> worker_pool)
    : m_stack_config(stack_config),
      m_ocpp_config(ocpp_config),
      m_events_handler(events_handler),
      m_timer_pool(timer_pool),
      m_worker_pool(worker_pool),
      m_database(),
      m_internal_config(m_database),
      m_messages_converter(),
      m_requests_fifo(m_database),
      m_security_manager(
          m_stack_config, m_ocpp_config, m_database, m_events_handler, *m_worker_pool.get(), m_messages_converter, m_requests_fifo),
      m_reconnect_scheduled(false),
      m_ws_client(),
      m_rpc_client(),
      m_msg_dispatcher(),
      m_msg_sender(),
      m_connectors(ocpp_config, m_database, *m_timer_pool.get()),
      m_config_manager(),
      m_status_manager(),
      m_authent_manager(),
      m_transaction_manager(),
      m_trigger_manager(),
      m_reservation_manager(),
      m_data_transfer_manager(),
      m_meter_values_manager(),
      m_smart_charging_manager(),
      m_maintenance_manager(),
      m_requests_fifo_manager(),
      m_uptime_timer(*m_timer_pool.get(), "Uptime timer"),
      m_uptime(0),
      m_disconnected_time(0),
      m_total_uptime(0),
      m_total_disconnected_time(0)
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
    m_uptime_timer.setCallback(std::bind(&ChargePoint::processUptime, this));

    // Random numbers
    std::srand(time(nullptr));
}

/** @brief Destructor */
ChargePoint::~ChargePoint()
{
    stop();
}

/** @copydoc bool IChargePoint::resetData() */
bool ChargePoint::resetData()
{
    bool ret = false;

    // Check if it is already started
    if (!m_rpc_client)
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

    return ret;
}

/** @copydoc bool IChargePoint::resetConnectorData() */
bool ChargePoint::resetConnectorData()
{
    bool ret = false;

    // Check if it is already started
    if (!m_rpc_client)
    {
        // Reset connector data
        m_connectors.resetConnectors();
        ret = true;
    }

    return ret;
}

/** @copydoc bool IChargePoint::start() */
bool ChargePoint::start()
{
    bool ret = false;

    // Check if it is already started
    if (!m_rpc_client)
    {

        LOG_INFO << "Starting OCPP stack v" << OPEN_OCPP_VERSION << " - Central System : " << m_stack_config.connexionUrl()
                 << " - Charge Point identifier : " << m_stack_config.chargePointIdentifier();

        // Start uptime counter
        m_uptime            = 0;
        m_disconnected_time = 0;
        m_internal_config.setKey(START_DATE_KEY, DateTime::now().str());
        m_uptime_timer.start(std::chrono::seconds(1u));

        // Allocate resources
        m_ws_client  = std::unique_ptr<ocpp::websockets::IWebsocketClient>(ocpp::websockets::WebsocketFactory::newClient());
        m_rpc_client = std::make_unique<ocpp::rpc::RpcClient>(*m_ws_client, "ocpp1.6");
        m_rpc_client->registerListener(*this);
        m_rpc_client->registerClientListener(*this);
        m_rpc_client->registerSpy(*this);
        m_msg_dispatcher = std::make_unique<ocpp::messages::MessageDispatcher>(m_stack_config.jsonSchemasPath());
        m_msg_sender     = std::make_unique<ocpp::messages::GenericMessageSender>(
            *m_rpc_client, m_messages_converter, m_stack_config.callRequestTimeout());

        m_config_manager  = std::make_unique<ConfigManager>(m_ocpp_config, m_messages_converter, *m_msg_dispatcher);
        m_trigger_manager = std::make_unique<TriggerMessageManager>(m_connectors, m_messages_converter, *m_msg_dispatcher);
        m_authent_manager = std::make_unique<AuthentManager>(
            m_stack_config, m_ocpp_config, m_database, m_internal_config, m_messages_converter, *m_msg_dispatcher, *m_msg_sender);
        m_status_manager         = std::make_unique<StatusManager>(m_stack_config,
                                                           m_ocpp_config,
                                                           m_events_handler,
                                                           m_internal_config,
                                                           *m_timer_pool.get(),
                                                           *m_worker_pool.get(),
                                                           m_connectors,
                                                           *m_msg_dispatcher,
                                                           *m_msg_sender,
                                                           m_messages_converter,
                                                           *m_trigger_manager);
        m_reservation_manager    = std::make_unique<ReservationManager>(m_ocpp_config,
                                                                     m_events_handler,
                                                                     *m_timer_pool.get(),
                                                                     *m_worker_pool.get(),
                                                                     m_connectors,
                                                                     m_messages_converter,
                                                                     *m_msg_dispatcher,
                                                                     *m_status_manager,
                                                                     *m_authent_manager);
        m_meter_values_manager   = std::make_unique<MeterValuesManager>(m_ocpp_config,
                                                                      m_database,
                                                                      m_events_handler,
                                                                      *m_timer_pool.get(),
                                                                      *m_worker_pool.get(),
                                                                      m_connectors,
                                                                      *m_msg_sender,
                                                                      m_requests_fifo,
                                                                      *m_status_manager,
                                                                      *m_trigger_manager,
                                                                      *m_config_manager);
        m_smart_charging_manager = std::make_unique<SmartChargingManager>(m_stack_config,
                                                                          m_ocpp_config,
                                                                          m_database,
                                                                          *m_timer_pool.get(),
                                                                          *m_worker_pool.get(),
                                                                          m_connectors,
                                                                          m_messages_converter,
                                                                          *m_msg_dispatcher);
        m_transaction_manager    = std::make_unique<TransactionManager>(m_ocpp_config,
                                                                     m_events_handler,
                                                                     m_connectors,
                                                                     m_messages_converter,
                                                                     *m_msg_dispatcher,
                                                                     *m_msg_sender,
                                                                     m_requests_fifo,
                                                                     *m_authent_manager,
                                                                     *m_reservation_manager,
                                                                     *m_meter_values_manager,
                                                                     *m_smart_charging_manager);
        m_data_transfer_manager =
            std::make_unique<DataTransferManager>(m_events_handler, m_messages_converter, *m_msg_dispatcher, *m_msg_sender);
        m_maintenance_manager = std::make_unique<MaintenanceManager>(m_stack_config,
                                                                     m_events_handler,
                                                                     *m_worker_pool.get(),
                                                                     m_messages_converter,
                                                                     *m_msg_dispatcher,
                                                                     *m_msg_sender,
                                                                     m_connectors,
                                                                     *m_trigger_manager,
                                                                     m_security_manager);

        m_requests_fifo_manager = std::make_unique<RequestFifoManager>(m_ocpp_config,
                                                                       m_events_handler,
                                                                       *m_timer_pool.get(),
                                                                       *m_worker_pool.get(),
                                                                       m_connectors,
                                                                       *m_msg_sender,
                                                                       m_requests_fifo,
                                                                       *m_status_manager,
                                                                       *m_authent_manager);

        // Register specific configuration checks
        m_config_manager->registerConfigChangedListener("AuthorizationKey", *this);
        m_config_manager->registerConfigChangedListener("SecurityProfile", *this);

        // Start security manager
        m_security_manager.start(*m_msg_sender, *m_msg_dispatcher, *m_trigger_manager, *m_config_manager);

        // Start connection
        ret = doConnect();
    }
    else
    {
        LOG_ERROR << "Stack already started";
    }

    return ret;
}

/** @copydoc bool IChargePoint::stop() */
bool ChargePoint::stop()
{
    bool ret = false;

    // Check if it is already started
    if (m_rpc_client)
    {
        LOG_INFO << "Stopping OCPP stack";

        // Stop uptime counter
        m_uptime_timer.stop();
        saveUptime();

        // Stop managers
        m_config_manager.reset();
        m_authent_manager.reset();
        m_reservation_manager.reset();
        m_trigger_manager.reset();
        m_status_manager.reset();
        m_transaction_manager.reset();
        m_data_transfer_manager.reset();
        m_meter_values_manager.reset();
        m_smart_charging_manager.reset();
        m_maintenance_manager.reset();
        m_requests_fifo_manager.reset();

        // Stop connection
        ret = m_rpc_client->stop();

        // Stop security manager
        m_security_manager.stop();

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

/** @copydoc bool IChargePoint::reconnect() */
bool ChargePoint::reconnect()
{
    bool ret = false;

    // Check if it is started
    if (m_rpc_client)
    {
        // Schedule of reconnexion
        LOG_INFO << "Reconnect asked by user application";
        scheduleReconnect();
        ret = true;
    }
    else
    {
        LOG_ERROR << "Stack stopped";
    }

    return ret;
}
/** @copydoc ocpp::types::RegistrationStatus IChargePoint::getRegistrationStatus() */
ocpp::types::RegistrationStatus ChargePoint::getRegistrationStatus()
{
    RegistrationStatus ret = RegistrationStatus::Rejected;

    if (m_status_manager)
    {
        ret = m_status_manager->getRegistrationStatus();
    }

    return ret;
}

/** @copydoc ocpp::types::ChargePointStatus IChargePoint::getConnectorStatus(unsigned int) */
ocpp::types::ChargePointStatus ChargePoint::getConnectorStatus(unsigned int connector_id)
{
    ChargePointStatus status = ChargePointStatus::Unavailable;

    Connector* connector = m_connectors.getConnector(connector_id);
    if (connector)
    {
        status = connector->status;
    }
    else
    {
        LOG_ERROR << "Invalid connector id : " << connector_id;
    }

    return status;
}

/** @copydoc bool IChargePoint::statusNotification(unsigned int,
 *                                                 ocpp::types::ChargePointStatus,
 *                                                 ocpp::types::ChargePointErrorCode,
 *                                                 const std::string&,
 *                                                 const std::string&,
 *                                                 const std::string&) */
bool ChargePoint::statusNotification(unsigned int                      connector_id,
                                     ocpp::types::ChargePointStatus    status,
                                     ocpp::types::ChargePointErrorCode error_code,
                                     const std::string&                info,
                                     const std::string&                vendor_id,
                                     const std::string&                vendor_error)
{
    bool ret = false;

    if (m_status_manager)
    {
        ret = m_status_manager->updateConnectorStatus(connector_id, status, error_code, info, vendor_id, vendor_error);
    }
    else
    {
        LOG_ERROR << "Stack is not started";
    }

    return ret;
}

/** @copydoc ocpp::types::AuthorizationStatus IChargePoint::authorize(unsigned int, const std::string&, std::string&) */
ocpp::types::AuthorizationStatus ChargePoint::authorize(unsigned int connector_id, const std::string& id_tag, std::string& parent_id)
{
    AuthorizationStatus ret = AuthorizationStatus::Invalid;

    if (m_status_manager)
    {
        if (m_status_manager->getRegistrationStatus() == RegistrationStatus::Accepted)
        {
            Connector* connector = m_connectors.getConnector(connector_id);
            if (connector)
            {
                // Check for reservation
                if (connector->status == ChargePointStatus::Reserved)
                {
                    ret = m_reservation_manager->isTransactionAllowed(connector_id, id_tag);
                }
                else
                {
                    ret = m_authent_manager->authorize(id_tag, parent_id);
                }
            }
        }
        else
        {
            LOG_ERROR << "Charge Point has not been accepted by Central System";
        }
    }
    else
    {
        LOG_ERROR << "Stack is not started";
    }

    return ret;
}

/** @copydoc ocpp::types::AuthorizationStatus IChargePoint::startTransaction(unsigned int, const std::string&) */
ocpp::types::AuthorizationStatus ChargePoint::startTransaction(unsigned int connector_id, const std::string& id_tag)
{
    AuthorizationStatus ret = AuthorizationStatus::Invalid;

    if (m_status_manager)
    {
        if (m_status_manager->getRegistrationStatus() == RegistrationStatus::Accepted)
        {
            ret = m_transaction_manager->startTransaction(connector_id, id_tag);
        }
        else
        {
            LOG_ERROR << "Charge Point has not been accepted by Central System";
        }
    }
    else
    {
        LOG_ERROR << "Stack is not started";
    }

    return ret;
}

/** @copydoc bool IChargePoint::stopTransaction(unsigned int, const std::string&, ocpp::types::Reason) */
bool ChargePoint::stopTransaction(unsigned int connector_id, const std::string& id_tag, ocpp::types::Reason reason)
{
    bool ret = false;

    if (m_status_manager)
    {
        if (m_status_manager->getRegistrationStatus() == RegistrationStatus::Accepted)
        {
            ret = m_transaction_manager->stopTransaction(connector_id, id_tag, reason);
        }
        else
        {
            LOG_ERROR << "Charge Point has not been accepted by Central System";
        }
    }
    else
    {
        LOG_ERROR << "Stack is not started";
    }

    return ret;
}

/** @copydoc bool IChargePoint::dataTransfer(const std::string&,
                                             const std::string&,
                                             const std::string&,
                                             ocpp::types::DataTransferStatus&,
                                             std::string& ) */
bool ChargePoint::dataTransfer(const std::string&               vendor_id,
                               const std::string&               message_id,
                               const std::string&               request_data,
                               ocpp::types::DataTransferStatus& status,
                               std::string&                     response_data)
{
    bool ret = false;

    if (m_status_manager)
    {
        if (m_status_manager->getRegistrationStatus() != RegistrationStatus::Rejected)
        {
            ret = m_data_transfer_manager->dataTransfer(vendor_id, message_id, request_data, status, response_data);
        }
        else
        {
            LOG_ERROR << "Charge Point has not been accepted by Central System";
        }
    }
    else
    {
        LOG_ERROR << "Stack is not started";
    }

    return ret;
}

/** @copydoc bool IChargePoint::sendMeterValues(unsigned int, const std::vector<ocpp::types::MeterValue>&) */
bool ChargePoint::sendMeterValues(unsigned int connector_id, const std::vector<ocpp::types::MeterValue>& values)
{
    bool ret = false;

    if (m_status_manager)
    {
        if (m_status_manager->getRegistrationStatus() != RegistrationStatus::Rejected)
        {
            ret = m_meter_values_manager->sendMeterValues(connector_id, values);
        }
        else
        {
            LOG_ERROR << "Charge Point has not been accepted by Central System";
        }
    }
    else
    {
        LOG_ERROR << "Stack is not started";
    }

    return ret;
}

/** @copydoc bool IChargePoint::getSetpoint(unsigned int,
                                            ocpp::types::Optional<ocpp::types::SmartChargingSetpoint>&,
                                            ocpp::types::Optional<ocpp::types::SmartChargingSetpoint>&,
                                            ocpp::types::ChargingRateUnitType) */
bool ChargePoint::getSetpoint(unsigned int                                               connector_id,
                              ocpp::types::Optional<ocpp::types::SmartChargingSetpoint>& charge_point_setpoint,
                              ocpp::types::Optional<ocpp::types::SmartChargingSetpoint>& connector_setpoint,
                              ocpp::types::ChargingRateUnitType                          unit)
{
    bool ret = false;

    if (m_smart_charging_manager)
    {
        ret = m_smart_charging_manager->getSetpoint(connector_id, charge_point_setpoint, connector_setpoint, unit);
    }
    else
    {
        LOG_ERROR << "Stack is not started";
    }

    return ret;
}

/** @copydoc bool IChargePoint::notifyFirmwareUpdateStatus(bool) */
bool ChargePoint::notifyFirmwareUpdateStatus(bool success)
{
    bool ret = false;

    if (m_status_manager)
    {
        if (m_status_manager->getRegistrationStatus() != RegistrationStatus::Rejected)
        {
            ret = m_maintenance_manager->notifyFirmwareUpdateStatus(success);
        }
        else
        {
            LOG_ERROR << "Charge Point has not been accepted by Central System";
        }
    }
    else
    {
        LOG_ERROR << "Stack is not started";
    }

    return ret;
}

// Security extensions

/** @copydoc bool IChargePoint::logSecurityEvent::logSecurityEvent(const std::string&, const std::string&, bool) */
bool ChargePoint::logSecurityEvent(const std::string& type, const std::string& message, bool critical)
{
    return m_security_manager.logSecurityEvent(type, message, critical);
}

/** @copydoc bool IChargePoint::ISecurityManager::clearSecurityEvents() */
bool ChargePoint::clearSecurityEvents()
{
    return m_security_manager.clearSecurityEvents();
}

/** @copydoc bool IChargePoint::ISecurityManager::signCertificate(const std::string&) */
bool ChargePoint::signCertificate(const std::string& csr)
{
    bool ret = false;

    if (m_status_manager)
    {
        if (m_status_manager->getRegistrationStatus() != RegistrationStatus::Rejected)
        {
            ret = m_security_manager.signCertificate(csr);
        }
        else
        {
            LOG_ERROR << "Charge Point has not been accepted by Central System";
        }
    }
    else
    {
        LOG_ERROR << "Stack is not started";
    }

    return ret;
}

/** @copydoc void RpcClient::IListener::rpcClientConnected() */
void ChargePoint::rpcClientConnected()
{
    LOG_INFO << "Connected to Central System";
    m_status_manager->updateConnectionStatus(true);
    m_requests_fifo_manager->updateConnectionStatus(true);
    m_events_handler.connectionStateChanged(true);
}

/** @copydoc void RpcClient::IListener::rpcClientFailed() */
void ChargePoint::rpcClientFailed()
{
    LOG_ERROR << "Connection failed with Central System";

    // Get last registration status to determine if offline actions
    // are allowed
    std::string last_status_string;
    m_internal_config.getKey(LAST_REGISTRATION_STATUS_KEY, last_status_string);
    RegistrationStatus last_status = RegistrationStatusHelper.fromString(last_status_string);
    if (last_status == RegistrationStatus::Accepted)
    {
        LOG_INFO << "Last registration status was accepted, offline actions are allowed";
        m_status_manager->forceRegistrationStatus(RegistrationStatus::Accepted);
    }

    // Notify failure
    m_events_handler.connectionFailed(last_status);
}

/** @copydoc void IRpc::IListener::rpcDisconnected() */
void ChargePoint::rpcDisconnected()
{
    LOG_ERROR << "Connection lost with Central System";
    m_status_manager->updateConnectionStatus(false);
    m_requests_fifo_manager->updateConnectionStatus(false);
    m_events_handler.connectionStateChanged(false);
}

/** @copydoc void IRpc::IListener::rpcError() */
void ChargePoint::rpcError()
{
    LOG_ERROR << "Connection error with Central System";
}

/** @copydoc void IRpc::IListener::rpcCallReceived(const std::string&,
                                                   const rapidjson::Value&,
                                                   rapidjson::Document&,
                                                   const char*&,
                                                   std::string&) */
bool ChargePoint::rpcCallReceived(const std::string&      action,
                                  const rapidjson::Value& payload,
                                  rapidjson::Document&    response,
                                  const char*&            error_code,
                                  std::string&            error_message)
{
    return m_msg_dispatcher->dispatchMessage(action, payload, response, error_code, error_message);
}

/** @copydoc void IRpc::ISpy::rcpMessageReceived(const std::string&) */
void ChargePoint::rcpMessageReceived(const std::string& msg)
{
    LOG_COM << "RX : " << msg;
}

/** @copydoc void IRpc::ISpy::rcpMessageSent(const std::string&) */
void ChargePoint::rcpMessageSent(const std::string& msg)
{
    m_status_manager->resetHeartBeatTimer();
    LOG_COM << "TX : " << msg;
}

/** @copydoc void IConfigChangedListener::configurationValueChanged(const std::string&) */
void ChargePoint::configurationValueChanged(const std::string& key)
{
    // Check configuration key
    if (key == "AuthorizationKey")
    {
        // Reconnect with new authorization key
        if (m_ocpp_config.securityProfile() != 3)
        {
            LOG_INFO << "AuthorizationKey modified, reconnect with new credentials";
            scheduleReconnect();
        }
    }
    else if (key == "SecurityProfile")
    {
        // Reconnect with new profile
        LOG_INFO << "SecurityProfile modified, reconnect with new security profile";
        scheduleReconnect();
    }
    else
    {
    }
}

/** @brief Initialize the database */
void ChargePoint::initDatabase()
{

    // Initialize internal configuration
    m_internal_config.initDatabaseTable();
    m_connectors.initDatabaseTable();
    m_requests_fifo.initDatabaseTable();
    m_security_manager.initDatabaseTable();

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
        m_total_uptime = std::atoi(value.c_str());
    }
    if (!m_internal_config.keyExist(TOTAL_DISCONNECTED_TIME_KEY))
    {
        m_internal_config.createKey(TOTAL_DISCONNECTED_TIME_KEY, "0");
    }
    else
    {
        std::string value;
        m_internal_config.getKey(TOTAL_DISCONNECTED_TIME_KEY, value);
        m_total_disconnected_time = std::atoi(value.c_str());
    }
    if (!m_internal_config.keyExist(LAST_CONNECTION_URL_KEY))
    {
        m_internal_config.createKey(LAST_CONNECTION_URL_KEY, "");
    }
    if (!m_internal_config.keyExist(LAST_REGISTRATION_STATUS_KEY))
    {
        m_internal_config.createKey(LAST_REGISTRATION_STATUS_KEY, RegistrationStatusHelper.toString(RegistrationStatus::Rejected));
    }
}

/** @brief Process uptime */
void ChargePoint::processUptime()
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
        m_worker_pool->run<void>(std::bind(&ChargePoint::saveUptime, this));
    }
}

/** @brief Save the uptime counter in database */
void ChargePoint::saveUptime()
{
    m_internal_config.setKey(UPTIME_KEY, std::to_string(m_uptime));
    m_internal_config.setKey(DISCONNECTED_TIME_KEY, std::to_string(m_disconnected_time));
    m_internal_config.setKey(TOTAL_UPTIME_KEY, std::to_string(m_total_uptime));
    m_internal_config.setKey(TOTAL_DISCONNECTED_TIME_KEY, std::to_string(m_total_disconnected_time));
}

/** @brief Schedule a reconnection to the Central System */
void ChargePoint::scheduleReconnect()
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
bool ChargePoint::doConnect()
{
    // Check if already connected
    if (m_rpc_client->isConnected())
    {
        // Close connection
        m_rpc_client->stop();
    }

    // Get current security profile
    unsigned int security_profile = m_ocpp_config.securityProfile();

    // Build connection URL
    std::string connection_url = m_stack_config.connexionUrl();
    if (connection_url[connection_url.size() - 1] != '/')
    {
        connection_url += "/";
    }
    connection_url += m_stack_config.chargePointIdentifier();

    // Check if URL has changed since last connection
    std::string last_url;
    m_internal_config.getKey(LAST_CONNECTION_URL_KEY, last_url);
    if (connection_url != last_url)
    {
        // Save URL
        m_internal_config.setKey(LAST_CONNECTION_URL_KEY, connection_url);

        // Reset registration status
        m_internal_config.setKey(LAST_REGISTRATION_STATUS_KEY, RegistrationStatusHelper.toString(RegistrationStatus::Rejected));

        LOG_INFO << "Connection URL has changed, reset registration status";
    }

    // Configure websocket link
    ocpp::websockets::IWebsocketClient::Credentials credentials;

    std::string authorization_key = m_ocpp_config.authorizationKey();
    if (!authorization_key.empty() && (security_profile <= 2))
    {
        credentials.user     = m_stack_config.chargePointIdentifier();
        credentials.password = authorization_key;
    }
    if (security_profile != 1)
    {
        credentials.tls12_cipher_list     = m_stack_config.tlsv12CipherList();
        credentials.tls13_cipher_list     = m_stack_config.tlsv13CipherList();
        credentials.ecdh_curve            = m_stack_config.tlsEcdhCurve();
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
                               m_ocpp_config.webSocketPingInterval());
}

} // namespace chargepoint
} // namespace ocpp
