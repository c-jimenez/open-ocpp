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

#ifndef OPENOCPP_REQUESTFIFOMANAGER_H
#define OPENOCPP_REQUESTFIFOMANAGER_H

#include <mutex>

#include "IRequestFifo.h"
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
class GenericMessageSender;
} // namespace messages
namespace helpers
{
class WorkerThreadPool;
} // namespace helpers

// Main namespace
namespace chargepoint
{

class IAuthentManager;
class Connectors;
class IStatusManager;
class IChargePointEventsHandler;

/** @brief Handle charge point FIFO requests */
class RequestFifoManager : public ocpp::messages::IRequestFifo::IListener
{
  public:
    /** @brief Constructor */
    RequestFifoManager(ocpp::config::IOcppConfig&            ocpp_config,
                       IChargePointEventsHandler&            events_handler,
                       ocpp::helpers::ITimerPool&            timer_pool,
                       ocpp::helpers::WorkerThreadPool&      worker_pool,
                       Connectors&                           connectors,
                       ocpp::messages::GenericMessageSender& msg_sender,
                       ocpp::messages::IRequestFifo&         requests_fifo,
                       IStatusManager&                       status_manager,
                       IAuthentManager&                      authent_manager);

    /** @brief Destructor */
    virtual ~RequestFifoManager();

    /**
     * @brief Update the charge point connection status
     * @param is_connected true if the charge point is connected to the central system, false otherwise
     */
    void updateConnectionStatus(bool is_connected);

    // IRequestFifo::IListener interface

    /** @copydoc void IRequestFifo::IListenerrequestQueued() */
    void requestQueued() override;

  private:
    /** @brief Standard OCPP configuration */
    ocpp::config::IOcppConfig& m_ocpp_config;
    /** @brief User defined events handler */
    IChargePointEventsHandler& m_events_handler;
    /** @brief Worker thread pool */
    ocpp::helpers::WorkerThreadPool& m_worker_pool;
    /** @brief Charge point's connectors */
    Connectors& m_connectors;
    /** @brief Message sender */
    ocpp::messages::GenericMessageSender& m_msg_sender;
    /** @brief Status manager */
    IStatusManager& m_status_manager;
    /** @brief Authentication manager */
    IAuthentManager& m_authent_manager;

    /** @brief Requests FIFO */
    ocpp::messages::IRequestFifo& m_requests_fifo;
    /** @brief FIFO retry timer */
    ocpp::helpers::Timer m_request_retry_timer;
    /** @brief Retry count for the current request */
    unsigned int m_request_retry_count;

    /** @brief Mutex protectiing against concurrent processFifoRequest calls */
    std::mutex m_process_mutex;

    /** @brief Process a FIFO request */
    void processFifoRequest();
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_REQUESTFIFOMANAGER_H
