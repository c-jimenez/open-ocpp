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

#ifndef OPENOCPP_LOGDATABASE_H
#define OPENOCPP_LOGDATABASE_H

#include "Database.h"

#include <chrono>

namespace ocpp
{
namespace log
{

/** @brief Handle persistency of logs */
class LogDatabase
{
  public:
    /** @brief Constructor */
    LogDatabase(ocpp::database::Database& database, const std::string& table_name, unsigned int max_entries);

    /** @brief Destructor */
    virtual ~LogDatabase();

    /**
     * @brief Add a log entry
     * @param timestamp Timestamp in UNIX format of the entry
     * @param level Log level
     * @param file File which generated the log
     * @param message log message
     */
    void log(std::time_t timestamp, unsigned int level, const std::string& file, const std::string& message);

  private:
    /** @brief Database to store the logs */
    ocpp::database::Database& m_database;

    /** @brief Query to insert a log */
    std::unique_ptr<ocpp::database::Database::Query> m_insert_query;

    /** @brief Initialize the database table */
    void initDatabaseTable(const std::string& table_name, unsigned int max_entries);
};

} // namespace log
} // namespace ocpp

#endif // OPENOCPP_LOGDATABASE_H
