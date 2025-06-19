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

#include "RequestFifoManager.h"
#include "Connectors.h"
#include "GenericMessageSender.h"
#include "IAuthentManager.h"
#include "IChargePointEventsHandler.h"
#include "IOcppConfig.h"
#include "IStatusManager.h"
#include "Logger.h"
#include "MeterValues.h"
#include "SecurityEventNotification.h"
#include "StartTransaction.h"
#include "StopTransaction.h"
#include "WorkerThreadPool.h"

using namespace ocpp::messages;
using namespace ocpp::messages::ocpp16;
using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace chargepoint
{

/** @brief Constructor */
RequestFifoManager::RequestFifoManager(ocpp::config::IOcppConfig&            ocpp_config,
                                       IChargePointEventsHandler&            events_handler,
                                       ocpp::helpers::ITimerPool&            timer_pool,
                                       ocpp::helpers::WorkerThreadPool&      worker_pool,
                                       Connectors&                           connectors,
                                       ocpp::messages::GenericMessageSender& msg_sender,
                                       ocpp::messages::IRequestFifo&         requests_fifo,
                                       IStatusManager&                       status_manager,
                                       IAuthentManager&                      authent_manager)
    : m_ocpp_config(ocpp_config),
      m_events_handler(events_handler),
      m_worker_pool(worker_pool),
      m_connectors(connectors),
      m_msg_sender(msg_sender),
      m_status_manager(status_manager),
      m_authent_manager(authent_manager),
      m_requests_fifo(requests_fifo),
      m_request_retry_timer(timer_pool, "Requests FIFO"),
      m_request_retry_count(0)
{
    m_request_retry_timer.setCallback([this] { m_worker_pool.run<void>(std::bind(&RequestFifoManager::processFifoRequest, this)); });
    m_requests_fifo.registerListener(this);
}

/** @brief Destructor */
RequestFifoManager::~RequestFifoManager()
{
    m_requests_fifo.registerListener(nullptr);
}

/** @brief Update the charge point connection status */
void RequestFifoManager::updateConnectionStatus(bool is_connected)
{
    if (is_connected)
    {
        // Check if the FIFO must be emptied
        if (!m_requests_fifo.empty())
        {
            LOG_INFO << "Restart transaction related FIFO processing";

            // Start processing FIFO requests
            m_worker_pool.run<void>(std::bind(&RequestFifoManager::processFifoRequest, this));
        }
    }
}

/** @copydoc void IRequestFifo::IListenerrequestQueued() */
void RequestFifoManager::requestQueued()
{
    if (m_msg_sender.isConnected() && !m_request_retry_timer.isStarted())
    {
        // Start processing FIFO requests
        LOG_DEBUG << "Request failed, next retry in " << m_ocpp_config.transactionMessageRetryInterval().count() << "second(s)";
        m_request_retry_timer.restart(std::chrono::seconds(m_ocpp_config.transactionMessageRetryInterval()), true);
    }
}

/** @brief Process a FIFO request */
void RequestFifoManager::processFifoRequest()
{
    std::lock_guard lock(m_process_mutex);

    // Check the connection state
    if (m_msg_sender.isConnected())
    {
        // Check registration status
        if (m_status_manager.getRegistrationStatus() == RegistrationStatus::Accepted)
        {
            do
            {
                // Get request
                std::string         action;
                rapidjson::Document payload;
                unsigned int        connector_id;
                if (m_requests_fifo.front(connector_id, action, payload))
                {
                    LOG_DEBUG << "Request FIFO processing " << action << " retries : " << m_request_retry_count << "/"
                              << m_ocpp_config.transactionMessageAttempts();

                    // Send request
                    CallResult res;
                    if (action == START_TRANSACTION_ACTION)
                    {
                        // Start transaction => result contains validity information
                        StartTransactionConf response;
                        res = m_msg_sender.call(action, payload, response);
                        if (res == CallResult::Ok)
                        {
                            // Extract transaction from the request
                            StartTransactionReq          request;
                            StartTransactionReqConverter req_converter;
                            std::string                  error_message;
                            std::string                  error_code;
                            req_converter.fromJson(payload, request, error_code, error_message);

                            // Update id tag information
                            if (response.idTagInfo.status != AuthorizationStatus::ConcurrentTx)
                            {
                                m_authent_manager.update(request.idTag, response.idTagInfo);
                            }

                            // Save the offline transaction id
                            Connector* connector = m_connectors.getConnector(request.connectorId);
                            if (connector)
                            {
                                std::lock_guard<std::mutex> lock(connector->mutex);
                                connector->transaction_id_offline = response.transactionId;
                                m_connectors.saveConnector(request.connectorId);
                            }

                            // Check if transaction has been rejected by the Central System
                            if (response.idTagInfo.status != AuthorizationStatus::Accepted)
                            {
                                // Look for the corresponding transaction
                                if (connector && (connector->transaction_id < 0) && (connector->transaction_start == request.timestamp))
                                {
                                    std::lock_guard<std::mutex> lock(connector->mutex);

                                    // Update current transaction id
                                    connector->transaction_id = connector->transaction_id_offline;
                                    m_connectors.saveConnector(request.connectorId);

                                    // Notify end of transaction
                                    m_events_handler.transactionDeAuthorized(connector->id);
                                }
                            }
                        }
                    }
                    else if (action == STOP_TRANSACTION_ACTION)
                    {
                        // Stop transaction => update transaction id if needed and ignore response

                        int stop_transaction_id = payload["transactionId"].GetInt();
                        if (stop_transaction_id < 0)
                        {
                            // Get the offline transaction id
                            Connector* connector = m_connectors.getConnector(connector_id);
                            if (connector)
                            {
                                payload["transactionId"].SetInt(connector->transaction_id_offline);
                            }
                        }

                        StopTransactionConf response;
                        res = m_msg_sender.call(action, payload, response);
                    }
                    else if (action == METER_VALUES_ACTION)
                    {
                        // Meter values => update transaction id if needed and ignore response

                        if (payload.HasMember("transactionId"))
                        {
                            int meter_transaction_id = payload["transactionId"].GetInt();
                            if (meter_transaction_id < 0)
                            {
                                // Get the offline transaction id
                                Connector* connector = m_connectors.getConnector(connector_id);
                                if (connector)
                                {
                                    payload["transactionId"].SetInt(connector->transaction_id_offline);
                                }
                            }
                        }

                        MeterValuesConf response;
                        res = m_msg_sender.call(action, payload, response);
                    }
                    else if (action == SECURITY_EVENT_NOTIFICATION_ACTION)
                    {
                        // Security events notification => ignore response

                        SecurityEventNotificationConf response;
                        res = m_msg_sender.call(action, payload, response);
                    }
                    else
                    {
                        // Unknown action
                        res = CallResult::Failed;
                    }
                    if (res == CallResult::Ok)
                    {
                        LOG_DEBUG << "Request succeeded";

                        // Remove request from the FIFO
                        m_requests_fifo.pop();
                        m_request_retry_count = 0;
                    }
                    else
                    {
                        // Update retry count
                        m_request_retry_count++;
                        if (m_request_retry_count > m_ocpp_config.transactionMessageAttempts())
                        {
                            // Drop message from the FIFO
                            LOG_DEBUG << "Request failed, drop message";
                            m_requests_fifo.pop();
                            m_request_retry_count = 0;
                        }
                        else
                        {
                            // Schedule next retry
                            if (m_msg_sender.isConnected())
                            {
                                LOG_DEBUG << "Request failed, next retry in " << m_ocpp_config.transactionMessageRetryInterval().count()
                                          << "second(s)";
                                m_request_retry_timer.restart(std::chrono::seconds(m_ocpp_config.transactionMessageRetryInterval()), true);
                            }
                        }
                    }
                }
            } while (!m_requests_fifo.empty() && !m_request_retry_timer.isStarted() && m_msg_sender.isConnected());

            // Update current transaction ids if needed
            if (m_requests_fifo.empty())
            {
                for (Connector* connector : m_connectors.getConnectors())
                {
                    std::lock_guard<std::mutex> lock(connector->mutex);
                    if (connector->transaction_id < 0)
                    {
                        connector->transaction_id = connector->transaction_id_offline;
                        m_connectors.saveConnector(connector->id);
                    }
                }
            }
        }
        else
        {
            // Wait to be accepted by the Central System
            m_request_retry_timer.restart(std::chrono::milliseconds(250u), true);
        }
    }
}

} // namespace chargepoint
} // namespace ocpp
