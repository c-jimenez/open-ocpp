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

#ifndef OPENOCPP_DATABASE_H
#define OPENOCPP_DATABASE_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// Forward declarations
struct sqlite3;
struct sqlite3_stmt;

namespace ocpp
{
namespace database
{

/** @brief Basic database implementation */
class Database
{
  public:
    // Forward declaration
    class Query;

    /** @brief Constructor */
    Database();
    /** @brief Destructor */
    virtual ~Database();

    /**
     * @brief Open a database
     * @param database_path Path to the database file
     * @return true if the database exists, false otherwise
     */
    bool open(const std::string& database_path);

    /**
     * @brief Close the database
     * @return true if the database exists, false otherwise
     */
    bool close();

    /**
     * @brief Create a new query to be executed on the database
     * @param sql SQL query to execute
     * @return Created query object if the database is opened and the query is valid, nullptr otherwise
     */
    std::unique_ptr<Query> query(const std::string& sql);

    /**
     * @brief Get the string explaining the last error
     * @return String explaining the last error
     */
    std::string lastError() const;

    /** @brief Represent a query to be executed on the database */
    class Query
    {
      public:
        /** @brief Constructor */
        Query(Database& database, sqlite3_stmt* stmt);
        /** @brief Destructor */
        virtual ~Query();

        /** @brief Reset the query so it can be reused for another execution */
        void reset();

        /**
         * @brief Bind a NULL value to a query parameter
         * @param number Number of the parameter in the query
         * @return true if the binding has been done, false otherwise
         */
        bool bind(int number);

        /**
         * @brief Bind a blob value to a query parameter
         * @param number Number of the parameter in the query
         * @param value Value to bind
         * @return true if the binding has been done, false otherwise
         */
        bool bind(int number, const std::vector<uint8_t>& value);

        /**
         * @brief Bind a boolean value to a query parameter
         * @param number Number of the parameter in the query
         * @param value Value to bind
         * @return true if the binding has been done, false otherwise
         */
        bool bind(int number, bool value);

        /**
         * @brief Bind a floating point value to a query parameter
         * @param number Number of the parameter in the query
         * @param value Value to bind
         * @return true if the binding has been done, false otherwise
         */
        bool bind(int number, double value);

        /**
         * @brief Bind a 32bits signed integer value to a query parameter
         * @param number Number of the parameter in the query
         * @param value Value to bind
         * @return true if the binding has been done, false otherwise
         */
        bool bind(int number, int32_t value);

        /**
         * @brief Bind a 32bits unsigned integer value to a query parameter
         * @param number Number of the parameter in the query
         * @param value Value to bind
         * @return true if the binding has been done, false otherwise
         */
        bool bind(int number, uint32_t value);

        /**
         * @brief Bind a 64bits signed integer value to a query parameter
         * @param number Number of the parameter in the query
         * @param value Value to bind
         * @return true if the binding has been done, false otherwise
         */
        bool bind(int number, int64_t value);

        /**
         * @brief Bind a 64bits unsigned integer value to a query parameter
         * @param number Number of the parameter in the query
         * @param value Value to bind
         * @return true if the binding has been done, false otherwise
         */
        bool bind(int number, uint64_t value);

        /**
         * @brief Bind a string value to a query parameter
         * @param number Number of the parameter in the query
         * @param value Value to bind
         * @return true if the binding has been done, false otherwise
         */
        bool bind(int number, const std::string& value);

        /**
         * @brief Bind a string value to a query parameter
         * @param number Number of the parameter in the query
         * @param value Value to bind
         * @return true if the binding has been done, false otherwise
         */
        bool bind(int number, const char* value);

        /**
         * @brief Execute the query
         * @return true if the query was executed without errors, false otherwise
         */
        bool exec();

        /**
         * @brief Indicate if the query result has rows to extract data
         * @return true if the query result has rows, false otherwise
         */
        bool hasRows() const;

        /**
         * @brief Get to the next value of the query result
         * @return true if the next value exists, false otherwise
         */
        bool next();

        /**
         * @brief Get the string explaining the last error
         * @return String explaining the last error
         */
        std::string lastError() const;

        /**
         * @brief Indicate if a value from a query result is NULL
         * @param column Column number of the value in the query result
         * @return true if the value is NULL, false otherwise
         */
        bool isNull(int column) const;

        /**
         * @brief Get a blob value from a query result
         * @param column Column number of the value in the query result
         * @return Pointer to the blob value
         */
        std::vector<uint8_t> getBlob(int column) const;

        /**
         * @brief Get a boolean value from a query result
         * @param column Column number of the value in the query result
         * @return Boolean value
         */
        bool getBool(int column) const;

        /**
         * @brief Get a floating point value from a query result
         * @param column Column number of the value in the query result
         * @return Floating point value
         */
        double getFloat(int column) const;

        /**
         * @brief Get a 32bits signed integer value from a query result
         * @param column Column number of the value in the query result
         * @return 32bits signed integer value
         */
        int32_t getInt32(int column) const;

        /**
         * @brief Get a 32bits unsigned integer value from a query result
         * @param column Column number of the value in the query result
         * @return 32bits unsigned integer value
         */
        uint32_t getUInt32(int column) const;

        /**
         * @brief Get a 64bits signed integer value from a query result
         * @param column Column number of the value in the query result
         * @return 64bits signed integer value
         */
        int64_t getInt64(int column) const;

        /**
         * @brief Get a 64bits unsigned integer value from a query result
         * @param column Column number of the value in the query result
         * @return 64bits unsigned integer value
         */
        uint64_t getUInt64(int column) const;

        /**
         * @brief Get a string value from a query result
         * @param column Column number of the value in the query result
         * @return String value
         */
        std::string getString(int column) const;

      private:
        /** @brief Associated database */
        Database& m_database;
        /** @brief Statement handle */
        sqlite3_stmt* m_stmt;
        /** @brief Indicate if the query result has rows to extract data */
        bool m_has_rows;
    };

  private:
    /** @brief Database handle */
    sqlite3* m_db;
};

} // namespace database
} // namespace ocpp

#endif // OPENOCPP_DATABASE_H
