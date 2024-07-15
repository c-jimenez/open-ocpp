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

#ifndef OPENOCPP_RESERVATIONMANAGER_H
#define OPENOCPP_RESERVATIONMANAGER_H

#include "CancelReservation.h"
#include "Enums.h"
#include "GenericMessageHandler.h"
#include "ReserveNow.h"
#include "Timer.h"

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
} // namespace messages
namespace helpers
{
class WorkerThreadPool;
} // namespace helpers

// Main namespace
namespace chargepoint
{

class IAuthentManager;
class IStatusManager;
class Connectors;
class IChargePointEventsHandler;

/** @brief Handle charge point reservation requests */
class ReservationManager
    : public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::ReserveNowReq, ocpp::messages::ocpp16::ReserveNowConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::CancelReservationReq,
                                                   ocpp::messages::ocpp16::CancelReservationConf>
{
  public:
    /** @brief Constructor */
    ReservationManager(ocpp::config::IOcppConfig&                      ocpp_config,
                       IChargePointEventsHandler&                      events_handler,
                       ocpp::helpers::ITimerPool&                      timer_pool,
                       ocpp::helpers::WorkerThreadPool&                worker_pool,
                       Connectors&                                     connectors,
                       const ocpp::messages::GenericMessagesConverter& messages_converter,
                       ocpp::messages::IMessageDispatcher&             msg_dispatcher,
                       IStatusManager&                                 status_manager,
                       IAuthentManager&                                authent_manager);

    /** @brief Destructor */
    virtual ~ReservationManager();

    /**
     * @brief Clear reservation data associated to a connector
     * @param connector_id Id of the connector
     */
    void clearReservation(unsigned int connector_id);

    /**
     * @brief Indicate if a transaction is allowed on a connector using a specific id tag
     * @param connector_id Id of the connector
     * @param id_tag Id of the user
     * @return ocpp::types::ocpp16::AuthorizationStatus (see AuthorizationStatus enum)
     */
    ocpp::types::ocpp16::AuthorizationStatus isTransactionAllowed(unsigned int connector_id, const std::string& id_tag);

    // GenericMessageHandler interface

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::ReserveNowReq& request,
                       ocpp::messages::ocpp16::ReserveNowConf&      response,
                       std::string&                                 error_code,
                       std::string&                                 error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::CancelReservationReq& request,
                       ocpp::messages::ocpp16::CancelReservationConf&      response,
                       std::string&                                        error_code,
                       std::string&                                        error_message) override;

  private:
    /** @brief Standard OCPP configuration */
    ocpp::config::IOcppConfig& m_ocpp_config;
    /** @brief User defined events handler */
    IChargePointEventsHandler& m_events_handler;
    /** @brief Worker thread pool */
    ocpp::helpers::WorkerThreadPool& m_worker_pool;
    /** @brief Charge point's connectors */
    Connectors& m_connectors;
    /** @brief Status manager */
    IStatusManager& m_status_manager;
    /** @brief Authentication manager */
    IAuthentManager& m_authent_manager;

    /** @brief Periodic timer to check reservations expiry */
    ocpp::helpers::Timer m_expiry_timer;

    /** @brief Check the reservations expiries */
    void checkExpiries();

    /** @brief End the reservation for the given connector */
    void endReservation(unsigned int connector_id, bool canceled);
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_RESERVATIONMANAGER_H
