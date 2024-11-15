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

#include "TriggerMessageManager20.h"
#include "IConnectors20.h"
#include "IRpc.h"
#include "Logger.h"

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

/** @brief Constructor */
TriggerMessageManager::TriggerMessageManager(IConnectors&                                    connectors,
                                             const ocpp::messages::GenericMessagesConverter& messages_converter,
                                             ocpp::messages::IMessageDispatcher&             msg_dispatcher)
    : GenericMessageHandler<TriggerMessageReq, TriggerMessageConf>(TRIGGERMESSAGE_ACTION, messages_converter),
      m_connectors(connectors),
      m_standard_handlers()
{
    msg_dispatcher.registerHandler(TRIGGERMESSAGE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<TriggerMessageReq, TriggerMessageConf>*>(this));
}

/** @brief Destructor */
TriggerMessageManager::~TriggerMessageManager() { }

/** @copydoc void ITriggerMessageManager::registerHandler(ocpp::types::ocpp20::MessageTriggerEnumType, ITriggerMessageHandler&) */
void TriggerMessageManager::registerHandler(ocpp::types::ocpp20::MessageTriggerEnumType message, ITriggerMessageHandler& handler)
{
    m_standard_handlers[message] = &handler;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool TriggerMessageManager::handleMessage(const ocpp::messages::ocpp20::TriggerMessageReq& request,
                                          ocpp::messages::ocpp20::TriggerMessageConf&      response,
                                          std::string&                                     error_code,
                                          std::string&                                     error_message)
{
    bool ret = true;

    (void)error_code;
    (void)error_message;

    std::string trigger_message = MessageTriggerEnumTypeHelper.toString(request.requestedMessage);
    LOG_INFO << "Trigger message requested : " << trigger_message
             << " - EVSE = " << (request.evse.isSet() ? std::to_string(request.evse.value().id) : "not set") << " - connectorId = "
             << ((request.evse.isSet() && request.evse.value().connectorId.isSet())
                     ? std::to_string(request.evse.value().connectorId.value())
                     : "not set");

    // Look for the corresponding handler
    auto it = m_standard_handlers.find(request.requestedMessage);
    if (it == m_standard_handlers.end())
    {
        // No handler => not implemented
        response.status = TriggerMessageStatusEnumType::NotImplemented;
        LOG_WARNING << "Trigger message not implemented : " << trigger_message;
    }
    else
    {
        // Check EVSE/connector validity
        response.status = TriggerMessageStatusEnumType::Accepted;
        if (request.evse.isSet())
        {
            if (m_connectors.getEvse(request.evse.value().id))
            {
                if (request.evse.value().connectorId.isSet() &&
                    !m_connectors.getConnector(request.evse.value().id, request.evse.value().connectorId.value()))
                {
                    LOG_ERROR << "Trigger message, invalid connector id : EVSE id = " << request.evse.value().id
                              << " - Connector id = " << request.evse.value().connectorId.value();
                    response.status = TriggerMessageStatusEnumType::Rejected;
                }
            }
            else
            {
                LOG_ERROR << "Trigger message, invalid EVSE id : " << request.evse.value().id;
                response.status = TriggerMessageStatusEnumType::Rejected;
            }
        }
        if (response.status == TriggerMessageStatusEnumType::Accepted)
        {
            // Call handler
            if (it->second->onTriggerMessage(request.requestedMessage, request.evse))
            {
                LOG_INFO << "Trigger message accepted : " << trigger_message;
            }
            else
            {
                response.status = TriggerMessageStatusEnumType::Rejected;
                LOG_WARNING << "Trigger message rejected : " << trigger_message;
            }
        }
    }

    return ret;
}

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp
