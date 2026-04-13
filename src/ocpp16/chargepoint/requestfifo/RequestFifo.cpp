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

#include "RequestFifo.h"
#include "Logger.h"

using namespace ocpp::database;
using namespace ocpp::messages;

namespace ocpp
{
namespace chargepoint
{

/** @brief Constructor */
RequestFifo::RequestFifo(ocpp::database::Database& database)
    : m_database(database), m_delete_query(), m_insert_query(), m_mutex(), m_fifo(), m_id(0), m_listener(nullptr)
{
}

/** @brief Destructor */
RequestFifo::~RequestFifo() { }

/** @copydoc void IRequestFifo::push(unsigned int, const std::string&, const rapidjson::Document&) const */
void RequestFifo::push(unsigned int connector_id, const std::string& action, const rapidjson::Document& payload)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_DEBUG << "Transaction related request FIFO : pushing " << action << " request";

    // Serialize request
    rapidjson::StringBuffer                    buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    payload.Accept(writer);
    std::string request = buffer.GetString();

    // Add a new entry to the FIFO
    m_fifo.emplace(m_id, connector_id, action, request);
    if (m_insert_query)
    {
        m_insert_query->bind(0, m_id);
        m_insert_query->bind(1, connector_id);
        m_insert_query->bind(2, action);
        m_insert_query->bind(3, request);
        m_insert_query->exec();
        m_insert_query->reset();
    }

    // Prepare for next entry
    m_id++;

    // Notify
    if (m_listener)
    {
        m_listener->requestQueued();
    }
}

/** @copydoc bool IRequestFifo::front(unsigned int&, std::string&, const rapidjson::Document&) const */
bool RequestFifo::front(unsigned int& connector_id, std::string& action, rapidjson::Document& payload)
{
    bool ret = false;

    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_fifo.empty())
    {
        // Get entry from FIFO
        const std::string& request_str = m_fifo.front().request;
        action                         = m_fifo.front().action;
        connector_id                   = m_fifo.front().connector_id;

        // Deserialize request
        payload.Parse(request_str.c_str());

        ret = true;
    }

    return ret;
}

/** @@copydoc void IRequestFifo::pop() */
void RequestFifo::pop()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_DEBUG << "Transaction related request FIFO : poping " << m_fifo.front().action << " request";

    // Delete entry
    unsigned int id = m_fifo.front().id;
    m_fifo.pop();
    if (m_delete_query)
    {
        m_delete_query->bind(0, id);
        m_delete_query->exec();
        m_delete_query->reset();
    }
}

/** @copydoc size_t IRequestFifo::size() const */
size_t RequestFifo::size() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_fifo.size();
}

/** @brief Initialize the database table */
void RequestFifo::initDatabaseTable()
{
    // Create database
    auto query = m_database.query("CREATE TABLE IF NOT EXISTS RequestFifo ("
                                  "[id]	INT UNSIGNED,"
                                  "[connector_id] INT UNSIGNED,"
                                  "[action]	VARCHAR(64),"
                                  "[request] VARCHAR(1024),"
                                  "PRIMARY KEY([id]));");
    if (query.get())
    {
        query->exec();
    }

    // Create parametrized queries
    m_delete_query = m_database.query("DELETE FROM RequestFifo WHERE id=?;");
    m_insert_query = m_database.query("INSERT INTO RequestFifo VALUES (?, ?, ?, ?);");

    // Load data
    load();
}

/** @brief Load requests from the database */
void RequestFifo::load()
{
    // Clear FIFO
    while (!m_fifo.empty())
    {
        m_fifo.pop();
    }

    // Query all stored requests
    auto query = m_database.query("SELECT * FROM RequestFifo WHERE TRUE ORDER BY id ASC;");
    if (query.get())
    {
        if (query->exec() && query->hasRows())
        {
            do
            {
                // Extract table data
                unsigned int id           = query->getUInt32(0);
                unsigned int connector_id = query->getUInt32(1);
                std::string  action       = query->getString(2);
                std::string  request_str  = query->getString(3);

                // Store request inside the FIFO
                m_fifo.emplace(id, connector_id, action, request_str);
            } while (query->next());

            // Prepare for next entry
            m_id = m_fifo.back().id + 1u;
        }
    }

    LOG_INFO << "Transaction related request FIFO : " << m_fifo.size() << " message(s) pending";
}

} // namespace chargepoint
} // namespace ocpp
