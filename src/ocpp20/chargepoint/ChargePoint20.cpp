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
    : GenericMessageHandler<CancelReservationReq, CancelReservationConf>(CANCELRESERVATION_ACTION, *messages_converter),
      GenericMessageHandler<CertificateSignedReq, CertificateSignedConf>(CERTIFICATESIGNED_ACTION, *messages_converter),
      GenericMessageHandler<ChangeAvailabilityReq, ChangeAvailabilityConf>(CHANGEAVAILABILITY_ACTION, *messages_converter),
      GenericMessageHandler<ClearCacheReq, ClearCacheConf>(CLEARCACHE_ACTION, *messages_converter),
      GenericMessageHandler<ClearChargingProfileReq, ClearChargingProfileConf>(CLEARCHARGINGPROFILE_ACTION, *messages_converter),
      GenericMessageHandler<ClearDisplayMessageReq, ClearDisplayMessageConf>(CLEARDISPLAYMESSAGE_ACTION, *messages_converter),
      GenericMessageHandler<ClearVariableMonitoringReq, ClearVariableMonitoringConf>(CLEARVARIABLEMONITORING_ACTION, *messages_converter),
      GenericMessageHandler<CustomerInformationReq, CustomerInformationConf>(CUSTOMERINFORMATION_ACTION, *messages_converter),
      GenericMessageHandler<DataTransferReq, DataTransferConf>(DATATRANSFER_ACTION, *messages_converter),
      GenericMessageHandler<DeleteCertificateReq, DeleteCertificateConf>(DELETECERTIFICATE_ACTION, *messages_converter),
      GenericMessageHandler<Get15118EVCertificateReq, Get15118EVCertificateConf>(GET15118EVCERTIFICATE_ACTION, *messages_converter),
      GenericMessageHandler<GetBaseReportReq, GetBaseReportConf>(GETBASEREPORT_ACTION, *messages_converter),
      GenericMessageHandler<GetCertificateStatusReq, GetCertificateStatusConf>(GETCERTIFICATESTATUS_ACTION, *messages_converter),
      GenericMessageHandler<GetChargingProfilesReq, GetChargingProfilesConf>(GETCHARGINGPROFILES_ACTION, *messages_converter),
      GenericMessageHandler<GetCompositeScheduleReq, GetCompositeScheduleConf>(GETCOMPOSITESCHEDULE_ACTION, *messages_converter),
      GenericMessageHandler<GetDisplayMessagesReq, GetDisplayMessagesConf>(GETDISPLAYMESSAGES_ACTION, *messages_converter),
      GenericMessageHandler<GetInstalledCertificateIdsReq, GetInstalledCertificateIdsConf>(GETINSTALLEDCERTIFICATEIDS_ACTION,
                                                                                           *messages_converter),
      GenericMessageHandler<GetLocalListVersionReq, GetLocalListVersionConf>(GETLOCALLISTVERSION_ACTION, *messages_converter),
      GenericMessageHandler<GetLogReq, GetLogConf>(GETLOG_ACTION, *messages_converter),
      GenericMessageHandler<GetMonitoringReportReq, GetMonitoringReportConf>(GETMONITORINGREPORT_ACTION, *messages_converter),
      GenericMessageHandler<GetReportReq, GetReportConf>(GETREPORT_ACTION, *messages_converter),
      GenericMessageHandler<GetTransactionStatusReq, GetTransactionStatusConf>(GETTRANSACTIONSTATUS_ACTION, *messages_converter),
      GenericMessageHandler<GetVariablesReq, GetVariablesConf>(GETVARIABLES_ACTION, *messages_converter),
      GenericMessageHandler<InstallCertificateReq, InstallCertificateConf>(INSTALLCERTIFICATE_ACTION, *messages_converter),
      GenericMessageHandler<PublishFirmwareReq, PublishFirmwareConf>(PUBLISHFIRMWARE_ACTION, *messages_converter),
      GenericMessageHandler<RequestStartTransactionReq, RequestStartTransactionConf>(REQUESTSTARTTRANSACTION_ACTION, *messages_converter),
      GenericMessageHandler<RequestStopTransactionReq, RequestStopTransactionConf>(REQUESTSTOPTRANSACTION_ACTION, *messages_converter),
      GenericMessageHandler<ReserveNowReq, ReserveNowConf>(RESERVENOW_ACTION, *messages_converter),
      GenericMessageHandler<ResetReq, ResetConf>(RESET_ACTION, *messages_converter),
      GenericMessageHandler<SendLocalListReq, SendLocalListConf>(SENDLOCALLIST_ACTION, *messages_converter),
      GenericMessageHandler<SetChargingProfileReq, SetChargingProfileConf>(SETCHARGINGPROFILE_ACTION, *messages_converter),
      GenericMessageHandler<SetDisplayMessageReq, SetDisplayMessageConf>(SETDISPLAYMESSAGE_ACTION, *messages_converter),
      GenericMessageHandler<SetMonitoringBaseReq, SetMonitoringBaseConf>(SETMONITORINGBASE_ACTION, *messages_converter),
      GenericMessageHandler<SetMonitoringLevelReq, SetMonitoringLevelConf>(SETMONITORINGLEVEL_ACTION, *messages_converter),
      GenericMessageHandler<SetNetworkProfileReq, SetNetworkProfileConf>(SETNETWORKPROFILE_ACTION, *messages_converter),
      GenericMessageHandler<SetVariableMonitoringReq, SetVariableMonitoringConf>(SETVARIABLEMONITORING_ACTION, *messages_converter),
      GenericMessageHandler<SetVariablesReq, SetVariablesConf>(SETVARIABLES_ACTION, *messages_converter),
      GenericMessageHandler<TriggerMessageReq, TriggerMessageConf>(TRIGGERMESSAGE_ACTION, *messages_converter),
      GenericMessageHandler<UnlockConnectorReq, UnlockConnectorConf>(UNLOCKCONNECTOR_ACTION, *messages_converter),
      GenericMessageHandler<UnpublishFirmwareReq, UnpublishFirmwareConf>(UNPUBLISHFIRMWARE_ACTION, *messages_converter),
      GenericMessageHandler<UpdateFirmwareReq, UpdateFirmwareConf>(UPDATEFIRMWARE_ACTION, *messages_converter),

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
            m_msg_dispatcher->registerHandler(CANCELRESERVATION_ACTION,
                                              *dynamic_cast<GenericMessageHandler<CancelReservationReq, CancelReservationConf>*>(this));
            m_msg_dispatcher->registerHandler(CERTIFICATESIGNED_ACTION,
                                              *dynamic_cast<GenericMessageHandler<CertificateSignedReq, CertificateSignedConf>*>(this));
            m_msg_dispatcher->registerHandler(CHANGEAVAILABILITY_ACTION,
                                              *dynamic_cast<GenericMessageHandler<ChangeAvailabilityReq, ChangeAvailabilityConf>*>(this));
            m_msg_dispatcher->registerHandler(CLEARCACHE_ACTION,
                                              *dynamic_cast<GenericMessageHandler<ClearCacheReq, ClearCacheConf>*>(this));
            m_msg_dispatcher->registerHandler(
                CLEARCHARGINGPROFILE_ACTION,
                *dynamic_cast<GenericMessageHandler<ClearChargingProfileReq, ClearChargingProfileConf>*>(this));
            m_msg_dispatcher->registerHandler(CLEARDISPLAYMESSAGE_ACTION,
                                              *dynamic_cast<GenericMessageHandler<ClearDisplayMessageReq, ClearDisplayMessageConf>*>(this));
            m_msg_dispatcher->registerHandler(
                CLEARVARIABLEMONITORING_ACTION,
                *dynamic_cast<GenericMessageHandler<ClearVariableMonitoringReq, ClearVariableMonitoringConf>*>(this));
            m_msg_dispatcher->registerHandler(CUSTOMERINFORMATION_ACTION,
                                              *dynamic_cast<GenericMessageHandler<CustomerInformationReq, CustomerInformationConf>*>(this));
            m_msg_dispatcher->registerHandler(DATATRANSFER_ACTION,
                                              *dynamic_cast<GenericMessageHandler<DataTransferReq, DataTransferConf>*>(this));
            m_msg_dispatcher->registerHandler(DELETECERTIFICATE_ACTION,
                                              *dynamic_cast<GenericMessageHandler<DeleteCertificateReq, DeleteCertificateConf>*>(this));
            m_msg_dispatcher->registerHandler(
                GET15118EVCERTIFICATE_ACTION,
                *dynamic_cast<GenericMessageHandler<Get15118EVCertificateReq, Get15118EVCertificateConf>*>(this));
            m_msg_dispatcher->registerHandler(GETBASEREPORT_ACTION,
                                              *dynamic_cast<GenericMessageHandler<GetBaseReportReq, GetBaseReportConf>*>(this));
            m_msg_dispatcher->registerHandler(
                GETCERTIFICATESTATUS_ACTION,
                *dynamic_cast<GenericMessageHandler<GetCertificateStatusReq, GetCertificateStatusConf>*>(this));
            m_msg_dispatcher->registerHandler(GETCHARGINGPROFILES_ACTION,
                                              *dynamic_cast<GenericMessageHandler<GetChargingProfilesReq, GetChargingProfilesConf>*>(this));
            m_msg_dispatcher->registerHandler(
                GETCOMPOSITESCHEDULE_ACTION,
                *dynamic_cast<GenericMessageHandler<GetCompositeScheduleReq, GetCompositeScheduleConf>*>(this));
            m_msg_dispatcher->registerHandler(GETDISPLAYMESSAGES_ACTION,
                                              *dynamic_cast<GenericMessageHandler<GetDisplayMessagesReq, GetDisplayMessagesConf>*>(this));
            m_msg_dispatcher->registerHandler(
                GETINSTALLEDCERTIFICATEIDS_ACTION,
                *dynamic_cast<GenericMessageHandler<GetInstalledCertificateIdsReq, GetInstalledCertificateIdsConf>*>(this));
            m_msg_dispatcher->registerHandler(GETLOCALLISTVERSION_ACTION,
                                              *dynamic_cast<GenericMessageHandler<GetLocalListVersionReq, GetLocalListVersionConf>*>(this));
            m_msg_dispatcher->registerHandler(GETLOG_ACTION, *dynamic_cast<GenericMessageHandler<GetLogReq, GetLogConf>*>(this));
            m_msg_dispatcher->registerHandler(GETMONITORINGREPORT_ACTION,
                                              *dynamic_cast<GenericMessageHandler<GetMonitoringReportReq, GetMonitoringReportConf>*>(this));
            m_msg_dispatcher->registerHandler(GETREPORT_ACTION, *dynamic_cast<GenericMessageHandler<GetReportReq, GetReportConf>*>(this));
            m_msg_dispatcher->registerHandler(
                GETTRANSACTIONSTATUS_ACTION,
                *dynamic_cast<GenericMessageHandler<GetTransactionStatusReq, GetTransactionStatusConf>*>(this));
            m_msg_dispatcher->registerHandler(GETVARIABLES_ACTION,
                                              *dynamic_cast<GenericMessageHandler<GetVariablesReq, GetVariablesConf>*>(this));
            m_msg_dispatcher->registerHandler(INSTALLCERTIFICATE_ACTION,
                                              *dynamic_cast<GenericMessageHandler<InstallCertificateReq, InstallCertificateConf>*>(this));
            m_msg_dispatcher->registerHandler(PUBLISHFIRMWARE_ACTION,
                                              *dynamic_cast<GenericMessageHandler<PublishFirmwareReq, PublishFirmwareConf>*>(this));
            m_msg_dispatcher->registerHandler(
                REQUESTSTARTTRANSACTION_ACTION,
                *dynamic_cast<GenericMessageHandler<RequestStartTransactionReq, RequestStartTransactionConf>*>(this));
            m_msg_dispatcher->registerHandler(
                REQUESTSTOPTRANSACTION_ACTION,
                *dynamic_cast<GenericMessageHandler<RequestStopTransactionReq, RequestStopTransactionConf>*>(this));
            m_msg_dispatcher->registerHandler(RESERVENOW_ACTION,
                                              *dynamic_cast<GenericMessageHandler<ReserveNowReq, ReserveNowConf>*>(this));
            m_msg_dispatcher->registerHandler(RESET_ACTION, *dynamic_cast<GenericMessageHandler<ResetReq, ResetConf>*>(this));
            m_msg_dispatcher->registerHandler(SENDLOCALLIST_ACTION,
                                              *dynamic_cast<GenericMessageHandler<SendLocalListReq, SendLocalListConf>*>(this));
            m_msg_dispatcher->registerHandler(SETCHARGINGPROFILE_ACTION,
                                              *dynamic_cast<GenericMessageHandler<SetChargingProfileReq, SetChargingProfileConf>*>(this));
            m_msg_dispatcher->registerHandler(SETDISPLAYMESSAGE_ACTION,
                                              *dynamic_cast<GenericMessageHandler<SetDisplayMessageReq, SetDisplayMessageConf>*>(this));
            m_msg_dispatcher->registerHandler(SETMONITORINGBASE_ACTION,
                                              *dynamic_cast<GenericMessageHandler<SetMonitoringBaseReq, SetMonitoringBaseConf>*>(this));
            m_msg_dispatcher->registerHandler(SETMONITORINGLEVEL_ACTION,
                                              *dynamic_cast<GenericMessageHandler<SetMonitoringLevelReq, SetMonitoringLevelConf>*>(this));
            m_msg_dispatcher->registerHandler(SETNETWORKPROFILE_ACTION,
                                              *dynamic_cast<GenericMessageHandler<SetNetworkProfileReq, SetNetworkProfileConf>*>(this));
            m_msg_dispatcher->registerHandler(
                SETVARIABLEMONITORING_ACTION,
                *dynamic_cast<GenericMessageHandler<SetVariableMonitoringReq, SetVariableMonitoringConf>*>(this));
            m_msg_dispatcher->registerHandler(SETVARIABLES_ACTION,
                                              *dynamic_cast<GenericMessageHandler<SetVariablesReq, SetVariablesConf>*>(this));
            m_msg_dispatcher->registerHandler(TRIGGERMESSAGE_ACTION,
                                              *dynamic_cast<GenericMessageHandler<TriggerMessageReq, TriggerMessageConf>*>(this));
            m_msg_dispatcher->registerHandler(UNLOCKCONNECTOR_ACTION,
                                              *dynamic_cast<GenericMessageHandler<UnlockConnectorReq, UnlockConnectorConf>*>(this));
            m_msg_dispatcher->registerHandler(UNPUBLISHFIRMWARE_ACTION,
                                              *dynamic_cast<GenericMessageHandler<UnpublishFirmwareReq, UnpublishFirmwareConf>*>(this));
            m_msg_dispatcher->registerHandler(UPDATEFIRMWARE_ACTION,
                                              *dynamic_cast<GenericMessageHandler<UpdateFirmwareReq, UpdateFirmwareConf>*>(this));

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

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::BootNotificationReq&,
 *                                          ocpp::messages::ocpp20::BootNotificationConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::BootNotificationReq& request,
                         ocpp::messages::ocpp20::BootNotificationConf&      response,
                         std::string&                                       error,
                         std::string&                                       message)
{
    return call(BOOTNOTIFICATION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::AuthorizeReq&,
 *                                          ocpp::messages::ocpp20::AuthorizeConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::AuthorizeReq& request,
                         ocpp::messages::ocpp20::AuthorizeConf&      response,
                         std::string&                                error,
                         std::string&                                message)
{
    return call(AUTHORIZE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::ClearedChargingLimitReq&,
 *                                          ocpp::messages::ocpp20::ClearedChargingLimitConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::ClearedChargingLimitReq& request,
                         ocpp::messages::ocpp20::ClearedChargingLimitConf&      response,
                         std::string&                                           error,
                         std::string&                                           message)
{
    return call(CLEAREDCHARGINGLIMIT_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::CostUpdatedReq&,
 *                                          ocpp::messages::ocpp20::CostUpdatedConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::CostUpdatedReq& request,
                         ocpp::messages::ocpp20::CostUpdatedConf&      response,
                         std::string&                                  error,
                         std::string&                                  message)
{
    return call(COSTUPDATED_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::DataTransferReq&,
 *                                          ocpp::messages::ocpp20::DataTransferConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::DataTransferReq& request,
                         ocpp::messages::ocpp20::DataTransferConf&      response,
                         std::string&                                   error,
                         std::string&                                   message)
{
    return call(DATATRANSFER_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::FirmwareStatusNotificationReq&,
 *                                          ocpp::messages::ocpp20::FirmwareStatusNotificationConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::FirmwareStatusNotificationReq& request,
                         ocpp::messages::ocpp20::FirmwareStatusNotificationConf&      response,
                         std::string&                                                 error,
                         std::string&                                                 message)
{
    return call(FIRMWARESTATUSNOTIFICATION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::HeartbeatReq&,
 *                                          ocpp::messages::ocpp20::HeartbeatConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::HeartbeatReq& request,
                         ocpp::messages::ocpp20::HeartbeatConf&      response,
                         std::string&                                error,
                         std::string&                                message)
{
    return call(HEARTBEAT_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::LogStatusNotificationReq&,
 *                                          ocpp::messages::ocpp20::LogStatusNotificationConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::LogStatusNotificationReq& request,
                         ocpp::messages::ocpp20::LogStatusNotificationConf&      response,
                         std::string&                                            error,
                         std::string&                                            message)
{
    return call(LOGSTATUSNOTIFICATION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::MeterValuesReq&,
 *                                          ocpp::messages::ocpp20::MeterValuesConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::MeterValuesReq& request,
                         ocpp::messages::ocpp20::MeterValuesConf&      response,
                         std::string&                                  error,
                         std::string&                                  message)
{
    return call(METERVALUES_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::NotifyChargingLimitReq&,
 *                                          ocpp::messages::ocpp20::NotifyChargingLimitConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::NotifyChargingLimitReq& request,
                         ocpp::messages::ocpp20::NotifyChargingLimitConf&      response,
                         std::string&                                          error,
                         std::string&                                          message)
{
    return call(NOTIFYCHARGINGLIMIT_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::NotifyCustomerInformationReq&,
 *                                          ocpp::messages::ocpp20::NotifyCustomerInformationConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::NotifyCustomerInformationReq& request,
                         ocpp::messages::ocpp20::NotifyCustomerInformationConf&      response,
                         std::string&                                                error,
                         std::string&                                                message)
{
    return call(NOTIFYCUSTOMERINFORMATION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::NotifyDisplayMessagesReq&,
 *                                          ocpp::messages::ocpp20::NotifyDisplayMessagesConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::NotifyDisplayMessagesReq& request,
                         ocpp::messages::ocpp20::NotifyDisplayMessagesConf&      response,
                         std::string&                                            error,
                         std::string&                                            message)
{
    return call(NOTIFYDISPLAYMESSAGES_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::NotifyEVChargingNeedsReq&,
 *                                          ocpp::messages::ocpp20::NotifyEVChargingNeedsConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::NotifyEVChargingNeedsReq& request,
                         ocpp::messages::ocpp20::NotifyEVChargingNeedsConf&      response,
                         std::string&                                            error,
                         std::string&                                            message)
{
    return call(NOTIFYEVCHARGINGNEEDS_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::NotifyEVChargingScheduleReq&,
 *                                          ocpp::messages::ocpp20::NotifyEVChargingScheduleConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::NotifyEVChargingScheduleReq& request,
                         ocpp::messages::ocpp20::NotifyEVChargingScheduleConf&      response,
                         std::string&                                               error,
                         std::string&                                               message)
{
    return call(NOTIFYEVCHARGINGSCHEDULE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::NotifyEventReq&,
 *                                          ocpp::messages::ocpp20::NotifyEventConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::NotifyEventReq& request,
                         ocpp::messages::ocpp20::NotifyEventConf&      response,
                         std::string&                                  error,
                         std::string&                                  message)
{
    return call(NOTIFYEVENT_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::NotifyMonitoringReportReq&,
 *                                          ocpp::messages::ocpp20::NotifyMonitoringReportConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::NotifyMonitoringReportReq& request,
                         ocpp::messages::ocpp20::NotifyMonitoringReportConf&      response,
                         std::string&                                             error,
                         std::string&                                             message)
{
    return call(NOTIFYMONITORINGREPORT_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::NotifyReportReq&,
 *                                          ocpp::messages::ocpp20::NotifyReportConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::NotifyReportReq& request,
                         ocpp::messages::ocpp20::NotifyReportConf&      response,
                         std::string&                                   error,
                         std::string&                                   message)
{
    return call(NOTIFYREPORT_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::PublishFirmwareStatusNotificationReq&,
 *                                          ocpp::messages::ocpp20::PublishFirmwareStatusNotificationConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::PublishFirmwareStatusNotificationReq& request,
                         ocpp::messages::ocpp20::PublishFirmwareStatusNotificationConf&      response,
                         std::string&                                                        error,
                         std::string&                                                        message)
{
    return call(PUBLISHFIRMWARESTATUSNOTIFICATION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::ReportChargingProfilesReq&,
 *                                          ocpp::messages::ocpp20::ReportChargingProfilesConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::ReportChargingProfilesReq& request,
                         ocpp::messages::ocpp20::ReportChargingProfilesConf&      response,
                         std::string&                                             error,
                         std::string&                                             message)
{
    return call(REPORTCHARGINGPROFILES_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::ReservationStatusUpdateReq&,
 *                                          ocpp::messages::ocpp20::ReservationStatusUpdateConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::ReservationStatusUpdateReq& request,
                         ocpp::messages::ocpp20::ReservationStatusUpdateConf&      response,
                         std::string&                                              error,
                         std::string&                                              message)
{
    return call(RESERVATIONSTATUSUPDATE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::SecurityEventNotificationReq&,
 *                                          ocpp::messages::ocpp20::SecurityEventNotificationConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::SecurityEventNotificationReq& request,
                         ocpp::messages::ocpp20::SecurityEventNotificationConf&      response,
                         std::string&                                                error,
                         std::string&                                                message)
{
    return call(SECURITYEVENTNOTIFICATION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::SignCertificateReq&,
 *                                          ocpp::messages::ocpp20::SignCertificateConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::SignCertificateReq& request,
                         ocpp::messages::ocpp20::SignCertificateConf&      response,
                         std::string&                                      error,
                         std::string&                                      message)
{
    return call(SIGNCERTIFICATE_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::StatusNotificationReq&,
 *                                          ocpp::messages::ocpp20::StatusNotificationConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::StatusNotificationReq& request,
                         ocpp::messages::ocpp20::StatusNotificationConf&      response,
                         std::string&                                         error,
                         std::string&                                         message)
{
    return call(STATUSNOTIFICATION_ACTION, request, response, error, message);
}

/** @copydoc bool IChargePoint20::call(const ocpp::messages::ocpp20::TransactionEventReq&,
 *                                          ocpp::messages::ocpp20::TransactionEventConf&,
 *                                          std::string&,
 *                                          std::string&) */
bool ChargePoint20::call(const ocpp::messages::ocpp20::TransactionEventReq& request,
                         ocpp::messages::ocpp20::TransactionEventConf&      response,
                         std::string&                                       error,
                         std::string&                                       message)
{
    return call(TRANSACTIONEVENT_ACTION, request, response, error, message);
}

// OCPP handlers

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::CancelReservationReq& request,
                                  ocpp::messages::ocpp20::CancelReservationConf&      response,
                                  std::string&                                        error_code,
                                  std::string&                                        error_message)
{
    return m_events_handler.onCancelReservation(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::CertificateSignedReq& request,
                                  ocpp::messages::ocpp20::CertificateSignedConf&      response,
                                  std::string&                                        error_code,
                                  std::string&                                        error_message)
{
    return m_events_handler.onCertificateSigned(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::ChangeAvailabilityReq& request,
                                  ocpp::messages::ocpp20::ChangeAvailabilityConf&      response,
                                  std::string&                                         error_code,
                                  std::string&                                         error_message)
{
    return m_events_handler.onChangeAvailability(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::ClearCacheReq& request,
                                  ocpp::messages::ocpp20::ClearCacheConf&      response,
                                  std::string&                                 error_code,
                                  std::string&                                 error_message)
{
    return m_events_handler.onClearCache(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::ClearChargingProfileReq& request,
                                  ocpp::messages::ocpp20::ClearChargingProfileConf&      response,
                                  std::string&                                           error_code,
                                  std::string&                                           error_message)
{
    return m_events_handler.onClearChargingProfile(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::ClearDisplayMessageReq& request,
                                  ocpp::messages::ocpp20::ClearDisplayMessageConf&      response,
                                  std::string&                                          error_code,
                                  std::string&                                          error_message)
{
    return m_events_handler.onClearDisplayMessage(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::ClearVariableMonitoringReq& request,
                                  ocpp::messages::ocpp20::ClearVariableMonitoringConf&      response,
                                  std::string&                                              error_code,
                                  std::string&                                              error_message)
{
    return m_events_handler.onClearVariableMonitoring(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::CustomerInformationReq& request,
                                  ocpp::messages::ocpp20::CustomerInformationConf&      response,
                                  std::string&                                          error_code,
                                  std::string&                                          error_message)
{
    return m_events_handler.onCustomerInformation(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::DataTransferReq& request,
                                  ocpp::messages::ocpp20::DataTransferConf&      response,
                                  std::string&                                   error_code,
                                  std::string&                                   error_message)
{
    return m_events_handler.onDataTransfer(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::DeleteCertificateReq& request,
                                  ocpp::messages::ocpp20::DeleteCertificateConf&      response,
                                  std::string&                                        error_code,
                                  std::string&                                        error_message)
{
    return m_events_handler.onDeleteCertificate(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::Get15118EVCertificateReq& request,
                                  ocpp::messages::ocpp20::Get15118EVCertificateConf&      response,
                                  std::string&                                            error_code,
                                  std::string&                                            error_message)
{
    return m_events_handler.onGet15118EVCertificate(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetBaseReportReq& request,
                                  ocpp::messages::ocpp20::GetBaseReportConf&      response,
                                  std::string&                                    error_code,
                                  std::string&                                    error_message)
{
    return m_events_handler.onGetBaseReport(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetCertificateStatusReq& request,
                                  ocpp::messages::ocpp20::GetCertificateStatusConf&      response,
                                  std::string&                                           error_code,
                                  std::string&                                           error_message)
{
    return m_events_handler.onGetCertificateStatus(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetChargingProfilesReq& request,
                                  ocpp::messages::ocpp20::GetChargingProfilesConf&      response,
                                  std::string&                                          error_code,
                                  std::string&                                          error_message)
{
    return m_events_handler.onGetChargingProfiles(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetCompositeScheduleReq& request,
                                  ocpp::messages::ocpp20::GetCompositeScheduleConf&      response,
                                  std::string&                                           error_code,
                                  std::string&                                           error_message)
{
    return m_events_handler.onGetCompositeSchedule(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetDisplayMessagesReq& request,
                                  ocpp::messages::ocpp20::GetDisplayMessagesConf&      response,
                                  std::string&                                         error_code,
                                  std::string&                                         error_message)
{
    return m_events_handler.onGetDisplayMessages(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetInstalledCertificateIdsReq& request,
                                  ocpp::messages::ocpp20::GetInstalledCertificateIdsConf&      response,
                                  std::string&                                                 error_code,
                                  std::string&                                                 error_message)
{
    return m_events_handler.onGetInstalledCertificateIds(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetLocalListVersionReq& request,
                                  ocpp::messages::ocpp20::GetLocalListVersionConf&      response,
                                  std::string&                                          error_code,
                                  std::string&                                          error_message)
{
    return m_events_handler.onGetLocalListVersion(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetLogReq& request,
                                  ocpp::messages::ocpp20::GetLogConf&      response,
                                  std::string&                             error_code,
                                  std::string&                             error_message)
{
    return m_events_handler.onGetLog(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetMonitoringReportReq& request,
                                  ocpp::messages::ocpp20::GetMonitoringReportConf&      response,
                                  std::string&                                          error_code,
                                  std::string&                                          error_message)
{
    return m_events_handler.onGetMonitoringReport(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetReportReq& request,
                                  ocpp::messages::ocpp20::GetReportConf&      response,
                                  std::string&                                error_code,
                                  std::string&                                error_message)
{
    return m_events_handler.onGetReport(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetTransactionStatusReq& request,
                                  ocpp::messages::ocpp20::GetTransactionStatusConf&      response,
                                  std::string&                                           error_code,
                                  std::string&                                           error_message)
{
    return m_events_handler.onGetTransactionStatus(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::GetVariablesReq& request,
                                  ocpp::messages::ocpp20::GetVariablesConf&      response,
                                  std::string&                                   error_code,
                                  std::string&                                   error_message)
{
    return m_events_handler.onGetVariables(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::InstallCertificateReq& request,
                                  ocpp::messages::ocpp20::InstallCertificateConf&      response,
                                  std::string&                                         error_code,
                                  std::string&                                         error_message)
{
    return m_events_handler.onInstallCertificate(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::PublishFirmwareReq& request,
                                  ocpp::messages::ocpp20::PublishFirmwareConf&      response,
                                  std::string&                                      error_code,
                                  std::string&                                      error_message)
{
    return m_events_handler.onPublishFirmware(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::RequestStartTransactionReq& request,
                                  ocpp::messages::ocpp20::RequestStartTransactionConf&      response,
                                  std::string&                                              error_code,
                                  std::string&                                              error_message)
{
    return m_events_handler.onRequestStartTransaction(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::RequestStopTransactionReq& request,
                                  ocpp::messages::ocpp20::RequestStopTransactionConf&      response,
                                  std::string&                                             error_code,
                                  std::string&                                             error_message)
{
    return m_events_handler.onRequestStopTransaction(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::ReserveNowReq& request,
                                  ocpp::messages::ocpp20::ReserveNowConf&      response,
                                  std::string&                                 error_code,
                                  std::string&                                 error_message)
{
    return m_events_handler.onReserveNow(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::ResetReq& request,
                                  ocpp::messages::ocpp20::ResetConf&      response,
                                  std::string&                            error_code,
                                  std::string&                            error_message)
{
    return m_events_handler.onReset(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::SendLocalListReq& request,
                                  ocpp::messages::ocpp20::SendLocalListConf&      response,
                                  std::string&                                    error_code,
                                  std::string&                                    error_message)
{
    return m_events_handler.onSendLocalList(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::SetChargingProfileReq& request,
                                  ocpp::messages::ocpp20::SetChargingProfileConf&      response,
                                  std::string&                                         error_code,
                                  std::string&                                         error_message)
{
    return m_events_handler.onSetChargingProfile(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::SetDisplayMessageReq& request,
                                  ocpp::messages::ocpp20::SetDisplayMessageConf&      response,
                                  std::string&                                        error_code,
                                  std::string&                                        error_message)
{
    return m_events_handler.onSetDisplayMessage(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::SetMonitoringBaseReq& request,
                                  ocpp::messages::ocpp20::SetMonitoringBaseConf&      response,
                                  std::string&                                        error_code,
                                  std::string&                                        error_message)
{
    return m_events_handler.onSetMonitoringBase(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::SetMonitoringLevelReq& request,
                                  ocpp::messages::ocpp20::SetMonitoringLevelConf&      response,
                                  std::string&                                         error_code,
                                  std::string&                                         error_message)
{
    return m_events_handler.onSetMonitoringLevel(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::SetNetworkProfileReq& request,
                                  ocpp::messages::ocpp20::SetNetworkProfileConf&      response,
                                  std::string&                                        error_code,
                                  std::string&                                        error_message)
{
    return m_events_handler.onSetNetworkProfile(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::SetVariableMonitoringReq& request,
                                  ocpp::messages::ocpp20::SetVariableMonitoringConf&      response,
                                  std::string&                                            error_code,
                                  std::string&                                            error_message)
{
    return m_events_handler.onSetVariableMonitoring(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::SetVariablesReq& request,
                                  ocpp::messages::ocpp20::SetVariablesConf&      response,
                                  std::string&                                   error_code,
                                  std::string&                                   error_message)
{
    return m_events_handler.onSetVariables(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::TriggerMessageReq& request,
                                  ocpp::messages::ocpp20::TriggerMessageConf&      response,
                                  std::string&                                     error_code,
                                  std::string&                                     error_message)
{
    return m_events_handler.onTriggerMessage(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::UnlockConnectorReq& request,
                                  ocpp::messages::ocpp20::UnlockConnectorConf&      response,
                                  std::string&                                      error_code,
                                  std::string&                                      error_message)
{
    return m_events_handler.onUnlockConnector(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::UnpublishFirmwareReq& request,
                                  ocpp::messages::ocpp20::UnpublishFirmwareConf&      response,
                                  std::string&                                        error_code,
                                  std::string&                                        error_message)
{
    return m_events_handler.onUnpublishFirmware(request, response, error_code, error_message);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
bool ChargePoint20::handleMessage(const ocpp::messages::ocpp20::UpdateFirmwareReq& request,
                                  ocpp::messages::ocpp20::UpdateFirmwareConf&      response,
                                  std::string&                                     error_code,
                                  std::string&                                     error_message)
{
    return m_events_handler.onUpdateFirmware(request, response, error_code, error_message);
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
