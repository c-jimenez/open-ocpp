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

#include "Database.h"
#include "sqlite3.h"

#include <cstring>

namespace ocpp
{
namespace database
{

/** @brief Constructor */
Database::Database() : m_db(nullptr) { }
/** @brief Destructor */
Database::~Database()
{
    close();
}

/** @brief Open a database */
bool Database::open(const std::string& database_path)
{
    bool ret = false;

    // Check if the database is opened
    if (!m_db)
    {
        // Open database
        if (sqlite3_open_v2(database_path.c_str(), &m_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr) ==
            SQLITE_OK)
        {
            ret = true;
        }
    }

    return ret;
}

/** @brief Close the database */
bool Database::close()
{
    bool ret = false;

    // Check if the database is opened
    if (m_db)
    {
        // Close and free resources
        sqlite3_close_v2(m_db);
        m_db = nullptr;
        ret  = true;
    }

    return ret;
}

/** @brief Create a new query to be executed on the database */
std::unique_ptr<Database::Query> Database::query(const std::string& sql)
{
    std::unique_ptr<Database::Query> query(nullptr);

    // Check if the database is opened
    if (m_db)
    {
        // Create new statement
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), static_cast<int>(sql.size()) + 1, &stmt, nullptr) == SQLITE_OK)
        {
            // Allocate new query
            query = std::make_unique<Database::Query>(*this, stmt);
        }
        else
        {
            // Free resources
            if (stmt)
            {
                sqlite3_finalize(stmt);
            }
        }
    }

    return query;
}

/** @brief Get the string explaining the last error */
std::string Database::lastError() const
{
    std::string error;
    if (m_db)
    {
        error = sqlite3_errmsg(m_db);
    }
    return error;
}

// Database::Query

/** @brief Constructor */
Database::Query::Query(Database& database, sqlite3_stmt* stmt) : m_database(database), m_stmt(stmt), m_has_rows(false) { }
/** @brief Destructor */
Database::Query::~Query()
{
    sqlite3_finalize(m_stmt);
}

/** @brief Reset the query so it can be reused for another execution */
void Database::Query::reset()
{
    // Reset query
    sqlite3_reset(m_stmt);
}

/** @brief Bind a NULL value to a query parameter */
bool Database::Query::bind(int number)
{
    bool ret = false;

    int result = sqlite3_bind_null(m_stmt, number + 1);
    if (result == SQLITE_OK)
    {
        ret = true;
    }

    return ret;
}

/** @brief Bind a blob value to a query parameter */
bool Database::Query::bind(int number, const std::vector<uint8_t>& value)
{
    bool ret = false;

    int result = sqlite3_bind_blob(m_stmt, number + 1, &value[0], static_cast<int>(value.size()), nullptr);
    if (result == SQLITE_OK)
    {
        ret = true;
    }

    return ret;
}

/** @brief Bind a boolean value to a query parameter */
bool Database::Query::bind(int number, bool value)
{
    return bind(number, static_cast<int32_t>(value));
}

/** @brief Bind a floating point value to a query parameter */
bool Database::Query::bind(int number, double value)
{
    bool ret = false;

    int result = sqlite3_bind_double(m_stmt, number + 1, value);
    if (result == SQLITE_OK)
    {
        ret = true;
    }

    return ret;
}

/** @brief Bind a 32bits signed integer value to a query parameter */
bool Database::Query::bind(int number, int32_t value)
{
    bool ret = false;

    int result = sqlite3_bind_int(m_stmt, number + 1, value);
    if (result == SQLITE_OK)
    {
        ret = true;
    }

    return ret;
}

/** @brief Bind a 32bits unsigned integer value to a query parameter */
bool Database::Query::bind(int number, uint32_t value)
{
    return bind(number, static_cast<int32_t>(value));
}

/** @brief Bind a 64bits signed integer value to a query parameter */
bool Database::Query::bind(int number, int64_t value)
{
    bool ret = false;

    int result = sqlite3_bind_int64(m_stmt, number + 1, value);
    if (result == SQLITE_OK)
    {
        ret = true;
    }

    return ret;
}

/** @brief Bind a 64bits unsigned integer value to a query parameter */
bool Database::Query::bind(int number, uint64_t value)
{
    return bind(number, static_cast<int64_t>(value));
}

/** @brief Bind a string value to a query parameter */
bool Database::Query::bind(int number, const std::string& value)
{
    bool ret = false;

    int result = sqlite3_bind_text(m_stmt, number + 1, value.c_str(), -1, nullptr);
    if (result == SQLITE_OK)
    {
        ret = true;
    }

    return ret;
}

/** @brief Bind a string value to a query parameter */
bool Database::Query::bind(int number, const char* value)
{
    bool ret = false;

    int result = sqlite3_bind_text(m_stmt, number + 1, value, -1, nullptr);
    if (result == SQLITE_OK)
    {
        ret = true;
    }

    return ret;
}

/** @brief Execute the query */
bool Database::Query::exec()
{
    bool ret   = false;
    m_has_rows = false;

    // Execute query
    int result = sqlite3_step(m_stmt);
    if (result == SQLITE_DONE)
    {
        ret = true;
    }
    else
    {
        if (result == SQLITE_ROW)
        {
            m_has_rows = true;
            ret        = true;
        }
    }

    return ret;
}
/** @brief Indicate if the query result has rows to extract data */
bool Database::Query::hasRows() const
{
    return m_has_rows;
}

/** @brief Get to the next value of the query result */
bool Database::Query::next()
{
    bool ret = false;

    // Execute next step
    int result = sqlite3_step(m_stmt);
    if (result == SQLITE_ROW)
    {
        ret = true;
    }

    return ret;
}

/** @brief Get the string explaining the last error */
std::string Database::Query::lastError() const
{
    return m_database.lastError();
}

/** @brief Indicate if a value from a query result is NULL */
bool Database::Query::isNull(int column) const
{
    bool ret = (sqlite3_column_type(m_stmt, column) == SQLITE_NULL);
    return ret;
}

/** @brief Get a blob value from a query result */
std::vector<uint8_t> Database::Query::getBlob(int column) const
{
    size_t      size = 0;
    const void* blob = sqlite3_column_blob(m_stmt, column);
    if (blob)
    {
        size = static_cast<size_t>(sqlite3_column_bytes(m_stmt, column));
    }

    std::vector<uint8_t> value(size);
    memcpy(&value[0], blob, size);
    return value;
}

/** @brief Get a boolean value from a query result */
bool Database::Query::getBool(int column) const
{
    return static_cast<bool>(getInt32(column));
}

/** @brief Get a floating point value from a query result */
double Database::Query::getFloat(int column) const
{
    return sqlite3_column_double(m_stmt, column);
}

/** @brief Get a 32bits signed integer value from a query result */
int32_t Database::Query::getInt32(int column) const
{
    return sqlite3_column_int(m_stmt, column);
}

/** @brief Get a 32bits unsigned integer value from a query result */
uint32_t Database::Query::getUInt32(int column) const
{
    return static_cast<uint32_t>(getInt32(column));
}

/** @brief Get a 64bits signed integer value from a query result */
int64_t Database::Query::getInt64(int column) const
{
    return sqlite3_column_int64(m_stmt, column);
}

/** @brief Get a 64bits unsigned integer value from a query result */
uint64_t Database::Query::getUInt64(int column) const
{
    return static_cast<uint64_t>(getInt64(column));
}

/** @brief Get a string value from a query result */
std::string Database::Query::getString(int column) const
{
    const char* text = reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, column));
    std::string value(text);
    return value;
}

} // namespace database
} // namespace ocpp
