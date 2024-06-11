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

#include "LogDatabase.h"

#include <sstream>

using namespace ocpp::database;

namespace ocpp
{
namespace log
{

/** @brief Constructor */
LogDatabase::LogDatabase(ocpp::database::Database& database, const std::string& table_name, unsigned int max_entries)
    : m_database(database), m_insert_query()
{
    initDatabaseTable(table_name, max_entries);
}

/** @brief Destructor */
LogDatabase::~LogDatabase() { }

/** @brief Add a log entry */
void LogDatabase::log(std::time_t timestamp, unsigned int level, const std::string& file, const std::string& message)
{
    if (m_insert_query)
    {
        m_insert_query->bind(0, static_cast<int64_t>(timestamp));
        m_insert_query->bind(1, level);
        m_insert_query->bind(2, file);
        m_insert_query->bind(3, message);
        m_insert_query->exec();
        m_insert_query->reset();
    }
}

/** @brief Initialize the database table */
void LogDatabase::initDatabaseTable(const std::string& table_name, unsigned int max_entries)
{
    // Create database
    std::stringstream create_query;
    create_query << "CREATE TABLE IF NOT EXISTS " << table_name
                 << " ("
                    "[id]	INTEGER,"
                    "[timestamp] BIGINT,"
                    "[level] INT UNSIGNED,"
                    "[file] VARCHAR(64),"
                    "[message] VARCHAR(1024),"
                    "PRIMARY KEY([id] AUTOINCREMENT));";
    auto query = m_database.query(create_query.str());
    if (query.get())
    {
        query->exec();
    }
    std::stringstream trigger_query;
    trigger_query << "CREATE TRIGGER delete_oldest_" << table_name << " AFTER INSERT ON " << table_name << " WHEN ((SELECT count() FROM "
                  << table_name << ") > " << max_entries << ") BEGIN DELETE FROM " << table_name << " WHERE ROWID IN (SELECT ROWID FROM "
                  << table_name << " LIMIT 1);END;";
    query = m_database.query(trigger_query.str());
    if (query.get())
    {
        query->exec();
    }

    // Create parametrized queries
    std::stringstream insert_query;
    insert_query << "INSERT INTO " << table_name << " VALUES (NULL, ?, ?, ?, ?);";
    m_insert_query = m_database.query(insert_query.str());
}

} // namespace log
} // namespace ocpp
