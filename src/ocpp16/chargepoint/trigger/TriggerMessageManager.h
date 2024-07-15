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

#ifndef OPENOCPP_TRIGGERMESSAGEMANAGER_H
#define OPENOCPP_TRIGGERMESSAGEMANAGER_H

#include "ExtendedTriggerMessage.h"
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
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::TriggerMessageReq, ocpp::messages::ocpp16::TriggerMessageConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::ExtendedTriggerMessageReq,
                                                   ocpp::messages::ocpp16::ExtendedTriggerMessageConf>
{
  public:
    /** @brief Constructor */
    TriggerMessageManager(Connectors&                                     connectors,
                          const ocpp::messages::GenericMessagesConverter& messages_converter,
                          ocpp::messages::IMessageDispatcher&             msg_dispatcher);

    /** @brief Destructor */
    virtual ~TriggerMessageManager();

    // ITriggerMessageManager interface

    /** @copydoc void ITriggerMessageManager::registerHandler(ocpp::types::ocpp16::MessageTrigger, ITriggerMessageHandler&) */
    void registerHandler(ocpp::types::ocpp16::MessageTrigger message, ITriggerMessageHandler& handler) override;

    /** @copydoc void ITriggerMessageManager::registerHandler(ocpp::types::ocpp16::MessageTriggerEnumType, IExtendedTriggerMessageHandler&) */
    void registerHandler(ocpp::types::ocpp16::MessageTriggerEnumType message, IExtendedTriggerMessageHandler& handler) override;

    // GenericMessageHandler interface

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::TriggerMessageReq& request,
                       ocpp::messages::ocpp16::TriggerMessageConf&      response,
                       std::string&                                     error_code,
                       std::string&                                     error_message) override;

    // Security extensions

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::ExtendedTriggerMessageReq& request,
                       ocpp::messages::ocpp16::ExtendedTriggerMessageConf&      response,
                       std::string&                                             error_code,
                       std::string&                                             error_message) override;

  private:
    /** @brief Charge point's connectors */
    Connectors& m_connectors;

    /** @brief Handlers for standard trigger messages */
    std::map<ocpp::types::ocpp16::MessageTrigger, ITriggerMessageHandler*> m_standard_handlers;

    /** @brief Handlers for extended trigger messages */
    std::map<ocpp::types::ocpp16::MessageTriggerEnumType, IExtendedTriggerMessageHandler*> m_extended_handlers;
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_TRIGGERMESSAGEMANAGER_H
