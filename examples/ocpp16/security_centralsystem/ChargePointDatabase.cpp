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

#include "ChargePointDatabase.h"

#include <iostream>
#include <sstream>

using namespace ocpp::database;

/** @brief Constructor */
ChargePointDatabase::ChargePointDatabase(ocpp::database::Database& database)
    : m_database(database), m_find_query(), m_insert_query(), m_update_profile_query()
{
    initDatabaseTable();
}

/** @brief Destructor */
ChargePointDatabase::~ChargePointDatabase() { }

/** @brief Add a charge point in database */
bool ChargePointDatabase::addChargePoint(const std::string& identifier,
                                         const std::string& serial_number,
                                         const std::string& vendor,
                                         const std::string& model,
                                         unsigned int       security_profile,
                                         const std::string& authent_key)
{
    bool ret = false;

    if (m_insert_query)
    {
        m_insert_query->bind(0, identifier);
        m_insert_query->bind(1, serial_number);
        m_insert_query->bind(2, vendor);
        m_insert_query->bind(3, model);
        m_insert_query->bind(4, security_profile);
        m_insert_query->bind(5, authent_key);
        ret = m_insert_query->exec();
        m_insert_query->reset();
    }

    return ret;
}

/** @brief Look for a charge point in database */
bool ChargePointDatabase::getChargePoint(const std::string& identifier,
                                         std::string&       serial_number,
                                         unsigned int&      security_profile,
                                         std::string&       authent_key)
{
    bool ret = false;

    if (m_find_query)
    {
        m_find_query->bind(0, identifier);
        if (m_find_query->exec() && m_find_query->hasRows())
        {
            serial_number    = m_find_query->getString(1);
            security_profile = m_find_query->getUInt32(4);
            authent_key      = m_find_query->getString(5);
            ret              = true;
        }
        m_find_query->reset();
    }

    return ret;
}

/** @brief Set the profile of a charge point in database */
bool ChargePointDatabase::setChargePointProfile(const std::string& identifier, unsigned int security_profile)
{
    bool ret = false;

    if (m_update_profile_query)
    {
        m_update_profile_query->bind(0, security_profile);
        m_update_profile_query->bind(1, identifier);
        ret = m_update_profile_query->exec();
        m_update_profile_query->reset();
    }

    return ret;
}

/** @brief Initialize the database table */
void ChargePointDatabase::initDatabaseTable()
{
    // Create database
    auto query = m_database.query("CREATE TABLE IF NOT EXISTS ChargePoints ("
                                  "[identifier]	    VARCHAR(255),"
                                  "[serial_number]  VARCHAR(64),"
                                  "[vendor]         VARCHAR(64),"
                                  "[model]          VARCHAR(64),"
                                  "[profile]    	INTEGER,"
                                  "[authentkey]     VARCHAR(64),"
                                  "PRIMARY KEY([identifier]));");
    if (query)
    {
        if (!query->exec())
        {
            std::cout << "Could not create charge point table : " << query->lastError() << std::endl;
        }
    }

    // Create parametrized queries
    m_find_query           = m_database.query("SELECT * FROM ChargePoints WHERE identifier=?;");
    m_insert_query         = m_database.query("INSERT INTO ChargePoints VALUES (?, ?, ?, ?, ?, ?);");
    m_update_profile_query = m_database.query("UPDATE ChargePoints SET [profile]=? WHERE identifier=?;");
}
