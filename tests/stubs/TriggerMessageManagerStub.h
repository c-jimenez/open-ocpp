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

#ifndef TRIGGERMESSAGEMANAGERSTUB_H
#define TRIGGERMESSAGEMANAGERSTUB_H

#include "ITriggerMessageManager.h"

#include <map>

namespace ocpp
{
namespace chargepoint
{

/** @brief TriggerMessage manager stub for unit tests */
class TriggerMessageManagerStub : public ITriggerMessageManager
{
  public:
    /** @brief Constructor */
    TriggerMessageManagerStub() : m_standard_handlers() { }
    /** @brief Destructor */
    virtual ~TriggerMessageManagerStub() { }

    /** @copydoc void ITriggerMessageManager::registerHandler(ocpp::types::ocpp16::MessageTrigger, ITriggerMessageHandler&) */
    void registerHandler(ocpp::types::ocpp16::MessageTrigger message, ITriggerMessageHandler& handler) override
    {
        m_standard_handlers[message] = &handler;
    }

    /** @copydoc void ITriggerMessageManager::registerHandler(ocpp::types::ocpp16::MessageTriggerEnumType, IExtendedTriggerMessageHandler&) */
    void registerHandler(ocpp::types::ocpp16::MessageTriggerEnumType message, IExtendedTriggerMessageHandler& handler) override
    {
        m_extended_handlers[message] = &handler;
    }

    // API

    /** @brief Check if a handler has been registered for a message */
    bool isMessageHandlerRegistered(ocpp::types::ocpp16::MessageTrigger message) const
    {
        return (m_standard_handlers.find(message) != m_standard_handlers.end());
    }

    /** @brief Check if a handler has been registered for a message */
    bool isMessageHandlerRegistered(ocpp::types::ocpp16::MessageTriggerEnumType message) const
    {
        return (m_extended_handlers.find(message) != m_extended_handlers.end());
    }

  private:
    /** @brief Handlers for standard trigger messages */
    std::map<ocpp::types::ocpp16::MessageTrigger, ITriggerMessageHandler*> m_standard_handlers;

    /** @brief Handlers for extended trigger messages */
    std::map<ocpp::types::ocpp16::MessageTriggerEnumType, IExtendedTriggerMessageHandler*> m_extended_handlers;
};

} // namespace chargepoint
} // namespace ocpp

#endif // TRIGGERMESSAGEMANAGERSTUB_H
