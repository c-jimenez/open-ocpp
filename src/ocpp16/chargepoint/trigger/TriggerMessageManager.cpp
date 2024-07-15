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

#include "TriggerMessageManager.h"
#include "Connectors.h"
#include "IRpc.h"
#include "Logger.h"

using namespace ocpp::messages;
using namespace ocpp::messages::ocpp16;
using namespace ocpp::types;
using namespace ocpp::types::ocpp16;
namespace ocpp
{
namespace chargepoint
{

/** @brief Constructor */
TriggerMessageManager::TriggerMessageManager(Connectors&                                     connectors,
                                             const ocpp::messages::GenericMessagesConverter& messages_converter,
                                             ocpp::messages::IMessageDispatcher&             msg_dispatcher)
    : GenericMessageHandler<TriggerMessageReq, TriggerMessageConf>(TRIGGER_MESSAGE_ACTION, messages_converter),
      GenericMessageHandler<ExtendedTriggerMessageReq, ExtendedTriggerMessageConf>(EXTENDED_TRIGGER_MESSAGE_ACTION, messages_converter),
      m_connectors(connectors),
      m_standard_handlers(),
      m_extended_handlers()
{
    msg_dispatcher.registerHandler(TRIGGER_MESSAGE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<TriggerMessageReq, TriggerMessageConf>*>(this));
    msg_dispatcher.registerHandler(EXTENDED_TRIGGER_MESSAGE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<ExtendedTriggerMessageReq, ExtendedTriggerMessageConf>*>(this));
}

/** @brief Destructor */
TriggerMessageManager::~TriggerMessageManager() { }

/** @copydoc void ITriggerMessageManager::registerHandler(ocpp::types::ocpp16::MessageTrigger, ITriggerMessageHandler&) */
void TriggerMessageManager::registerHandler(ocpp::types::ocpp16::MessageTrigger message, ITriggerMessageHandler& handler)
{
    m_standard_handlers[message] = &handler;
}

/** @copydoc void ITriggerMessageManager::registerHandler(ocpp::types::ocpp16::MessageTriggerEnumType, IExtendedTriggerMessageHandler&) */
void TriggerMessageManager::registerHandler(ocpp::types::ocpp16::MessageTriggerEnumType message, IExtendedTriggerMessageHandler& handler)
{
    m_extended_handlers[message] = &handler;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool TriggerMessageManager::handleMessage(const ocpp::messages::ocpp16::TriggerMessageReq& request,
                                          ocpp::messages::ocpp16::TriggerMessageConf&      response,
                                          std::string&                                     error_code,
                                          std::string&                                     error_message)
{
    bool ret = true;

    std::string trigger_message = MessageTriggerHelper.toString(request.requestedMessage);
    LOG_INFO << "Trigger message requested : " << trigger_message
             << " - connectorId = " << (request.connectorId.isSet() ? std::to_string(request.connectorId) : "not set");

    // Look for the corresponding handler
    auto it = m_standard_handlers.find(request.requestedMessage);
    if (it == m_standard_handlers.end())
    {
        // No handler => not implemented
        response.status = TriggerMessageStatus::NotImplemented;
        LOG_WARNING << "Trigger message not implemented : " << trigger_message;
    }
    else
    {
        // Check invalid connector id
        if (request.connectorId.isSet() && !m_connectors.isValid(request.connectorId))
        {
            error_code      = ocpp::rpc::IRpc::RPC_ERROR_PROPERTY_CONSTRAINT_VIOLATION;
            error_message   = "Invalid connector id";
            response.status = TriggerMessageStatus::Rejected;
        }
        else
        {
            // Call handler
            if (it->second->onTriggerMessage(request.requestedMessage, request.connectorId))
            {
                response.status = TriggerMessageStatus::Accepted;
                LOG_INFO << "Trigger message accepted : " << trigger_message;
            }
            else
            {
                response.status = TriggerMessageStatus::Rejected;
                LOG_WARNING << "Trigger message rejected : " << trigger_message;
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
bool TriggerMessageManager::handleMessage(const ocpp::messages::ocpp16::ExtendedTriggerMessageReq& request,
                                          ocpp::messages::ocpp16::ExtendedTriggerMessageConf&      response,
                                          std::string&                                             error_code,
                                          std::string&                                             error_message)
{
    bool ret = true;

    std::string trigger_message = MessageTriggerEnumTypeHelper.toString(request.requestedMessage);
    LOG_INFO << "Extended trigger message requested : " << trigger_message
             << " - connectorId = " << (request.connectorId.isSet() ? std::to_string(request.connectorId) : "not set");

    // Look for the corresponding handler
    auto it = m_extended_handlers.find(request.requestedMessage);
    if (it == m_extended_handlers.end())
    {
        // No handler => not implemented
        response.status = TriggerMessageStatusEnumType::NotImplemented;
        LOG_WARNING << "Extended trigger message not implemented : " << trigger_message;
    }
    else
    {
        // Check invalid connector id
        if (request.connectorId.isSet() && !m_connectors.isValid(request.connectorId))
        {
            error_code      = ocpp::rpc::IRpc::RPC_ERROR_PROPERTY_CONSTRAINT_VIOLATION;
            error_message   = "Invalid connector id";
            response.status = TriggerMessageStatusEnumType::Rejected;
        }
        else
        {
            // Call handler
            if (it->second->onTriggerMessage(request.requestedMessage, request.connectorId))
            {
                response.status = TriggerMessageStatusEnumType::Accepted;
                LOG_INFO << "Extended trigger message accepted : " << trigger_message;
            }
            else
            {
                response.status = TriggerMessageStatusEnumType::Rejected;
                LOG_WARNING << "Extended trigger message rejected : " << trigger_message;
            }
        }
    }

    return ret;
}

} // namespace chargepoint
} // namespace ocpp
