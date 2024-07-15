/*
MIT License

Copyright (c) 2020 Cedric Jimenez

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef CHARGEPOINTDATABASE_H
#define CHARGEPOINTDATABASE_H

#include "Database.h"

#include <memory>

/** @brief Handle persistency of known charge points */
class ChargePointDatabase
{
  public:
    /** @brief Constructor */
    ChargePointDatabase(ocpp::database::Database& database);

    /** @brief Destructor */
    virtual ~ChargePointDatabase();

    /** @brief Add a charge point in database */
    bool addChargePoint(const std::string& identifier,
                        const std::string& serial_number,
                        const std::string& vendor,
                        const std::string& model,
                        unsigned int       security_profile,
                        const std::string& authent_key);

    /** @brief Look for a charge point in database */
    bool getChargePoint(const std::string& identifier,
                        std::string&       serial_number,
                        unsigned int&      security_profile,
                        std::string&       authent_key);

    /** @brief Set the profile of a charge point in database */
    bool setChargePointProfile(const std::string& identifier, unsigned int security_profile);

  private:
    /** @brief Database */
    ocpp::database::Database& m_database;

    /** @brief Query to look for a charge point */
    std::unique_ptr<ocpp::database::Database::Query> m_find_query;
    /** @brief Query to insert a charge point */
    std::unique_ptr<ocpp::database::Database::Query> m_insert_query;
    /** @brief Query to update the security profile of a charge point */
    std::unique_ptr<ocpp::database::Database::Query> m_update_profile_query;

    /** @brief Initialize the database table */
    void initDatabaseTable();
};

#endif // CHARGEPOINTDATABASE_H
