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

#ifndef TRIGGERMESSAGEMANAGER_H
#define TRIGGERMESSAGEMANAGER_H

#include "GenericMessageHandler.h"
#include "ITriggerMessageManager.h"
#include "TriggerMessage.h"

#include <map>

namespace ocpp
{
// Forward declarations
namespace messages
{
class IMessageDispatcher;
} // namespace messages

// Main namespace
namespace chargepoint
{

class Connectors;

/** @brief Manage TriggerMessage requests */
class TriggerMessageManager
    : public ITriggerMessageManager,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::TriggerMessageReq, ocpp::messages::TriggerMessageConf>
{
  public:
    /** @brief Constructor */
    TriggerMessageManager(Connectors&                                     connectors,
                          const ocpp::messages::GenericMessagesConverter& messages_converter,
                          ocpp::messages::IMessageDispatcher&             msg_dispatcher);

    /** @brief Destructor */
    virtual ~TriggerMessageManager();

    // ITriggerMessageManager interface

    /** @copydoc void ITriggerMessageManager::registerHandler(ocpp::types::MessageTrigger, ITriggerMessageHandler&) */
    void registerHandler(ocpp::types::MessageTrigger message, ITriggerMessageHandler& handler) override;

    // GenericMessageHandler interface

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                const char*& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::TriggerMessageReq& request,
                       ocpp::messages::TriggerMessageConf&      response,
                       const char*&                             error_code,
                       std::string&                             error_message) override;

  private:
    /** @brief Charge point's connectors */
    Connectors& m_connectors;
    /** @brief Message dispatcher */
    ocpp::messages::IMessageDispatcher& m_msg_dispatcher;

    /** @brief Handlers */
    std::map<ocpp::types::MessageTrigger, ITriggerMessageHandler*> m_handlers;
};

} // namespace chargepoint
} // namespace ocpp

#endif // TRIGGERMESSAGEMANAGER_H
