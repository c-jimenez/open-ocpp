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

#ifndef OPENOCPP_STATUSMANAGER_H
#define OPENOCPP_STATUSMANAGER_H

#include "ChangeAvailability.h"
#include "GenericMessageHandler.h"
#include "IStatusManager.h"
#include "ITriggerMessageManager.h"
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
class IChargePointConfig;
class IOcppConfig;
class IInternalConfigManager;
} // namespace config
namespace helpers
{
class WorkerThreadPool;
} // namespace helpers

// Main namespace
namespace chargepoint
{

class Connectors;
class IChargePointEventsHandler;

/** @brief Handle charge point status (boot notification, status notification, heartbeat) */
class StatusManager : public IStatusManager,
                      public ITriggerMessageManager::ITriggerMessageHandler,
                      public ITriggerMessageManager::IExtendedTriggerMessageHandler,
                      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::ChangeAvailabilityReq,
                                                                   ocpp::messages::ocpp16::ChangeAvailabilityConf>
{
  public:
    /** @brief Constructor */
    StatusManager(const ocpp::config::IChargePointConfig&         stack_config,
                  ocpp::config::IOcppConfig&                      ocpp_config,
                  IChargePointEventsHandler&                      events_handler,
                  ocpp::config::IInternalConfigManager&           internal_config,
                  ocpp::helpers::ITimerPool&                      timer_pool,
                  ocpp::helpers::WorkerThreadPool&                worker_pool,
                  Connectors&                                     connectors,
                  ocpp::messages::IMessageDispatcher&             msg_dispatcher,
                  ocpp::messages::GenericMessageSender&           msg_sender,
                  const ocpp::messages::GenericMessagesConverter& messages_converter,
                  ITriggerMessageManager&                         trigger_manager);

    /** @brief Destructor */
    virtual ~StatusManager();

    // IStatusManager interface

    /** @copydoc ocpp::types::ocpp16::RegistrationStatus IStatusManager::getRegistrationStatus() */
    ocpp::types::ocpp16::RegistrationStatus getRegistrationStatus() override { return m_registration_status; }

    /** @copydoc void IStatusManager::forceRegistrationStatus(ocpp::types::ocpp16::RegistrationStatus) */
    void forceRegistrationStatus(ocpp::types::ocpp16::RegistrationStatus status) override;

    /** @copydoc void IStatusManager::updateConnectionStatus(bool) */
    void updateConnectionStatus(bool is_connected) override;

    /** @copydoc bool IStatusManager::updateConnectorStatus(unsigned int,
     *                                                      ocpp::types::ocpp16::ChargePointStatus,
     *                                                      ocpp::types::ocpp16::ChargePointErrorCode,
     *                                                      const std::string&,
     *                                                      const std::string&,
     *                                                      const std::string&)
    */
    bool updateConnectorStatus(unsigned int                              connector_id,
                               ocpp::types::ocpp16::ChargePointStatus    status,
                               ocpp::types::ocpp16::ChargePointErrorCode error_code   = ocpp::types::ocpp16::ChargePointErrorCode::NoError,
                               const std::string&                        info         = "",
                               const std::string&                        vendor_id    = "",
                               const std::string&                        vendor_error = "") override;

    /** @copydoc void IStatusManager::resetHeartBeatTimer() */
    void resetHeartBeatTimer() override;

    // ITriggerMessageHandler interfaces

    /** @copydoc bool ITriggerMessageHandler::onTriggerMessage(ocpp::types::ocpp16::MessageTrigger, const ocpp::types::Optional<unsigned int>&) */
    bool onTriggerMessage(ocpp::types::ocpp16::MessageTrigger message, const ocpp::types::Optional<unsigned int>& connector_id) override;

    /** @copydoc bool ITriggerMessageHandler::onTriggerMessage(ocpp::types::ocpp16::MessageTriggerEnumType, const ocpp::types::Optional<unsigned int>&) */
    bool onTriggerMessage(ocpp::types::ocpp16::MessageTriggerEnumType message,
                          const ocpp::types::Optional<unsigned int>&  connector_id) override;

    // GenericMessageHandler interface

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::ChangeAvailabilityReq& request,
                       ocpp::messages::ocpp16::ChangeAvailabilityConf&      response,
                       std::string&                                         error_code,
                       std::string&                                         error_message) override;

  private:
    /** @brief Stack configuration */
    const ocpp::config::IChargePointConfig& m_stack_config;
    /** @brief Standard OCPP configuration */
    ocpp::config::IOcppConfig& m_ocpp_config;
    /** @brief User defined events handler */
    IChargePointEventsHandler& m_events_handler;
    /** @brief Charge point's internal configuration */
    ocpp::config::IInternalConfigManager& m_internal_config;
    /** @brief Worker thread pool */
    ocpp::helpers::WorkerThreadPool& m_worker_pool;
    /** @brief Charge point's connectors */
    Connectors& m_connectors;
    /** @brief Message sender */
    ocpp::messages::GenericMessageSender& m_msg_sender;

    /** @brief Registration status */
    ocpp::types::ocpp16::RegistrationStatus m_registration_status;
    /** @brief Indicate if the boot notification message must be inconditionnaly sent on connection */
    bool m_force_boot_notification;
    /** @brief Indicate if the boot notification message has been sent */
    bool m_boot_notification_sent;
    /** @brief Boot notification process timer */
    ocpp::helpers::Timer m_boot_notification_timer;
    /** @brief Heartbeat timer */
    ocpp::helpers::Timer m_heartbeat_timer;

    /** @brief Boot notification process */
    void bootNotificationProcess();
    /** @brief Heartbeat process */
    void heartBeatProcess();
    /** @brief Status notification process */
    void statusNotificationProcess(unsigned int connector_id);
    /** @brief Send the boot notification message */
    void sendBootNotification();
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_STATUSMANAGER_H
