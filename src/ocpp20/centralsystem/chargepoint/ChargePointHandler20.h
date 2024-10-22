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

#ifndef OPENOCPP_OCPP20_CS_CHARGEPOINTHANDLER20_H
#define OPENOCPP_OCPP20_CS_CHARGEPOINTHANDLER20_H

#include "Authorize20.h"
#include "BootNotification20.h"
#include "ClearedChargingLimit20.h"
#include "CostUpdated20.h"
#include "DataTransfer20.h"
#include "FirmwareStatusNotification20.h"
#include "GenericMessageHandler.h"
#include "Heartbeat20.h"
#include "LogStatusNotification20.h"
#include "Logger.h"
#include "MessagesConverter20.h"
#include "MeterValues20.h"
#include "NotifyChargingLimit20.h"
#include "NotifyCustomerInformation20.h"
#include "NotifyDisplayMessages20.h"
#include "NotifyEVChargingNeeds20.h"
#include "NotifyEVChargingSchedule20.h"
#include "NotifyEvent20.h"
#include "NotifyMonitoringReport20.h"
#include "NotifyReport20.h"
#include "PublishFirmwareStatusNotification20.h"
#include "ReportChargingProfiles20.h"
#include "ReservationStatusUpdate20.h"
#include "SecurityEventNotification20.h"
#include "SignCertificate20.h"
#include "StatusNotification20.h"
#include "TransactionEvent20.h"

namespace ocpp
{
namespace config
{
class ICentralSystemConfig20;
} // namespace config
namespace messages
{
class MessageDispatcher;

namespace ocpp20
{
class MessagesConverter20;
} // namespace ocpp20
} // namespace messages

namespace centralsystem
{
namespace ocpp20
{

class IChargePointRequestHandler20;

/** @brief Handler for charge point requests */
class ChargePointHandler20
    : public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::BootNotification20Req,
                                                   ocpp::messages::ocpp20::BootNotification20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::Authorize20Req, ocpp::messages::ocpp20::Authorize20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::ClearedChargingLimit20Req,
                                                   ocpp::messages::ocpp20::ClearedChargingLimit20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::CostUpdated20Req, ocpp::messages::ocpp20::CostUpdated20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::DataTransfer20Req, ocpp::messages::ocpp20::DataTransfer20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::FirmwareStatusNotification20Req,
                                                   ocpp::messages::ocpp20::FirmwareStatusNotification20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::Heartbeat20Req, ocpp::messages::ocpp20::Heartbeat20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::LogStatusNotification20Req,
                                                   ocpp::messages::ocpp20::LogStatusNotification20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::MeterValues20Req, ocpp::messages::ocpp20::MeterValues20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::NotifyChargingLimit20Req,
                                                   ocpp::messages::ocpp20::NotifyChargingLimit20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::NotifyCustomerInformation20Req,
                                                   ocpp::messages::ocpp20::NotifyCustomerInformation20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::NotifyDisplayMessages20Req,
                                                   ocpp::messages::ocpp20::NotifyDisplayMessages20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::NotifyEVChargingNeeds20Req,
                                                   ocpp::messages::ocpp20::NotifyEVChargingNeeds20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::NotifyEVChargingSchedule20Req,
                                                   ocpp::messages::ocpp20::NotifyEVChargingSchedule20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::NotifyEvent20Req, ocpp::messages::ocpp20::NotifyEvent20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::NotifyMonitoringReport20Req,
                                                   ocpp::messages::ocpp20::NotifyMonitoringReport20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::NotifyReport20Req, ocpp::messages::ocpp20::NotifyReport20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::PublishFirmwareStatusNotification20Req,
                                                   ocpp::messages::ocpp20::PublishFirmwareStatusNotification20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::ReportChargingProfiles20Req,
                                                   ocpp::messages::ocpp20::ReportChargingProfiles20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::ReservationStatusUpdate20Req,
                                                   ocpp::messages::ocpp20::ReservationStatusUpdate20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::SecurityEventNotification20Req,
                                                   ocpp::messages::ocpp20::SecurityEventNotification20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::SignCertificate20Req,
                                                   ocpp::messages::ocpp20::SignCertificate20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::StatusNotification20Req,
                                                   ocpp::messages::ocpp20::StatusNotification20Conf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::TransactionEvent20Req,
                                                   ocpp::messages::ocpp20::TransactionEvent20Conf>
{
  public:
    /**
     * @brief Constructor
     * @param identifier Charge point's identifier
     * @param messages_converter Converter from/to OCPP to/from JSON messages
     * @param msg_dispatcher Message dispatcher
     * @param stack_config Stack configuration
     */
    ChargePointHandler20(const std::string&                                 identifier,
                         const ocpp::messages::ocpp20::MessagesConverter20& messages_converter,
                         ocpp::messages::MessageDispatcher&                 msg_dispatcher,
                         const ocpp::config::ICentralSystemConfig20&        stack_config);
    /** @brief Destructor */
    virtual ~ChargePointHandler20();

    /** @brief Register the event handler */
    void registerHandler(IChargePointRequestHandler20& handler) { m_handler = &handler; }

    // OCPP handlers
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::BootNotification20Req& request,
                       ocpp::messages::ocpp20::BootNotification20Conf&      response,
                       std::string&                                         error_code,
                       std::string&                                         error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::Authorize20Req& request,
                       ocpp::messages::ocpp20::Authorize20Conf&      response,
                       std::string&                                  error_code,
                       std::string&                                  error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::ClearedChargingLimit20Req& request,
                       ocpp::messages::ocpp20::ClearedChargingLimit20Conf&      response,
                       std::string&                                             error_code,
                       std::string&                                             error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::CostUpdated20Req& request,
                       ocpp::messages::ocpp20::CostUpdated20Conf&      response,
                       std::string&                                    error_code,
                       std::string&                                    error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::DataTransfer20Req& request,
                       ocpp::messages::ocpp20::DataTransfer20Conf&      response,
                       std::string&                                     error_code,
                       std::string&                                     error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::FirmwareStatusNotification20Req& request,
                       ocpp::messages::ocpp20::FirmwareStatusNotification20Conf&      response,
                       std::string&                                                   error_code,
                       std::string&                                                   error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::Heartbeat20Req& request,
                       ocpp::messages::ocpp20::Heartbeat20Conf&      response,
                       std::string&                                  error_code,
                       std::string&                                  error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::LogStatusNotification20Req& request,
                       ocpp::messages::ocpp20::LogStatusNotification20Conf&      response,
                       std::string&                                              error_code,
                       std::string&                                              error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::MeterValues20Req& request,
                       ocpp::messages::ocpp20::MeterValues20Conf&      response,
                       std::string&                                    error_code,
                       std::string&                                    error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::NotifyChargingLimit20Req& request,
                       ocpp::messages::ocpp20::NotifyChargingLimit20Conf&      response,
                       std::string&                                            error_code,
                       std::string&                                            error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::NotifyCustomerInformation20Req& request,
                       ocpp::messages::ocpp20::NotifyCustomerInformation20Conf&      response,
                       std::string&                                                  error_code,
                       std::string&                                                  error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::NotifyDisplayMessages20Req& request,
                       ocpp::messages::ocpp20::NotifyDisplayMessages20Conf&      response,
                       std::string&                                              error_code,
                       std::string&                                              error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::NotifyEVChargingNeeds20Req& request,
                       ocpp::messages::ocpp20::NotifyEVChargingNeeds20Conf&      response,
                       std::string&                                              error_code,
                       std::string&                                              error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::NotifyEVChargingSchedule20Req& request,
                       ocpp::messages::ocpp20::NotifyEVChargingSchedule20Conf&      response,
                       std::string&                                                 error_code,
                       std::string&                                                 error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::NotifyEvent20Req& request,
                       ocpp::messages::ocpp20::NotifyEvent20Conf&      response,
                       std::string&                                    error_code,
                       std::string&                                    error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::NotifyMonitoringReport20Req& request,
                       ocpp::messages::ocpp20::NotifyMonitoringReport20Conf&      response,
                       std::string&                                               error_code,
                       std::string&                                               error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::NotifyReport20Req& request,
                       ocpp::messages::ocpp20::NotifyReport20Conf&      response,
                       std::string&                                     error_code,
                       std::string&                                     error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::PublishFirmwareStatusNotification20Req& request,
                       ocpp::messages::ocpp20::PublishFirmwareStatusNotification20Conf&      response,
                       std::string&                                                          error_code,
                       std::string&                                                          error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::ReportChargingProfiles20Req& request,
                       ocpp::messages::ocpp20::ReportChargingProfiles20Conf&      response,
                       std::string&                                               error_code,
                       std::string&                                               error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::ReservationStatusUpdate20Req& request,
                       ocpp::messages::ocpp20::ReservationStatusUpdate20Conf&      response,
                       std::string&                                                error_code,
                       std::string&                                                error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::SecurityEventNotification20Req& request,
                       ocpp::messages::ocpp20::SecurityEventNotification20Conf&      response,
                       std::string&                                                  error_code,
                       std::string&                                                  error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::SignCertificate20Req& request,
                       ocpp::messages::ocpp20::SignCertificate20Conf&      response,
                       std::string&                                        error_code,
                       std::string&                                        error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::StatusNotification20Req& request,
                       ocpp::messages::ocpp20::StatusNotification20Conf&      response,
                       std::string&                                           error_code,
                       std::string&                                           error_message) override;
    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::TransactionEvent20Req& request,
                       ocpp::messages::ocpp20::TransactionEvent20Conf&      response,
                       std::string&                                         error_code,
                       std::string&                                         error_message) override;

  private:
    /** @brief Charge point's identifier */
    const std::string m_identifier;
    /** @brief Stack configuration */
    const ocpp::config::ICentralSystemConfig20& m_stack_config;
    /** @brief Messages converters */
    const ocpp::messages::GenericMessagesConverter& m_messages_converter;
    /** @brief Request handler */
    IChargePointRequestHandler20* m_handler;
};

} // namespace ocpp20
} // namespace centralsystem
} // namespace ocpp

#endif // OPENOCPP_OCPP20_CS_CHARGEPOINTHANDLER20_H