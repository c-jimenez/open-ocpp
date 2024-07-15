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

#ifndef OPENOCPP_CONFIGMANAGER_H
#define OPENOCPP_CONFIGMANAGER_H

#include "ChangeConfiguration.h"
#include "Enums.h"
#include "GenericMessageHandler.h"
#include "GetConfiguration.h"
#include "IConfigManager.h"

#include <unordered_map>

namespace ocpp
{
// Forward declarations
namespace config
{
class IOcppConfig;
} // namespace config

// Main namespace
namespace chargepoint
{

/** @brief Handle charge point configuration requests */
class ConfigManager : public IConfigManager,
                      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::GetConfigurationReq,
                                                                   ocpp::messages::ocpp16::GetConfigurationConf>,
                      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::ChangeConfigurationReq,
                                                                   ocpp::messages::ocpp16::ChangeConfigurationConf>
{
  public:
    /** @brief Constructor */
    ConfigManager(ocpp::config::IOcppConfig&                      ocpp_config,
                  const ocpp::messages::GenericMessagesConverter& messages_converter,
                  ocpp::messages::IMessageDispatcher&             msg_dispatcher);

    /** @brief Destructor */
    virtual ~ConfigManager();

    // IConfigManager interface

    /** @copydoc void IConfigManager::registerCheckFunction(const std::string&, ConfigurationValueCheckFunc) */
    void registerCheckFunction(const std::string& key, ConfigurationValueCheckFunc func) override;

    /** @copydoc void IConfigManager::registerConfigChangedListener(const std::string&, IConfigChangedListener&) */
    void registerConfigChangedListener(const std::string& key, IConfigChangedListener& listener) override;

    // GenericMessageHandler interface

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::GetConfigurationReq& request,
                       ocpp::messages::ocpp16::GetConfigurationConf&      response,
                       std::string&                                       error_code,
                       std::string&                                       error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::ChangeConfigurationReq& request,
                       ocpp::messages::ocpp16::ChangeConfigurationConf&      response,
                       std::string&                                          error_code,
                       std::string&                                          error_message) override;

  private:
    /** @brief Standard OCPP configuration */
    ocpp::config::IOcppConfig& m_ocpp_config;
    /** @brief Specific check functions */
    std::unordered_map<std::string, ConfigurationValueCheckFunc> m_specific_checks;
    /** @brief Configuration listeners */
    std::unordered_map<std::string, IConfigChangedListener*> m_listeners;
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_CONFIGMANAGER_H
