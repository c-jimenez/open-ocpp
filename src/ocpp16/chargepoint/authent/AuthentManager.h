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

#ifndef OPENOCPP_AUTHENTMANAGER_H
#define OPENOCPP_AUTHENTMANAGER_H

#include "IAuthentManager.h"

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
class AuthentManager : public IAuthentManager
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

    /** @copydoc ocpp::types::ocpp16::AuthorizationStatus IAuthentManager::authorize(const std::string&, std::string&) */
    ocpp::types::ocpp16::AuthorizationStatus authorize(const std::string& id_tag, std::string& parent_id) override;

    /** @copydoc void IAuthentManager::update(const std::string& id_tag, const ocpp::types::ocpp16::IdTagInfo&) */
    void update(const std::string& id_tag, const ocpp::types::ocpp16::IdTagInfo& tag_info) override;

    /** @copydoc ocpp::types::ocpp16::AuthorizationStatus IAuthentManager::iso15118Authorize(const std::string&) */
    ocpp::types::ocpp16::AuthorizationStatus iso15118Authorize(const std::string& token_id) override;

    /** @copydoc void IAuthentManager::iso15118Update(const std::string&, const ocpp::types::ocpp16::IdTokenInfoType&) */
    void iso15118Update(const std::string& token_id, const ocpp::types::ocpp16::IdTokenInfoType& token_info) override;

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

#endif // OPENOCPP_AUTHENTMANAGER_H
