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

#ifndef OPENOCPP_OCPP20_TRIGGERMESSAGEMANAGER20_H
#define OPENOCPP_OCPP20_TRIGGERMESSAGEMANAGER20_H

#include "GenericMessageHandler.h"
#include "ITriggerMessageManager20.h"
#include "TriggerMessage20.h"

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
namespace ocpp20
{

class IConnectors;

/** @brief Manage TriggerMessage requests */
class TriggerMessageManager
    : public ITriggerMessageManager,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp20::TriggerMessageReq, ocpp::messages::ocpp20::TriggerMessageConf>
{
  public:
    /** @brief Constructor */
    TriggerMessageManager(IConnectors&                                    connectors,
                          const ocpp::messages::GenericMessagesConverter& messages_converter,
                          ocpp::messages::IMessageDispatcher&             msg_dispatcher);

    /** @brief Destructor */
    virtual ~TriggerMessageManager();

    // ITriggerMessageManager interface

    /** @copydoc void ITriggerMessageManager::registerHandler(ocpp::types::ocpp20::MessageTriggerEnumType, ITriggerMessageHandler&) */
    void registerHandler(ocpp::types::ocpp20::MessageTriggerEnumType message, ITriggerMessageHandler& handler) override;

    // GenericMessageHandler interface

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp20::TriggerMessageReq& request,
                       ocpp::messages::ocpp20::TriggerMessageConf&      response,
                       std::string&                                     error_code,
                       std::string&                                     error_message) override;

  private:
    /** @brief Charge point's connectors */
    IConnectors& m_connectors;

    /** @brief Handlers for trigger messages */
    std::map<ocpp::types::ocpp20::MessageTriggerEnumType, ITriggerMessageHandler*> m_standard_handlers;
};

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_OCPP20_TRIGGERMESSAGEMANAGER20_H
