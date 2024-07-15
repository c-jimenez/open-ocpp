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

#ifndef OPENOCPP_TRANSACTIONMANAGER_H
#define OPENOCPP_TRANSACTIONMANAGER_H

#include "Enums.h"
#include "GenericMessageHandler.h"
#include "RemoteStartTransaction.h"
#include "RemoteStopTransaction.h"

namespace ocpp
{
// Forward declarations
namespace config
{
class IOcppConfig;
} // namespace config
namespace messages
{
class IMessageDispatcher;
class GenericMessagesConverter;
class GenericMessageSender;
class IRequestFifo;
} // namespace messages

// Main namespace
namespace chargepoint
{

class IAuthentManager;
class Connectors;
class ReservationManager;
class IChargePointEventsHandler;
class IMeterValuesManager;
class ISmartChargingManager;
class IStatusManager;

/** @brief Handle charge point transaction requests */
class TransactionManager : public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::RemoteStartTransactionReq,
                                                                        ocpp::messages::ocpp16::RemoteStartTransactionConf>,
                           public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::RemoteStopTransactionReq,
                                                                        ocpp::messages::ocpp16::RemoteStopTransactionConf>
{
  public:
    /** @brief Constructor */
    TransactionManager(ocpp::config::IOcppConfig&                      ocpp_config,
                       IChargePointEventsHandler&                      events_handler,
                       Connectors&                                     connectors,
                       const ocpp::messages::GenericMessagesConverter& messages_converter,
                       ocpp::messages::IMessageDispatcher&             msg_dispatcher,
                       ocpp::messages::GenericMessageSender&           msg_sender,
                       ocpp::messages::IRequestFifo&                   requests_fifo,
                       IStatusManager&                                 status_manager,
                       IAuthentManager&                                authent_manager,
                       ReservationManager&                             reservation_manager,
                       IMeterValuesManager&                            meter_values_manager,
                       ISmartChargingManager&                          smart_charging_manager);

    /** @brief Destructor */
    virtual ~TransactionManager();

    /**
     * @brief Start a transaction
     * @param connector_id Id of the connector
     * @param id_tag Id of the user
     * @param transaction_id Id of the transaction
     * @return Authorization status (see AuthorizationStatus enum)
     */
    ocpp::types::ocpp16::AuthorizationStatus startTransaction(unsigned int connector_id, const std::string& id_tag, int& transaction_id);

    /**
     * @brief Stop a transaction
     * @param connector_id Id of the connector
     * @param id_tag Id of the user (leave empty if no id tag)
     * @param reason Stop reason
     * @return true if a corresponding transaction exist and has been stopped, false otherwise
     */
    bool stopTransaction(unsigned int connector_id, const std::string& id_tag, ocpp::types::ocpp16::Reason reason);

    // GenericMessageHandler interface

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::RemoteStartTransactionReq& request,
                       ocpp::messages::ocpp16::RemoteStartTransactionConf&      response,
                       std::string&                                             error_code,
                       std::string&                                             error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::RemoteStopTransactionReq& request,
                       ocpp::messages::ocpp16::RemoteStopTransactionConf&      response,
                       std::string&                                            error_code,
                       std::string&                                            error_message) override;

  private:
    /** @brief Standard OCPP configuration */
    ocpp::config::IOcppConfig& m_ocpp_config;
    /** @brief User defined events handler */
    IChargePointEventsHandler& m_events_handler;
    /** @brief Charge point's connectors */
    Connectors& m_connectors;
    /** @brief Message sender */
    ocpp::messages::GenericMessageSender& m_msg_sender;
    /** @brief Status manager */
    IStatusManager& m_status_manager;
    /** @brief Authentication manager */
    IAuthentManager& m_authent_manager;
    /** @brief Reservation manager */
    ReservationManager& m_reservation_manager;
    /** @brief Meter values manager */
    IMeterValuesManager& m_meter_values_manager;
    /** @brief Smart charging manager */
    ISmartChargingManager& m_smart_charging_manager;

    /** @brief Transaction related requests FIFO */
    ocpp::messages::IRequestFifo& m_requests_fifo;
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_TRANSACTIONMANAGER_H
