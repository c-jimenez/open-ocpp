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

#include "ConfigManager.h"
#include "IOcppConfig.h"

using namespace ocpp::messages;
using namespace ocpp::messages::ocpp16;
using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

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
    auto lower_case_key               = ocpp::helpers::tolower(key);
    m_specific_checks[lower_case_key] = func;
}

/** @copydoc void IConfigManager::registerConfigChangedListener(const std::string&, IConfigChangedListener&) */
void ConfigManager::registerConfigChangedListener(const std::string& key, IConfigChangedListener& listener)
{
    auto lower_case_key         = ocpp::helpers::tolower(key);
    m_listeners[lower_case_key] = &listener;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool ConfigManager::handleMessage(const ocpp::messages::ocpp16::GetConfigurationReq& request,
                                  ocpp::messages::ocpp16::GetConfigurationConf&      response,
                                  std::string&                                       error_code,
                                  std::string&                                       error_message)
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
bool ConfigManager::handleMessage(const ocpp::messages::ocpp16::ChangeConfigurationReq& request,
                                  ocpp::messages::ocpp16::ChangeConfigurationConf&      response,
                                  std::string&                                          error_code,
                                  std::string&                                          error_message)
{
    (void)error_code;
    (void)error_message;

    // Default
    response.status = ConfigurationStatus::Accepted;

    // Specific check
    auto lower_case_key = ocpp::helpers::tolower(request.key);
    auto it             = m_specific_checks.find(lower_case_key);
    if (it != m_specific_checks.end())
    {
        response.status = it->second(lower_case_key, request.value);
    }
    if (response.status == ConfigurationStatus::Accepted)
    {
        // Set configuration value
        response.status = m_ocpp_config.setConfiguration(request.key, request.value);
        if (response.status == ConfigurationStatus::Accepted)
        {
            // Notify change
            auto iter = m_listeners.find(lower_case_key);
            if (iter != m_listeners.end())
            {
                iter->second->configurationValueChanged(lower_case_key);
            }
        }
    }
    return true;
}

} // namespace chargepoint
} // namespace ocpp
