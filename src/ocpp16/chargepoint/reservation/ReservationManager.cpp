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

#include "ReservationManager.h"
#include "Connectors.h"
#include "IAuthentManager.h"
#include "IChargePointEventsHandler.h"
#include "IOcppConfig.h"
#include "IRpc.h"
#include "IStatusManager.h"
#include "WorkerThreadPool.h"

#include <functional>
#include <thread>

using namespace ocpp::messages;
using namespace ocpp::messages::ocpp16;
using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace chargepoint
{

/** @brief Constructor */
ReservationManager::ReservationManager(ocpp::config::IOcppConfig&                      ocpp_config,
                                       IChargePointEventsHandler&                      events_handler,
                                       ocpp::helpers::ITimerPool&                      timer_pool,
                                       ocpp::helpers::WorkerThreadPool&                worker_pool,
                                       Connectors&                                     connectors,
                                       const ocpp::messages::GenericMessagesConverter& messages_converter,
                                       ocpp::messages::IMessageDispatcher&             msg_dispatcher,
                                       IStatusManager&                                 status_manager,
                                       IAuthentManager&                                authent_manager)
    : GenericMessageHandler<ReserveNowReq, ReserveNowConf>(RESERVE_NOW_ACTION, messages_converter),
      GenericMessageHandler<CancelReservationReq, CancelReservationConf>(CANCEL_RESERVATION_ACTION, messages_converter),
      m_ocpp_config(ocpp_config),
      m_events_handler(events_handler),
      m_worker_pool(worker_pool),
      m_connectors(connectors),
      m_status_manager(status_manager),
      m_authent_manager(authent_manager),
      m_expiry_timer(timer_pool, "Reservation expiry")
{
    msg_dispatcher.registerHandler(RESERVE_NOW_ACTION, *dynamic_cast<GenericMessageHandler<ReserveNowReq, ReserveNowConf>*>(this));
    msg_dispatcher.registerHandler(CANCEL_RESERVATION_ACTION,
                                   *dynamic_cast<GenericMessageHandler<CancelReservationReq, CancelReservationConf>*>(this));

    m_expiry_timer.setCallback(std::bind(&ReservationManager::checkExpiries, this));
    m_expiry_timer.start(std::chrono::milliseconds(10000u));
}

/** @brief Destructor */
ReservationManager::~ReservationManager() { }

/** @brief Clear reservation data associated to a connector */
void ReservationManager::clearReservation(unsigned int connector_id)
{
    // Get connector
    Connector* connector = m_connectors.getConnector(connector_id);
    if (connector)
    {
        {
            std::lock_guard<std::mutex> lock(connector->mutex);

            // Reset reservation data
            connector->reservation_id            = 0;
            connector->reservation_id_tag        = "";
            connector->reservation_parent_id_tag = "";
            connector->reservation_expiry_date   = DateTime::now();
            m_connectors.saveConnector(connector->id);
        }
        if (connector_id == Connectors::CONNECTOR_ID_CHARGE_POINT)
        {
            // Update charge point status
            m_status_manager.updateConnectorStatus(Connectors::CONNECTOR_ID_CHARGE_POINT, ChargePointStatus::Available);
        }
    }
}

/** @brief Indicate if a transaction is allowed on a connector using a specific id tag */
ocpp::types::ocpp16::AuthorizationStatus ReservationManager::isTransactionAllowed(unsigned int connector_id, const std::string& id_tag)
{
    AuthorizationStatus ret = AuthorizationStatus::Invalid;

    // Get requested connector
    Connector* connector = m_connectors.getConnector(connector_id);
    if (connector)
    {
        // Check if connector is reserved
        if (!connector->reservation_id_tag.empty())
        {
            // Check if id tag match
            if (id_tag == connector->reservation_id_tag)
            {
                ret = AuthorizationStatus::Accepted;
            }
            else
            {
                // Check parent id tag
                if (!connector->reservation_parent_id_tag.empty())
                {
                    std::string parent_id;
                    m_authent_manager.authorize(id_tag, parent_id);
                    if (parent_id == connector->reservation_parent_id_tag)
                    {
                        ret = AuthorizationStatus::Accepted;
                    }
                }
            }
        }
        else
        {
            // Handle reservation on whole charge point
            if (m_ocpp_config.reserveConnectorZeroSupported())
            {
                // Check if connector 0 is reserved
                Connector& charge_point = m_connectors.getChargePointConnector();
                if (!charge_point.reservation_id_tag.empty())
                {
                    // Ensure that the module functions properly even when the gun is inserted first by the user.
                    if (m_connectors.getConnector(connector_id)->status == ChargePointStatus::Preparing)
                    {
                        ret = AuthorizationStatus::Accepted;
                    }
                    else
                    {
                        // At least 1 connector must stay available
                        unsigned int available_count = 0;
                        for (const Connector* c : m_connectors.getConnectors())
                        {
                            if (c->status == ChargePointStatus::Available)
                            {
                                available_count++;
                            }
                        }
                        if (available_count >= 1)
                        {
                            ret = AuthorizationStatus::Accepted;
                        }
                    }
                }
                else
                {
                    ret = AuthorizationStatus::Accepted;
                }
            }
            else
            {
                ret = AuthorizationStatus::Accepted;
            }
        }
    }
    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ReservationManager::handleMessage(const ocpp::messages::ocpp16::ReserveNowReq& request,
                                       ocpp::messages::ocpp16::ReserveNowConf&      response,
                                       std::string&                                 error_code,
                                       std::string&                                 error_message)
{
    bool ret = false;

    // Get requested connector
    Connector* connector = m_connectors.getConnector(request.connectorId);
    if (connector)
    {
        // Check if reservation is allowed on connector
        if (((request.connectorId != 0) || ((request.connectorId == 0) && m_ocpp_config.reserveConnectorZeroSupported())) &&
            (m_ocpp_config.supportedFeatureProfiles().find("Reservation") != std::string::npos))
        {
            std::lock_guard<std::mutex> lock(connector->mutex);

            // Check connector status
            switch (connector->status)
            {
                case ChargePointStatus::Preparing:
                // Intended fallthrough
                case ChargePointStatus::Charging:
                // Intended fallthrough
                case ChargePointStatus::SuspendedEV:
                // Intended fallthrough
                case ChargePointStatus::SuspendedEVSE:
                // Intended fallthrough
                case ChargePointStatus::Finishing:
                {
                    response.status = ReservationStatus::Occupied;
                    break;
                }

                case ChargePointStatus::Faulted:
                {
                    response.status = ReservationStatus::Faulted;
                    break;
                }

                case ChargePointStatus::Unavailable:
                {
                    response.status = ReservationStatus::Unavailable;
                    break;
                }

                case ChargePointStatus::Available:
                {
                    // Save reservation
                    connector->reservation_id            = request.reservationId;
                    connector->reservation_id_tag        = request.idTag;
                    connector->reservation_parent_id_tag = request.parentIdTag.value();
                    connector->reservation_expiry_date   = request.expiryDate;
                    response.status                      = ReservationStatus::Accepted;

                    // Update connector status and notify new status
                    m_worker_pool.run<void>(
                        [this, connector]
                        {
                            m_status_manager.updateConnectorStatus(connector->id, ChargePointStatus::Reserved);
                            m_events_handler.reservationStarted(connector->id);
                        });

                    break;
                }

                case ChargePointStatus::Reserved:
                {
                    // Check reservation id to update the reservation
                    if (request.reservationId == connector->reservation_id)
                    {
                        connector->reservation_id_tag        = request.idTag;
                        connector->reservation_parent_id_tag = request.parentIdTag.value();
                        connector->reservation_expiry_date   = request.expiryDate;
                        response.status                      = ReservationStatus::Accepted;
                    }
                    else
                    {
                        response.status = ReservationStatus::Rejected;
                    }
                    break;
                }

                default:
                {
                    response.status = ReservationStatus::Rejected;
                    break;
                }
            }
        }
        else
        {
            response.status = ReservationStatus::Rejected;
        }
        ret = true;
    }
    else
    {
        error_code    = ocpp::rpc::IRpc::RPC_ERROR_PROPERTY_CONSTRAINT_VIOLATION;
        error_message = "Invalid connector id";
    }

    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ReservationManager::handleMessage(const ocpp::messages::ocpp16::CancelReservationReq& request,
                                       ocpp::messages::ocpp16::CancelReservationConf&      response,
                                       std::string&                                        error_code,
                                       std::string&                                        error_message)
{
    (void)error_code;
    (void)error_message;

    // Look for corresponding reservation id
    response.status = CancelReservationStatus::Rejected;
    for (const Connector* connector : m_connectors.getConnectors())
    {
        if ((!connector->reservation_id_tag.empty()) && (connector->reservation_id == request.reservationId))
        {
            // Cancel reservation
            m_worker_pool.run<void>([this, connector_id = connector->id] { endReservation(connector_id, true); });

            // Prepare response
            response.status = CancelReservationStatus::Accepted;
            break;
        }
    }

    return true;
}

/** @brief Check the reservations expiries */
void ReservationManager::checkExpiries()
{
    // Get current date and time
    DateTime now = DateTime::now();

    // Check reservations
    for (const Connector* connector : m_connectors.getConnectors())
    {
        if ((!connector->reservation_id_tag.empty()) && (connector->reservation_expiry_date <= now))
        {
            // End reservation
            m_worker_pool.run<void>(std::bind(&ReservationManager::endReservation, this, connector->id, false));
        }
    }
}

/** @brief End the reservation for the given connector */
void ReservationManager::endReservation(unsigned int connector_id, bool canceled)
{
    // Reset reservation data
    clearReservation(connector_id);

    // Update connector state
    m_status_manager.updateConnectorStatus(connector_id, ChargePointStatus::Available);

    // Notify end of reservation
    m_events_handler.reservationEnded(connector_id, canceled);
}

} // namespace chargepoint
} // namespace ocpp
