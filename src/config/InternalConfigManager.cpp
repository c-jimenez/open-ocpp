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

#include "InternalConfigManager.h"
#include "Logger.h"

namespace ocpp
{
namespace config
{

/** @brief Constructor */
InternalConfigManager::InternalConfigManager(ocpp::database::Database& database) : m_database(database) { }

/** @brief Destructor */
InternalConfigManager::~InternalConfigManager() { }

/** @copydoc bool IInternalConfigManager::keyExist(const std::string&) */
bool InternalConfigManager::keyExist(const std::string& key)
{
    bool ret = false;

    if (m_find_query)
    {
        m_find_query->bind(0, key);
        ret = m_find_query->exec();
        if (ret)
        {
            ret = m_find_query->hasRows();
        }
        else
        {
            LOG_ERROR << "Could not search key [" << key << "] : " << m_insert_query->lastError();
        }
        m_find_query->reset();
    }

    return ret;
}

/** @copydoc bool IInternalConfigManager::createKey(const std::string&, const std::string&) */
bool InternalConfigManager::createKey(const std::string& key, const std::string& value)
{
    bool ret = true;

    if (m_insert_query)
    {
        m_insert_query->bind(0, key);
        m_insert_query->bind(1, value);
        ret = m_insert_query->exec();
        if (!ret)
        {
            LOG_ERROR << "Could not insert key [" << key << "] : " << m_insert_query->lastError();
        }
        m_insert_query->reset();
    }

    return ret;
}

/** @copydoc bool IInternalConfigManager::setKey(const std::string&, const std::string&) */
bool InternalConfigManager::setKey(const std::string& key, const std::string& value)
{
    bool ret = true;

    if (m_update_query)
    {
        m_update_query->bind(1, key);
        m_update_query->bind(0, value);
        ret = m_update_query->exec();
        if (!ret)
        {
            LOG_ERROR << "Could not update key [" << key << "] : " << m_insert_query->lastError();
        }
        m_update_query->reset();
    }

    return ret;
}

/** @copydoc bool IInternalConfigManager::getKey(const std::string&, const std::string&) */
bool InternalConfigManager::getKey(const std::string& key, std::string& value)
{
    bool ret = true;

    if (m_find_query)
    {
        m_find_query->bind(0, key);
        ret = m_find_query->exec();
        if (ret)
        {
            if (m_find_query->hasRows())
            {
                value = m_find_query->getString(2u);
                ret   = true;
            }
            else
            {
                LOG_WARNING << "Key [" << key << "] does not exist: " << m_insert_query->lastError();
            }
        }
        else
        {
            LOG_ERROR << "Could not search key [" << key << "] : " << m_insert_query->lastError();
        }
        m_find_query->reset();
    }

    return ret;
}

/** @brief Initialize the database table */
void InternalConfigManager::initDatabaseTable()
{
    // Create database
    auto query = m_database.query("CREATE TABLE IF NOT EXISTS InternalConfig ("
                                  "[id]	INTEGER,"
                                  "[key]	VARCHAR(20),"
                                  "[value]	VARCHAR(20),"
                                  "PRIMARY KEY([id] AUTOINCREMENT));");
    if (query)
    {
        if (!query->exec())
        {
            LOG_ERROR << "Could not create internal configuration table : " << query->lastError();
        }
    }

    // Create parametrized queries
    m_find_query   = m_database.query("SELECT * FROM InternalConfig WHERE key=?;");
    m_insert_query = m_database.query("INSERT INTO InternalConfig VALUES (NULL, ?, ?);");
    m_update_query = m_database.query("UPDATE InternalConfig SET [value]=? WHERE key=?;");

    // List all configuration keys
    query = m_database.query("SELECT * FROM InternalConfig WHERE TRUE;");
    if (query)
    {
        if (query->exec() && query->hasRows())
        {
            do
            {
                LOG_DEBUG << "Key : " << query->getString(1u) << " = " << query->getString(2u);
            } while (query->next());
        }
    }
}

} // namespace config
} // namespace ocpp
