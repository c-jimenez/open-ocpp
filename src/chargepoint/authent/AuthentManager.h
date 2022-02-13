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

#ifndef AUTHENTMANAGER_H
#define AUTHENTMANAGER_H

#include "Enums.h"
#include "IdTagInfo.h"

namespace ocpp
{
// Forward declarations
namespace database
{
class Database;
}
namespace config
{
class IChargePointConfig;
class IOcppConfig;
class IInternalConfigManager;
} // namespace config
namespace messages
{
class IMessageDispatcher;
class GenericMessagesConverter;
class GenericMessageSender;
} // namespace messages

// Main namespace
namespace chargepoint
{

class AuthentCache;
class AuthentLocalList;

/** @brief Handle charge point authentication requests */
class AuthentManager
{
  public:
    /** @brief Constructor */
    AuthentManager(const ocpp::config::IChargePointConfig&         stack_config,
                   ocpp::config::IOcppConfig&                      ocpp_config,
                   ocpp::database::Database&                       database,
                   ocpp::config::IInternalConfigManager&           internal_config,
                   const ocpp::messages::GenericMessagesConverter& messages_converter,
                   ocpp::messages::IMessageDispatcher&             msg_dispatcher,
                   ocpp::messages::GenericMessageSender&           msg_sender);

    /** @brief Destructor */
    virtual ~AuthentManager();

    /**
     * @brief Ask for authorization of operation
     * @param id_tag Id of the user's
     * @param parent_id If of the user's parent tag
     * @return Authorization status (see AuthorizationStatus enum)
     */
    ocpp::types::AuthorizationStatus authorize(const std::string& id_tag, std::string& parent_id);

    /**
     * @brief Update a tag information
     * @param id_tag Id of the tag to update
     * @param id_tag_info New tag informations
     */
    void update(const std::string& id_tag, const ocpp::types::IdTagInfo& tag_info);

  private:
    /** @brief Standard OCPP configuration */
    ocpp::config::IOcppConfig& m_ocpp_config;
    /** @brief Message sender */
    ocpp::messages::GenericMessageSender& m_msg_sender;

    /** @brief Cache */
    AuthentCache& m_cache;
    /** @brief Local list */
    AuthentLocalList& m_local_list;
};

} // namespace chargepoint
} // namespace ocpp

#endif // AUTHENTMANAGER_H
