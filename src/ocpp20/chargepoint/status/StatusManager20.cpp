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

#include "StatusManager20.h"
#include "BootNotification20.h"
#include "Connectors20.h"
#include "GenericMessageSender.h"
#include "Heartbeat20.h"
#include "IBasicChargePointEventsHandler20.h"
#include "IChargePointConfig20.h"
#include "IDeviceModel20.h"
#include "IInternalConfigManager.h"
#include "InternalConfigKeys.h"
#include "Logger.h"
#include "StatusNotification20.h"
#include "WorkerThreadPool.h"

#include <functional>
#include <thread>

using namespace ocpp::messages;
using namespace ocpp::messages::ocpp20;
using namespace ocpp::types;
using namespace ocpp::types::ocpp20;

namespace ocpp
{
namespace chargepoint
{
namespace ocpp20
{

/** @brief Mandatory heartbeat period */
static const std::chrono::hours MANDATORY_HEARTBEAT_PERIOD(24);

/** @brief Constructor */
StatusManager::StatusManager(const ocpp::config::IChargePointConfig20& stack_config,
                             IDeviceModel&                             device_model,
                             IBasicChargePointEventsHandler&           events_handler,
                             ocpp::config::IInternalConfigManager&     internal_config,
                             ocpp::helpers::ITimerPool&                timer_pool,
                             ocpp::helpers::WorkerThreadPool&          worker_pool,
                             Connectors&                               connectors,
                             ocpp::messages::GenericMessageSender&     msg_sender,
                             ITriggerMessageManager&                   trigger_manager,
                             ocpp::types::ocpp20::BootReasonEnumType   boot_reason)
    : m_stack_config(stack_config),
      m_device_model(device_model),
      m_events_handler(events_handler),
      m_internal_config(internal_config),
      m_worker_pool(worker_pool),
      m_connectors(connectors),
      m_msg_sender(msg_sender),
      m_boot_reason(boot_reason),
      m_registration_status(RegistrationStatusEnumType::Rejected),
      m_force_boot_notification(false),
      m_boot_notification_sent(false),
      m_boot_notification_timer(timer_pool, "Boot notification"),
      m_heartbeat_timer(timer_pool, "Heartbeat"),
      m_heartbeat_interval(0),
      m_next_heartbeat_timestamp(std::chrono::steady_clock::now() + MANDATORY_HEARTBEAT_PERIOD),
      m_last_disconnect_timestamp()
{
    trigger_manager.registerHandler(MessageTriggerEnumType::BootNotification, *this);
    trigger_manager.registerHandler(MessageTriggerEnumType::StatusNotification, *this);
    trigger_manager.registerHandler(MessageTriggerEnumType::Heartbeat, *this);

    m_boot_notification_timer.setCallback([this] { m_worker_pool.run<void>(std::bind(&StatusManager::bootNotificationProcess, this)); });
    m_heartbeat_timer.setCallback([this] { m_worker_pool.run<void>(std::bind(&StatusManager::heartBeatProcess, this)); });

    // Look for HeartBeatInterval variable in the device model
    GetVariableDataType var_req;
    var_req.component.name.assign("OCPPCommCtrlr");
    var_req.variable.name.assign("HeartbeatInterval");
    GetVariableResultType var_res = m_device_model.getVariable(var_req);
    if (var_res.attributeStatus == GetVariableStatusEnumType::Accepted)
    {
        m_heartbeat_interval = std::chrono::seconds(std::atoi(var_res.attributeValue.value().c_str()));
    }
    else
    {
        LOG_WARNING << "OCPPCommCtrlr.HeartbeatInterval not present in the device model, using default value = 1h";
        m_heartbeat_interval = std::chrono::hours(1);
    }
}

/** @brief Destructor */
StatusManager::~StatusManager() { }

/** @copydoc void IStatusManager::forceRegistrationStatus(ocpp::types::ocpp20::RegistrationStatusEnumType) */
void StatusManager::forceRegistrationStatus(ocpp::types::ocpp20::RegistrationStatusEnumType status)
{
    if (status != m_registration_status)
    {
        m_force_boot_notification = true;
        m_boot_notification_sent  = false;
    }
    m_registration_status = status;
}

/** @copydoc void IStatusManager::updateConnectionStatus(bool) */
void StatusManager::updateConnectionStatus(bool is_connected)
{
    if (is_connected)
    {
        // If not accepted by the central system, restart boot notification process
        if (m_force_boot_notification || (m_registration_status != RegistrationStatusEnumType::Accepted))
        {
            m_boot_notification_timer.start(std::chrono::milliseconds(1u), true);
        }
        else
        {
            // Look for OfflineThreshold variable in the device model
            std::chrono::seconds offline_threshold(0);
            GetVariableDataType  var_req;
            var_req.component.name.assign("OCPPCommCtrlr");
            var_req.variable.name.assign("OfflineThreshold");
            GetVariableResultType var_res = m_device_model.getVariable(var_req);
            if (var_res.attributeStatus == GetVariableStatusEnumType::Accepted)
            {
                offline_threshold = std::chrono::seconds(std::atoi(var_res.attributeValue.value().c_str()));
            }
            else
            {
                LOG_WARNING << "OCPPCommCtrlr.OfflineThreshold not present in the device model";
            }

            bool notify_all_connectors;
            if ((std::chrono::steady_clock::now() - m_last_disconnect_timestamp) >= offline_threshold)
            {
                // Notify status of all the connectors
                notify_all_connectors = true;
            }
            else
            {
                // Only if the status of a connector has changed since the last notification
                // to the central system, send the new connector status
                notify_all_connectors = false;
            }

            // Notify connectors status
            for (const Evse* evse : m_connectors.getEvses())
            {
                for (const Connector* connector : evse->connectors)
                {
                    if (notify_all_connectors || (connector->status != connector->last_notified_status))
                    {
                        statusNotificationProcess(evse->id, connector->id);
                    }
                }
            }

            // Restart heartbeat process
            m_heartbeat_timer.start(m_heartbeat_interval);
        }
    }
    else
    {
        // Stop boot notification and heartbeat processes
        m_boot_notification_timer.stop();
        m_heartbeat_timer.stop();
        m_last_disconnect_timestamp = std::chrono::steady_clock::now();
    }
}

/** @copydoc bool IStatusManager::updateConnectorStatus(unsigned int,
 *                                                      unsigned int,
 *                                                      ocpp::types::ocpp20::ConnectorStatusEnumType)
 */
bool StatusManager::updateConnectorStatus(unsigned int                                 evse_id,
                                          unsigned int                                 connector_id,
                                          ocpp::types::ocpp20::ConnectorStatusEnumType status)
{
    bool ret = false;

    // Get selected connector
    Connector* connector = m_connectors.getConnector(evse_id, connector_id);
    if (connector)
    {
        std::lock_guard<std::mutex> lock(connector->mutex);

        // Check if status has changed
        if (connector->status != status)
        {
            std::string status_str = ConnectorStatusEnumTypeHelper.toString(status);
            LOG_INFO << "EVSE " << evse_id << " - Connector " << connector_id << " : " << ConnectorStatusEnumTypeHelper.toString(status);

            // Save new status
            connector->status           = status;
            connector->status_timestamp = DateTime::now();
            m_connectors.saveConnector(evse_id, connector->id);

            // Update device model
            SetVariableDataType var_req;
            var_req.component.name.assign("Connector");
            var_req.component.evse.value().id          = evse_id;
            var_req.component.evse.value().connectorId = connector_id;
            var_req.variable.name.assign("AvailabilityState");
            var_req.attributeValue.assign(status_str);
            SetVariableResultType var_res = m_device_model.updateVariable(var_req);
            if (var_res.attributeStatus != SetVariableStatusEnumType::Accepted)
            {
                LOG_WARNING << "Connector.AvailabilityState couldn't be updated in the device model, EVSE = " << evse_id
                            << " - Connector = " << connector_id;
            }

            // Compute EVSE status
            ConnectorStatusEnumType evse_status = ConnectorStatusEnumType::Available;
            Evse*                   evse        = m_connectors.getEvse(evse_id);
            if (evse)
            {
                for (const Connector* connector : evse->connectors)
                {
                    if (static_cast<int>(connector->status) >= static_cast<int>(evse_status))
                    {
                        evse_status = connector->status;
                    }
                }
                if (evse_status != evse->status)
                {
                    // Save new status
                    evse->status           = evse_status;
                    evse->status_timestamp = DateTime::now();
                    m_connectors.saveEvse(evse_id);

                    // Update device model
                    var_req.component.name.assign("EVSE");
                    var_req.component.evse.value().connectorId.clear();
                    var_req.attributeValue.assign(status_str);
                    var_res = m_device_model.updateVariable(var_req);
                    if (var_res.attributeStatus != SetVariableStatusEnumType::Accepted)
                    {
                        LOG_WARNING << "EVSE.AvailabilityState couldn't be updated in the device model, EVSE = " << evse_id;
                    }
                }
            }

            // Check registration status
            if (m_registration_status == RegistrationStatusEnumType::Accepted)
            {
                // Notify now
                m_worker_pool.run<void>(std::bind(&StatusManager::statusNotificationProcess, this, evse_id, connector_id));
            }
        }
        ret = true;
    }

    return ret;
}

/** @copydoc void IStatusManager::resetHeartBeatTimer() */
void StatusManager::resetHeartBeatTimer()
{
    if (m_heartbeat_timer.isStarted() && !m_heartbeat_timer.isSingleShot())
    {
        auto now = std::chrono::steady_clock::now();
        if ((now + m_heartbeat_interval) >= m_next_heartbeat_timestamp)
        {
            auto interval = m_next_heartbeat_timestamp - now;
            m_heartbeat_timer.restart(std::chrono::duration_cast<std::chrono::milliseconds>(interval), true);
        }
        else
        {
            m_heartbeat_timer.restart(m_heartbeat_interval);
        }
    }
}

/** @copydoc bool ITriggerMessageManager::ITriggerMessageHandler::onTriggerMessage(ocpp::types::ocpp20::MessageTriggerEnumType,
 *                                                                                 const ocpp::types::Optional<ocpp::types::ocpp20::EVSEType>&)
 */
bool StatusManager::onTriggerMessage(ocpp::types::ocpp20::MessageTriggerEnumType                 message,
                                     const ocpp::types::Optional<ocpp::types::ocpp20::EVSEType>& evse)
{
    bool ret = true;

    switch (message)
    {
        case MessageTriggerEnumType::BootNotification:
        {
            m_worker_pool.run<void>(
                [this]
                {
                    // To let some time for the trigger message reply
                    std::this_thread::sleep_for(std::chrono::milliseconds(250u));
                    sendBootNotification();
                });
        }
        break;

        case MessageTriggerEnumType::Heartbeat:
        {
            m_worker_pool.run<void>(
                [this]
                {
                    // To let some time for the trigger message reply
                    std::this_thread::sleep_for(std::chrono::milliseconds(250u));
                    heartBeatProcess();
                });
        }
        break;

        case MessageTriggerEnumType::StatusNotification:
        {
            if (evse.isSet())
            {
                if (evse.value().connectorId.isSet())
                {
                    m_worker_pool.run<void>(
                        [this, evse_id = evse.value().id, connector_id = evse.value().connectorId.value()]
                        {
                            // To let some time for the trigger message reply
                            std::this_thread::sleep_for(std::chrono::milliseconds(250u));
                            statusNotificationProcess(evse_id, connector_id);
                        });
                }
                else
                {
                    const Evse* req_evse = m_connectors.getEvse(evse.value().id);
                    if (req_evse)
                    {
                        for (const Connector* connector : req_evse->connectors)
                        {
                            m_worker_pool.run<void>(
                                [this, evse_id = req_evse->id, connector_id = connector->id]
                                {
                                    // To let some time for the trigger message reply
                                    std::this_thread::sleep_for(std::chrono::milliseconds(250u));
                                    statusNotificationProcess(evse_id, connector_id);
                                });
                        }
                    }
                }
            }
            else
            {
                for (const Evse* evse : m_connectors.getEvses())
                {
                    for (const Connector* connector : evse->connectors)
                    {
                        m_worker_pool.run<void>(
                            [this, evse_id = evse->id, connector_id = connector->id]
                            {
                                // To let some time for the trigger message reply
                                std::this_thread::sleep_for(std::chrono::milliseconds(250u));
                                statusNotificationProcess(evse_id, connector_id);
                            });
                    }
                }
            }
        }
        break;

        default:
            ret = false;
            break;
    }

    return ret;
}

/** @brief Boot notification process thread */
void StatusManager::bootNotificationProcess()
{
    if (m_boot_notification_sent == false)
    {
        // Fill boot notification request
        BootNotificationReq boot_req;
        boot_req.reason = m_boot_reason;
        boot_req.chargingStation.model.assign(m_stack_config.chargePointModel());
        boot_req.chargingStation.vendorName.assign(m_stack_config.chargePointVendor());
        if (!m_stack_config.chargePointSerialNumber().empty())
        {
            boot_req.chargingStation.serialNumber.value().assign(m_stack_config.chargePointSerialNumber());
        }
        if (!m_stack_config.firmwareVersion().empty())
        {
            boot_req.chargingStation.firmwareVersion.value().assign(m_stack_config.firmwareVersion());
        }
        if (!m_stack_config.imsi().empty())
        {
            boot_req.chargingStation.modem.value().imsi.value().assign(m_stack_config.imsi());
        }
        if (!m_stack_config.iccid().empty())
        {
            boot_req.chargingStation.modem.value().iccid.value().assign(m_stack_config.iccid());
        }

        m_registration_status = RegistrationStatusEnumType::Rejected;

        // Send BootNotificationRequest
        BootNotificationConf boot_conf;
        CallResult           result = m_msg_sender.call(BOOTNOTIFICATION_ACTION, boot_req, boot_conf);
        if (result == CallResult::Ok)
        {
            if (boot_conf.status == RegistrationStatusEnumType::Accepted)
            {
                m_boot_notification_sent = true;

                // Send first status notifications
                for (const Evse* evse : m_connectors.getEvses())
                {
                    for (const Connector* connector : evse->connectors)
                    {
                        statusNotificationProcess(evse->id, connector->id);
                    }
                }

                // Configure hearbeat
                std::chrono::seconds interval(boot_conf.interval);
                m_heartbeat_interval       = std::chrono::seconds(boot_conf.interval);
                m_next_heartbeat_timestamp = std::chrono::steady_clock::now() + MANDATORY_HEARTBEAT_PERIOD;
                m_heartbeat_timer.start(m_heartbeat_interval);

                // Update value in device model
                SetVariableDataType var_req;
                var_req.component.name.assign("OCPPCommCtrlr");
                var_req.variable.name.assign("HeartbeatInterval");
                var_req.attributeValue.assign(std::to_string(m_heartbeat_interval.count()));
                SetVariableResultType var_res = m_device_model.setVariable(var_req);
                if (var_res.attributeStatus != SetVariableStatusEnumType::Accepted)
                {
                    LOG_WARNING << "OCPPCommCtrlr.HeartbeatInterval couldn't be updated in the device model";
                }
            }
            else
            {
                // Schedule next retry
                m_boot_notification_timer.start(std::chrono::seconds(boot_conf.interval), true);
            }

            m_registration_status           = boot_conf.status;
            std::string registration_status = RegistrationStatusEnumTypeHelper.toString(m_registration_status);
            LOG_INFO << "Registration status : " << registration_status;

            // Save registration status
            m_force_boot_notification = false;
            m_internal_config.setKey(LAST_REGISTRATION_STATUS_KEY, registration_status);

            // Notify boot
            m_events_handler.bootNotification(m_registration_status, boot_conf.currentTime);
        }
        else
        {
            // Schedule next retry
            m_boot_notification_timer.start(m_stack_config.retryInterval(), true);
        }
    }
    else
    {
        // If the status of a connector has changed since the last notification
        // to the central system, send the new connector status
        for (const Evse* evse : m_connectors.getEvses())
        {
            for (const Connector* connector : evse->connectors)
            {
                if (connector->status != connector->last_notified_status)
                {
                    statusNotificationProcess(evse->id, connector->id);
                }
            }
        }

        // Configure hearbeat
        m_heartbeat_timer.start(m_heartbeat_interval);
    }
}

/** @brief Heartbeat process */
void StatusManager::heartBeatProcess()
{
    HeartbeatReq  heartbeat_req;
    HeartbeatConf heartbeat_conf;
    CallResult    result = m_msg_sender.call(HEARTBEAT_ACTION, heartbeat_req, heartbeat_conf);
    if (result == CallResult::Ok)
    {
        LOG_INFO << "Heartbeat : " << heartbeat_conf.currentTime.str();

        m_next_heartbeat_timestamp = std::chrono::steady_clock::now() + MANDATORY_HEARTBEAT_PERIOD;
        m_events_handler.datetimeReceived(heartbeat_conf.currentTime);
        if (m_heartbeat_timer.isSingleShot())
        {
            m_heartbeat_timer.restart(m_heartbeat_interval);
        }
    }
}

/** @brief Status notification process */
void StatusManager::statusNotificationProcess(unsigned int evse_id, unsigned int connector_id)
{
    // Get connector
    Connector* connector = m_connectors.getConnector(evse_id, connector_id);
    if (connector)
    {
        // Send request
        StatusNotificationReq status_req;
        status_req.evseId          = evse_id;
        status_req.connectorId     = connector->id;
        status_req.connectorStatus = connector->status;
        status_req.timestamp       = connector->status_timestamp;

        StatusNotificationConf status_conf;
        CallResult             result = m_msg_sender.call(STATUSNOTIFICATION_ACTION, status_req, status_conf);
        if (result == CallResult::Ok)
        {
            // Update last notified status
            connector->last_notified_status = connector->status;
        }
    }
}

/** @brief Send the boot notification message */
void StatusManager::sendBootNotification()
{
    // Fill boot notification request
    BootNotificationReq boot_req;
    boot_req.reason = BootReasonEnumType::Triggered;
    boot_req.chargingStation.model.assign(m_stack_config.chargePointModel());
    boot_req.chargingStation.vendorName.assign(m_stack_config.chargePointVendor());
    if (!m_stack_config.chargePointSerialNumber().empty())
    {
        boot_req.chargingStation.serialNumber.value().assign(m_stack_config.chargePointSerialNumber());
    }
    if (!m_stack_config.firmwareVersion().empty())
    {
        boot_req.chargingStation.firmwareVersion.value().assign(m_stack_config.firmwareVersion());
    }
    if (!m_stack_config.imsi().empty())
    {
        boot_req.chargingStation.modem.value().imsi.value().assign(m_stack_config.imsi());
    }
    if (!m_stack_config.iccid().empty())
    {
        boot_req.chargingStation.modem.value().iccid.value().assign(m_stack_config.iccid());
    }

    // Send BootNotificationRequest
    BootNotificationConf boot_conf;
    CallResult           result = m_msg_sender.call(BOOTNOTIFICATION_ACTION, boot_req, boot_conf);
    if (result == CallResult::Ok)
    {
        // Save registration status
        m_registration_status = boot_conf.status;

        // Restart hearbeat timer
        std::chrono::seconds interval(boot_conf.interval);
        m_heartbeat_interval = std::chrono::seconds(boot_conf.interval);
        m_heartbeat_timer.restart(m_heartbeat_interval);

        // Save registration status
        m_force_boot_notification = false;
        m_internal_config.setKey(LAST_REGISTRATION_STATUS_KEY, RegistrationStatusEnumTypeHelper.toString(m_registration_status));
        if (m_registration_status == RegistrationStatusEnumType::Accepted)
        {
            // Cancel next retry
            m_boot_notification_timer.stop();
        }
    }

    return;
}

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp
