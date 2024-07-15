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

#include "AuthentCache.h"
#include "IChargePointConfig.h"
#include "IMessageDispatcher.h"
#include "IOcppConfig.h"
#include "Logger.h"

#include <sstream>

using namespace ocpp::database;
using namespace ocpp::messages;
using namespace ocpp::messages::ocpp16;
using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace chargepoint
{

/** @brief Constructor */
AuthentCache::AuthentCache(const ocpp::config::IChargePointConfig&         stack_config,
                           ocpp::config::IOcppConfig&                      ocpp_config,
                           ocpp::database::Database&                       database,
                           const ocpp::messages::GenericMessagesConverter& messages_converter,
                           ocpp::messages::IMessageDispatcher&             msg_dispatcher)
    : GenericMessageHandler<ClearCacheReq, ClearCacheConf>(CLEAR_CACHE_ACTION, messages_converter),
      m_stack_config(stack_config),
      m_ocpp_config(ocpp_config),
      m_database(database),
      m_find_query(),
      m_delete_query(),
      m_insert_query(),
      m_update_query()
{
    initDatabaseTable();
    msg_dispatcher.registerHandler(CLEAR_CACHE_ACTION, *this);
}

/** @brief Destructor */
AuthentCache::~AuthentCache() { }

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool AuthentCache::handleMessage(const ocpp::messages::ocpp16::ClearCacheReq& request,
                                 ocpp::messages::ocpp16::ClearCacheConf&      response,
                                 std::string&                                 error_code,
                                 std::string&                                 error_message)
{
    (void)request;
    (void)error_code;
    (void)error_message;

    LOG_INFO << "Clear cache requested";

    if (m_ocpp_config.authorizationCacheEnabled())
    {
        clear();
        response.status = ClearCacheStatus::Accepted;
    }
    else
    {
        response.status = ClearCacheStatus::Rejected;
    }

    LOG_INFO << "Clear cache status : " << ClearCacheStatusHelper.toString(response.status);

    return true;
}

/** @brief Look for a tag id in the cache */
bool AuthentCache::check(const std::string& id_tag, ocpp::types::ocpp16::IdTagInfo& tag_info)
{
    bool ret = false;

    if (m_find_query)
    {
        // Execute query
        m_find_query->bind(0, id_tag);
        if (m_find_query->exec())
        {
            // Check if a match has been found
            ret = m_find_query->hasRows();
            if (ret)
            {
                // Extract data
                bool        expiry_valid = !m_find_query->isNull(3);
                std::time_t expiry       = m_find_query->getInt64(3);
                if (expiry_valid)
                {
                    tag_info.expiryDate = DateTime(expiry);
                }
                else
                {
                    tag_info.expiryDate.clear();
                }
                std::string parent = m_find_query->getString(2);
                if (!parent.empty())
                {
                    tag_info.parentIdTag.value().assign(parent);
                }
                else
                {
                    tag_info.parentIdTag.clear();
                }
                tag_info.status = static_cast<AuthorizationStatus>(m_find_query->getInt32(4));

                // Check expiry date
                std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                if (expiry_valid && (expiry < now))
                {
                    // Entry is no more valid, delete entry
                    if (m_delete_query)
                    {
                        m_delete_query->bind(0, id_tag);
                        m_delete_query->exec();
                        m_delete_query->reset();
                    }
                    ret = false;
                }
            }
        }
        m_find_query->reset();
    }
    return ret;
}

/** @brief Update a tag id entry in the cache */
void AuthentCache::update(const std::string& id_tag, const ocpp::types::ocpp16::IdTagInfo& tag_info)
{
    // Look for the entry
    if (m_find_query)
    {
        // Execute query
        m_find_query->bind(0, id_tag);
        if (m_find_query->exec())
        {
            // Convert status
            AuthorizationStatus status = tag_info.status;
            if (status == AuthorizationStatus::ConcurrentTx)
            {
                status = AuthorizationStatus::Accepted;
            }
            if (m_find_query->hasRows())
            {
                // Update entry
                if (m_update_query)
                {
                    int entry = m_find_query->getInt32(0);
                    if (tag_info.parentIdTag.isSet())
                    {
                        m_update_query->bind(0, tag_info.parentIdTag.value());
                    }
                    else
                    {
                        m_update_query->bind(0, "");
                    }
                    if (tag_info.expiryDate.isSet())
                    {
                        m_update_query->bind(1, static_cast<int64_t>(tag_info.expiryDate.value().timestamp()));
                    }
                    else
                    {
                        m_update_query->bind(1);
                    }
                    m_update_query->bind(2, static_cast<int>(status));
                    m_update_query->bind(3, entry);
                    if (!m_update_query->exec())
                    {
                        LOG_ERROR << "Could not update idTag [" << id_tag << "]";
                    }
                    else
                    {
                        LOG_DEBUG << "IdTag [" << id_tag << "] updated";
                    }
                    m_update_query->reset();
                }
            }
            else
            {
                if (m_insert_query)
                {
                    m_insert_query->bind(0, id_tag);
                    if (tag_info.parentIdTag.isSet())
                    {
                        m_insert_query->bind(1, tag_info.parentIdTag.value());
                    }
                    else
                    {
                        m_insert_query->bind(1, "");
                    }
                    if (tag_info.expiryDate.isSet())
                    {
                        m_insert_query->bind(2, static_cast<int64_t>(tag_info.expiryDate.value().timestamp()));
                    }
                    else
                    {
                        m_insert_query->bind(2);
                    }
                    m_insert_query->bind(3, static_cast<int>(status));
                    if (!m_insert_query->exec())
                    {
                        LOG_ERROR << "Could not insert idTag [" << id_tag << "]";
                    }
                    else
                    {
                        LOG_DEBUG << "IdTag [" << id_tag << "] inserted";
                    }
                    m_insert_query->reset();
                }
            }
        }
        m_find_query->reset();
    }
}

/** @brief Initialize the database table */
void AuthentCache::initDatabaseTable()
{
    // Create database
    auto query = m_database.query("CREATE TABLE IF NOT EXISTS AuthentCache ("
                                  "[id]	INTEGER,"
                                  "[tag]	VARCHAR(20),"
                                  "[parent]	VARCHAR(20),"
                                  "[expiry]	INTEGER,"
                                  "[status]	INTEGER,"
                                  "PRIMARY KEY([id] AUTOINCREMENT));");
    if (query)
    {
        if (!query->exec())
        {
            LOG_ERROR << "Could not create authent cache table : " << query->lastError();
        }
    }
    std::stringstream trigger_query;
    trigger_query << "CREATE TRIGGER delete_oldest_AuthentCache AFTER INSERT ON AuthentCache WHEN "
                     " ((SELECT count() FROM AuthentCache) > ";
    trigger_query << m_stack_config.authentCacheMaxEntriesCount();
    trigger_query << ") BEGIN DELETE FROM AuthentCache WHERE ROWID IN "
                     "(SELECT ROWID FROM AuthentCache LIMIT 1);END;";
    query = m_database.query(trigger_query.str());
    if (query)
    {
        if (!query->exec())
        {
            LOG_ERROR << "Could not create authent cache trigger  : " << query->lastError();
        }
    }

    // Create parametrized queries
    m_find_query   = m_database.query("SELECT * FROM AuthentCache WHERE tag=?;");
    m_delete_query = m_database.query("DELETE FROM AuthentCache WHERE tag=?;");
    m_insert_query = m_database.query("INSERT INTO AuthentCache VALUES (NULL, ?, ?, ?, ?);");
    m_update_query = m_database.query("UPDATE AuthentCache SET [parent]=?, [expiry]=?, [status]=? WHERE id=?;");
}

/** @brief Clear the cache */
void AuthentCache::clear()
{
    auto query = m_database.query("DELETE FROM AuthentCache WHERE TRUE;");
    if (query.get())
    {
        query->exec();
    }
}

} // namespace chargepoint
} // namespace ocpp
