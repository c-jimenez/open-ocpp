/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#include "DataTransferManager.h"
#include "GenericMessageSender.h"
#include "IChargePointEventsHandler.h"

using namespace ocpp::types;
using namespace ocpp::messages;

namespace ocpp
{
namespace chargepoint
{

/** @brief Constructor */
DataTransferManager::DataTransferManager(IChargePointEventsHandler&                      events_handler,
                                         const ocpp::messages::GenericMessagesConverter& messages_converter,
                                         ocpp::messages::IMessageDispatcher&             msg_dispatcher,
                                         ocpp::messages::GenericMessageSender&           msg_sender)
    : GenericMessageHandler<DataTransferReq, DataTransferConf>(DATA_TRANSFER_ACTION, messages_converter),
      m_events_handler(events_handler),
      m_msg_sender(msg_sender)
{
    msg_dispatcher.registerHandler(DATA_TRANSFER_ACTION, *this);
}

/** @brief Destructor */
DataTransferManager::~DataTransferManager() { }

/** @brief Send a data transfer request */
bool DataTransferManager::dataTransfer(const std::string&               vendor_id,
                                       const std::string&               message_id,
                                       const std::string&               request_data,
                                       ocpp::types::DataTransferStatus& status,
                                       std::string&                     response_data)
{
    bool ret = false;

    // Fill request
    DataTransferReq request;
    request.vendorId.assign(vendor_id);
    if (!message_id.empty())
    {
        request.messageId.value().assign(message_id);
    }
    if (!request_data.empty())
    {
        request.data = request_data;
    }

    // Send request
    DataTransferConf response;
    if (m_msg_sender.call(DATA_TRANSFER_ACTION, request, response) == CallResult::Ok)
    {
        // Extract response
        status        = response.status;
        response_data = response.data;
        ret           = true;
    }

    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool DataTransferManager::handleMessage(const ocpp::messages::DataTransferReq& request,
                                        ocpp::messages::DataTransferConf&      response,
                                        const char*&                           error_code,
                                        std::string&                           error_message)
{
    (void)error_code;
    (void)error_message;

    // Notify request
    response.status =
        m_events_handler.dataTransferRequested(request.vendorId, request.messageId.value(), request.data, response.data.value());

    return true;
}

} // namespace chargepoint
} // namespace ocpp
