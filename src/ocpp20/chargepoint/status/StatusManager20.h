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

#ifndef OPENOCPP_OCPP20_STATUSMANAGER20_H
#define OPENOCPP_OCPP20_STATUSMANAGER20_H

#include "IStatusManager20.h"
#include "ITriggerMessageManager20.h"
#include "Timer.h"

namespace ocpp
{
// Forward declarations
namespace messages
{
class GenericMessageSender;
} // namespace messages
namespace config
{
class IChargePointConfig20;
class IInternalConfigManager;
} // namespace config
namespace helpers
{
class WorkerThreadPool;
} // namespace helpers

// Main namespace
namespace chargepoint
{
namespace ocpp20
{

class Connectors;
class IBasicChargePointEventsHandler;
class IDeviceModel;

/** @brief Handle charge point status (boot notification, status notification, heartbeat) */
class StatusManager : public IStatusManager, public ITriggerMessageManager::ITriggerMessageHandler
{
  public:
    /** @brief Constructor */
    StatusManager(const ocpp::config::IChargePointConfig20& stack_config,
                  IDeviceModel&                             device_model,
                  IBasicChargePointEventsHandler&           events_handler,
                  ocpp::config::IInternalConfigManager&     internal_config,
                  ocpp::helpers::ITimerPool&                timer_pool,
                  ocpp::helpers::WorkerThreadPool&          worker_pool,
                  Connectors&                               connectors,
                  ocpp::messages::GenericMessageSender&     msg_sender,
                  ITriggerMessageManager&                   trigger_manager,
                  ocpp::types::ocpp20::BootReasonEnumType   boot_reason);

    /** @brief Destructor */
    virtual ~StatusManager();

    // IStatusManager interface

    /** @copydoc ocpp::types::ocpp20::RegistrationStatusEnumType IStatusManager::getRegistrationStatus() */
    ocpp::types::ocpp20::RegistrationStatusEnumType getRegistrationStatus() override { return m_registration_status; }

    /** @copydoc void IStatusManager::forceRegistrationStatus(ocpp::types::ocpp20::RegistrationStatusEnumType) */
    void forceRegistrationStatus(ocpp::types::ocpp20::RegistrationStatusEnumType status) override;

    /** @copydoc void IStatusManager::updateConnectionStatus(bool) */
    void updateConnectionStatus(bool is_connected) override;

    /** @copydoc bool IStatusManager::updateConnectorStatus(unsigned int,
     *                                                      unsigned int,
     *                                                      ocpp::types::ocpp20::ConnectorStatusEnumType)
     */
    bool updateConnectorStatus(unsigned int                                 evse_id,
                               unsigned int                                 connector_id,
                               ocpp::types::ocpp20::ConnectorStatusEnumType status) override;

    /** @copydoc void IStatusManager::resetHeartBeatTimer() */
    void resetHeartBeatTimer() override;

    // ITriggerMessageManager::ITriggerMessageHandler interface

    /** @copydoc bool ITriggerMessageManager::ITriggerMessageHandler::onTriggerMessage(ocpp::types::ocpp20::MessageTriggerEnumType,
     *                                                                                 const ocpp::types::Optional<ocpp::types::ocpp20::EVSEType>&)
     */
    bool onTriggerMessage(ocpp::types::ocpp20::MessageTriggerEnumType                 message,
                          const ocpp::types::Optional<ocpp::types::ocpp20::EVSEType>& evse) override;

  private:
    /** @brief Stack configuration */
    const ocpp::config::IChargePointConfig20& m_stack_config;
    /** @brief Device model */
    IDeviceModel& m_device_model;
    /** @brief User defined events handler */
    IBasicChargePointEventsHandler& m_events_handler;
    /** @brief Charge point's internal configuration */
    ocpp::config::IInternalConfigManager& m_internal_config;
    /** @brief Worker thread pool */
    ocpp::helpers::WorkerThreadPool& m_worker_pool;
    /** @brief Charge point's connectors */
    Connectors& m_connectors;
    /** @brief Message sender */
    ocpp::messages::GenericMessageSender& m_msg_sender;

    /** @brief Boot reason */
    const ocpp::types::ocpp20::BootReasonEnumType m_boot_reason;
    /** @brief Registration status */
    ocpp::types::ocpp20::RegistrationStatusEnumType m_registration_status;
    /** @brief Indicate if the boot notification message must be inconditionnaly sent on connection */
    bool m_force_boot_notification;
    /** @brief Indicate if the boot notification message has been sent */
    bool m_boot_notification_sent;
    /** @brief Boot notification process timer */
    ocpp::helpers::Timer m_boot_notification_timer;
    /** @brief Heartbeat timer */
    ocpp::helpers::Timer m_heartbeat_timer;
    /** @brief Heartbeat interval */
    std::chrono::seconds m_heartbeat_interval;
    /** @brief Next mandatory heartbeat timestamp */
    std::chrono::steady_clock::time_point m_next_heartbeat_timestamp;
    /** @brief Last disconnection timestamp */
    std::chrono::steady_clock::time_point m_last_disconnect_timestamp;

    /** @brief Boot notification process */
    void bootNotificationProcess();
    /** @brief Heartbeat process */
    void heartBeatProcess();
    /** @brief Status notification process */
    void statusNotificationProcess(unsigned int evse_id, unsigned int connector_id);
    /** @brief Send the boot notification message */
    void sendBootNotification();
};

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_OCPP20_STATUSMANAGER20_H
