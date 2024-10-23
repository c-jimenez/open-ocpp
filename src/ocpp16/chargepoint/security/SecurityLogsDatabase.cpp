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

#include "SecurityLogsDatabase.h"
#include "IChargePointConfig.h"
#include "Logger.h"

#include <fstream>
#include <sstream>

using namespace ocpp::database;
using namespace ocpp::types;

namespace ocpp
{
namespace chargepoint
{

/** @brief Constructor */
SecurityLogsDatabase::SecurityLogsDatabase(const ocpp::config::IChargePointConfig& stack_config, ocpp::database::Database& database)
    : m_stack_config(stack_config), m_database(database), m_clear_query(), m_insert_query()
{
}

/** @brief Destructor */
SecurityLogsDatabase::~SecurityLogsDatabase() { }

/** @brief Log a security event */
bool SecurityLogsDatabase::log(const std::string& type, const std::string& message, bool critical, const ocpp::types::DateTime& timestamp)
{
    bool ret = false;

    if (m_insert_query)
    {
        m_insert_query->bind(0, static_cast<int64_t>(timestamp));
        m_insert_query->bind(1, type);
        m_insert_query->bind(2, message);
        m_insert_query->bind(3, critical);
        ret = m_insert_query->exec();
        if (!ret)
        {
            LOG_ERROR << "Unable to store security log : " << m_insert_query->lastError();
        }
        m_insert_query->reset();
    }

    return ret;
}

/** @brief Clear all the security events */
bool SecurityLogsDatabase::clear()
{
    bool ret = false;

    if (m_clear_query)
    {
        ret = m_clear_query->exec();
        if (!ret)
        {
            LOG_ERROR << "Unable to clear security logs : " << m_clear_query->lastError();
        }
        m_clear_query->reset();
    }

    return ret;
}

/** @brief Export security events into a file */
bool SecurityLogsDatabase::exportSecurityEvents(const std::string&                                  filepath,
                                                const ocpp::types::Optional<ocpp::types::DateTime>& start_time,
                                                const ocpp::types::Optional<ocpp::types::DateTime>& stop_time)
{
    bool ret = false;

    // Create export file
    std::fstream export_file(filepath, std::fstream::out);
    if (export_file.is_open())
    {
        // Create export request
        std::stringstream select_query;
        select_query << "SELECT * FROM SecurityLogs WHERE ";
        if (start_time.isSet() && stop_time.isSet())
        {
            select_query << "timestamp >= " << start_time.value().timestamp() << " AND ";
            select_query << "timestamp <= " << stop_time.value().timestamp() << ";";
        }
        else if (start_time.isSet())
        {
            select_query << "timestamp >= " << start_time.value().timestamp() << ";";
        }
        else if (stop_time.isSet())
        {
            select_query << "timestamp <= " << stop_time.value().timestamp() << ";";
        }
        else
        {
            select_query << "TRUE;";
        }
        auto query = m_database.query(select_query.str());
        if (query && query->exec())
        {
            // Header
            export_file << "Timestamp,Type,Message" << std::endl;

            // Logs
            if (query->hasRows())
            {
                do
                {
                    time_t      timestamp = query->getInt64(1);
                    std::string type      = query->getString(2);
                    std::string message   = query->getString(3);
                    export_file << timestamp << "," << type << "," << message << std::endl;
                } while (query->next());
            }

            ret = true;
        }
        else
        {
            LOG_ERROR << "Unable to retrieve logs : " << (query ? query->lastError() : m_database.lastError());
        }
    }
    else
    {
        LOG_ERROR << "Unable to create export file : " << filepath;
    }

    return ret;
}

/** @brief Initialize the database table */
void SecurityLogsDatabase::initDatabaseTable()
{
    if (m_stack_config.securityLogMaxEntriesCount() > 0)
    {
        // Create database
        auto query = m_database.query("CREATE TABLE IF NOT EXISTS SecurityLogs ("
                                      "[id]	INTEGER,"
                                      "[timestamp] BIGINT,"
                                      "[type] VARCHAR(50),"
                                      "[message] VARCHAR(255),"
                                      "[critical] BOOLEAN,"
                                      "PRIMARY KEY([id] AUTOINCREMENT));");
        if (query.get())
        {
            if (!query->exec())
            {
                LOG_ERROR << "Could not create security logs table  : " << query->lastError();
            }
        }
        std::stringstream trigger_query;
        trigger_query << "CREATE TRIGGER delete_oldest_SecurityLogs AFTER INSERT ON SecurityLogs WHEN "
                         " ((SELECT count() FROM SecurityLogs) > ";
        trigger_query << m_stack_config.securityLogMaxEntriesCount();
        trigger_query << ") BEGIN DELETE FROM SecurityLogs WHERE ROWID IN "
                         "(SELECT ROWID FROM SecurityLogs LIMIT 1);END;";
        query = m_database.query(trigger_query.str());
        if (query)
        {
            if (!query->exec())
            {
                LOG_ERROR << "Could not create security logs trigger  : " << query->lastError();
            }
        }

        // Create parametrized queries
        m_clear_query  = m_database.query("DELETE FROM SecurityLogs WHERE TRUE;");
        m_insert_query = m_database.query("INSERT INTO SecurityLogs VALUES (NULL, ?, ?, ?, ?);");
    }
    else
    {
        // Disable logging
        m_clear_query.reset();
        m_insert_query.reset();
    }
}

} // namespace chargepoint
} // namespace ocpp
