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

#include "AuthentManager.h"
#include "AuthentCache.h"
#include "AuthentLocalList.h"
#include "Authorize.h"
#include "Connectors.h"
#include "GenericMessageSender.h"
#include "IChargePointConfig.h"
#include "IOcppConfig.h"
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
AuthentManager::AuthentManager(const ocpp::config::IChargePointConfig&         stack_config,
                               ocpp::config::IOcppConfig&                      ocpp_config,
                               ocpp::database::Database&                       database,
                               ocpp::config::IInternalConfigManager&           internal_config,
                               const ocpp::messages::GenericMessagesConverter& messages_converter,
                               ocpp::messages::IMessageDispatcher&             msg_dispatcher,
                               ocpp::messages::GenericMessageSender&           msg_sender)
    : m_ocpp_config(ocpp_config),
      m_msg_sender(msg_sender),
      m_cache(*new AuthentCache(stack_config, ocpp_config, database, messages_converter, msg_dispatcher)),
      m_local_list(*new AuthentLocalList(ocpp_config, database, internal_config, messages_converter, msg_dispatcher))
{
}

/** @brief Destructor */
AuthentManager::~AuthentManager()
{
    delete &m_cache;
    delete &m_local_list;
}

/** @copydoc ocpp::types::ocpp16::AuthorizationStatus IAuthentManager::authorize(const std::string&, std::string&) */
ocpp::types::ocpp16::AuthorizationStatus AuthentManager::authorize(const std::string& id_tag, std::string& parent_id)
{
    bool                retry;
    AuthorizationStatus status;

    // Check connection status
    bool is_connected = m_msg_sender.isConnected();

    // Authorize procedure
    parent_id = "";
    do
    {
        bool found = false;

        // Default = no retry, not authorized
        retry  = false;
        status = AuthorizationStatus::Invalid;

        // Check if local authorization is enabled
        if ((is_connected && m_ocpp_config.localPreAuthorize()) || (!is_connected && m_ocpp_config.localAuthorizeOffline()))
        {
            // Check local authorization list
            IdTagInfo tag_info;
            if (m_ocpp_config.localAuthListEnabled())
            {
                found = m_local_list.check(id_tag, tag_info);
                if (found)
                {
                    status    = tag_info.status;
                    parent_id = (tag_info.parentIdTag.isSet() ? tag_info.parentIdTag.value().str() : "");
                }
                LOG_DEBUG << "IdTag [" << id_tag << "] found in local list : " << found;
            }

            // Check local cache
            if (!found)
            {
                if (m_ocpp_config.authorizationCacheEnabled())
                {
                    found = m_cache.check(id_tag, tag_info);
                    if (found)
                    {
                        status    = tag_info.status;
                        parent_id = (tag_info.parentIdTag.isSet() ? tag_info.parentIdTag.value().str() : "");
                        if (is_connected && (status != AuthorizationStatus::Accepted))
                        {
                            // Online check is required to try to obtain an up to date status
                            found = false;
                        }
                    }
                    LOG_DEBUG << "IdTag [" << id_tag << "] found in cache : " << found;
                }
            }
        }

        if (!found)
        {
            // Check online status
            if (is_connected)
            {
                // Ask for central system authorization

                LOG_DEBUG << "Ask authorization to Central System for IdTag [" << id_tag << "]";

                AuthorizeReq authorize_req;
                authorize_req.idTag.assign(id_tag);
                AuthorizeConf authorize_conf;
                CallResult    result = m_msg_sender.call(AUTHORIZE_ACTION, authorize_req, authorize_conf);
                if (result == CallResult::Ok)
                {
                    // Get result
                    status = authorize_conf.idTagInfo.status;
                    if (authorize_conf.idTagInfo.parentIdTag.isSet())
                    {
                        parent_id = authorize_conf.idTagInfo.parentIdTag.value();
                    }
                    else
                    {
                        parent_id = "";
                    }

                    // Update cache
                    update(id_tag, authorize_conf.idTagInfo);
                }
                else
                {
                    // Retry with offline procedure
                    is_connected = false;
                    retry        = true;
                    LOG_WARNING << "No response to Authorize request for IdTag [" << id_tag << "], use offline procedure";
                }
            }
            else
            {
                // Check if unknown ids are allowed when offline
                if (m_ocpp_config.allowOfflineTxForUnknownId())
                {
                    status    = AuthorizationStatus::Accepted;
                    parent_id = "";

                    LOG_DEBUG << "IdTag [" << id_tag << "] unknown but accepted";
                }
            }
        }
    } while (retry);

    LOG_INFO << "Authorization for idTag [" << id_tag << "] : " << AuthorizationStatusHelper.toString(status);

    return status;
}

/** @copydoc void IAuthentManager::update(const std::string& id_tag, const ocpp::types::ocpp16::IdTagInfo&) */
void AuthentManager::update(const std::string& id_tag, const ocpp::types::ocpp16::IdTagInfo& tag_info)
{
    // Check if the cache is enabled
    if (m_ocpp_config.authorizationCacheEnabled())
    {
        // Check local authorization list
        bool in_local_list = false;
        if (m_ocpp_config.localAuthListEnabled())
        {
            IdTagInfo unused_tag_info;
            in_local_list = m_local_list.check(id_tag, unused_tag_info);
        }
        if (!in_local_list)
        {
            // Update cache
            m_cache.update(id_tag, tag_info);
        }
    }
}

/** @copydoc ocpp::types::ocpp16::AuthorizationStatus IAuthentManager::iso15118Authorize(const std::string&) */
ocpp::types::ocpp16::AuthorizationStatus AuthentManager::iso15118Authorize(const std::string& token_id)
{
    AuthorizationStatus status = AuthorizationStatus::Invalid;

    // Check if local authorization is enabled
    if (m_ocpp_config.localAuthorizeOffline())
    {
        bool found = false;

        // Check local authorization list
        IdTagInfo tag_info;
        if (m_ocpp_config.localAuthListEnabled())
        {
            found = m_local_list.check(token_id, tag_info);
            if (found)
            {
                status = tag_info.status;
            }
            LOG_DEBUG << "Token [" << token_id << "] found in local list : " << found;
        }

        // Check local cache
        if (!found)
        {
            if (m_ocpp_config.authorizationCacheEnabled())
            {
                found = m_cache.check(token_id, tag_info);
                if (found)
                {
                    status = tag_info.status;
                }
                LOG_DEBUG << "Token [" << token_id << "] found in cache : " << found;
            }
        }

        // Check if unknown ids are allowed when offline
        if (!found)
        {
            if (m_ocpp_config.allowOfflineTxForUnknownId())
            {
                status = AuthorizationStatus::Accepted;

                LOG_DEBUG << "Token [" << token_id << "] unknown but accepted";
            }
        }
    }

    LOG_INFO << "Authorization for token [" << token_id << "] : " << AuthorizationStatusHelper.toString(status);

    return status;
}

/** @copydoc void IAuthentManager::iso15118Update(const std::string&, const ocpp::types::ocpp16::IdTokenInfoType&) */
void AuthentManager::iso15118Update(const std::string& token_id, const ocpp::types::ocpp16::IdTokenInfoType& token_info)
{
    // Check if the cache is enabled
    if (m_ocpp_config.authorizationCacheEnabled())
    {
        // Check local authorization list
        bool in_local_list = false;
        if (m_ocpp_config.localAuthListEnabled())
        {
            IdTagInfo unused_tag_info;
            in_local_list = m_local_list.check(token_id, unused_tag_info);
        }
        if (!in_local_list)
        {
            // Update cache
            IdTagInfo tag_info;
            tag_info.status     = token_info.status;
            tag_info.expiryDate = token_info.cacheExpiryDateTime;
            m_cache.update(token_id, tag_info);
        }
    }
}

} // namespace chargepoint
} // namespace ocpp
