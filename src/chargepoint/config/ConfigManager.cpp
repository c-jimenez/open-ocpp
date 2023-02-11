/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License version 2.1
as published by the Free Software Foundation.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#include "ConfigManager.h"
#include "IOcppConfig.h"

using namespace ocpp::messages;
using namespace ocpp::types;

namespace ocpp
{
namespace chargepoint
{

/** @brief Constructor */
ConfigManager::ConfigManager(ocpp::config::IOcppConfig&                      ocpp_config,
                             const ocpp::messages::GenericMessagesConverter& messages_converter,
                             ocpp::messages::IMessageDispatcher&             msg_dispatcher)
    : GenericMessageHandler<GetConfigurationReq, GetConfigurationConf>(GET_CONFIGURATION_ACTION, messages_converter),
      GenericMessageHandler<ChangeConfigurationReq, ChangeConfigurationConf>(CHANGE_CONFIGURATION_ACTION, messages_converter),
      m_ocpp_config(ocpp_config),
      m_specific_checks(),
      m_listeners()
{
    msg_dispatcher.registerHandler(GET_CONFIGURATION_ACTION,
                                   *dynamic_cast<GenericMessageHandler<GetConfigurationReq, GetConfigurationConf>*>(this));
    msg_dispatcher.registerHandler(CHANGE_CONFIGURATION_ACTION,
                                   *dynamic_cast<GenericMessageHandler<ChangeConfigurationReq, ChangeConfigurationConf>*>(this));
}

/** @brief Destructor */
ConfigManager::~ConfigManager() { }

/** @copydoc void registerCheckFunction(const std::string&, ConfigurationValueCheckFunc) */
void ConfigManager::registerCheckFunction(const std::string& key, ConfigurationValueCheckFunc func)
{
    m_specific_checks[key] = func;
}

/** @copydoc void IConfigManager::registerConfigChangedListener(const std::string&, IConfigChangedListener&) */
void ConfigManager::registerConfigChangedListener(const std::string& key, IConfigChangedListener& listener)
{
    m_listeners[key] = &listener;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ConfigManager::handleMessage(const ocpp::messages::GetConfigurationReq& request,
                                  ocpp::messages::GetConfigurationConf&      response,
                                  std::string&                               error_code,
                                  std::string&                               error_message)
{
    (void)error_code;
    (void)error_message;

    m_ocpp_config.getConfiguration(request.key, response.configurationKey, response.unknownKey);

    return true;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ConfigManager::handleMessage(const ocpp::messages::ChangeConfigurationReq& request,
                                  ocpp::messages::ChangeConfigurationConf&      response,
                                  std::string&                                  error_code,
                                  std::string&                                  error_message)
{
    (void)error_code;
    (void)error_message;

    // Default
    response.status = ConfigurationStatus::Accepted;

    // Specific check
    auto it = m_specific_checks.find(request.key);
    if (it != m_specific_checks.end())
    {
        response.status = it->second(request.key, request.value);
    }
    if (response.status == ConfigurationStatus::Accepted)
    {
        // Set configuration value
        response.status = m_ocpp_config.setConfiguration(request.key, request.value);
        if (response.status == ConfigurationStatus::Accepted)
        {
            // Notify change
            auto iter = m_listeners.find(request.key);
            if (iter != m_listeners.end())
            {
                iter->second->configurationValueChanged(request.key);
            }
        }
    }
    return true;
}

} // namespace chargepoint
} // namespace ocpp
