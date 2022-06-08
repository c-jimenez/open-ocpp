/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Connectors.h"
#include "IOcppConfig.h"
#include "Logger.h"

using namespace ocpp::types;

namespace ocpp
{
namespace chargepoint
{

/** @brief Constructor */
Connectors::Connectors(ocpp::config::IOcppConfig& ocpp_config, ocpp::database::Database& database, ocpp::helpers::ITimerPool& timer_pool)
    : m_ocpp_config(ocpp_config),
      m_database(database),
      m_timer_pool(timer_pool),
      m_connectors(),
      m_find_query(),
      m_insert_query(),
      m_update_query()
{
}

/** @brief Indicate if a connector id is valid */
bool Connectors::isValid(unsigned int id) const
{
    return (id < m_connectors.size());
}

/** @brief Get the number of connectors */
unsigned int Connectors::getCount() const
{
    return static_cast<unsigned int>(m_connectors.size() - 1u);
}

/** @brief Get the connector with the specified id */
Connector* Connectors::getConnector(unsigned int id)
{
    Connector* connector = nullptr;
    if (id < m_connectors.size())
    {
        connector = m_connectors[id];
    }
    return connector;
}

/** @brief Get the connector associated with the charge point */
Connector& Connectors::getChargePointConnector()
{
    return *m_connectors[CONNECTOR_ID_CHARGE_POINT];
}

/** @brief Initialize the database table */
void Connectors::initDatabaseTable()
{
    // Create database
    auto query = m_database.query("CREATE TABLE IF NOT EXISTS Connectors ("
                                  "[id] INT UNSIGNED,"
                                  "[status] INT,"
                                  "[last_notified_status] INT,"
                                  "[transaction_id] INT,"
                                  "[transaction_id_offline] INT,"
                                  "[transaction_start] BIGINT,"
                                  "[transaction_id_tag] VARCHAR(20),"
                                  "[reservation_id] INT,"
                                  "[reservation_id_tag] VARCHAR(20),"
                                  "[reservation_parent_id_tag] VARCHAR(20),"
                                  "[reservation_expiry_date] BIGINT,"
                                  "PRIMARY KEY([id]));");
    if (query)
    {
        if (!query->exec())
        {
            LOG_ERROR << "Could not create connectors table : " << query->lastError();
        }
    }

    // Create parametrized queries
    m_find_query   = m_database.query("SELECT * FROM Connectors WHERE id=?;");
    m_insert_query = m_database.query("INSERT INTO Connectors VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
    m_update_query = m_database.query("UPDATE Connectors SET [status]=?, [last_notified_status]=?, [transaction_id]=?, "
                                      "[transaction_id_offline]=?, [transaction_start]=?, [transaction_id_tag]=?, [reservation_id]=?, "
                                      "[reservation_id_tag]=?, [reservation_parent_id_tag]=?, [reservation_expiry_date]=? WHERE id=?;");

    // Load the connector state
    loadConnectors();
}

/** @brief Save the state of a connector to the database */
bool Connectors::saveConnector(unsigned int id)
{
    bool ret = false;
    if (isValid(id))
    {
        ret = saveConnector(*m_connectors[id]);
    }
    return ret;
}

/** @brief Reset the state of all connectors */
void Connectors::resetConnectors()
{
    // Clear connector array
    for (Connector* connector : m_connectors)
    {
        delete connector;
    }
    m_connectors.clear();

    // Allocate new connectors according to the configuration
    for (unsigned int id = 0; id <= m_ocpp_config.numberOfConnectors(); id++)
    {
        m_connectors.push_back(new Connector(id, m_timer_pool));
    }

    // Reset all database data
    LOG_WARNING << "Reset connector data in database";
    auto query = m_database.query("DELETE FROM Connectors WHERE TRUE;");
    if (query && query->exec())
    {
        // Store default connector data
        for (const Connector* connector : m_connectors)
        {
            createConnector(*connector);
        }
    }
}

/** @brief Load the connectors states from the database */
void Connectors::loadConnectors()
{
    // Check the number of connectors inside the database
    unsigned int count = 0;
    auto         query = m_database.query("SELECT count(id) FROM Connectors WHERE TRUE;");
    if (query && query->exec())
    {
        count = query->getUInt32(0);
    }

    // Clear connector array
    for (Connector* connector : m_connectors)
    {
        delete connector;
    }
    m_connectors.clear();

    // Allocate new connectors according to the configuration
    for (unsigned int id = 0; id <= m_ocpp_config.numberOfConnectors(); id++)
    {
        m_connectors.push_back(new Connector(id, m_timer_pool));
    }

    bool delete_all = false;
    do
    {
        // Check coherency betwwen database and configuration
        unsigned int config_count = m_ocpp_config.numberOfConnectors() + 1u;
        if (count != config_count)
        {
            LOG_WARNING << count << " connectors found in database / " << config_count << " connectors declared in configuration";
            delete_all = true;
        }
        if (delete_all)
        {
            // Reset all database data
            LOG_WARNING << "Reset connector data in database";
            delete_all = false;
            query      = m_database.query("DELETE FROM Connectors WHERE TRUE;");
            if (query && query->exec())
            {
                // Store default connector data
                for (const Connector* connector : m_connectors)
                {
                    createConnector(*connector);
                }
            }
        }
        else
        {
            // Load connector data from the database
            for (Connector* connector : m_connectors)
            {
                if (!loadConnector(*connector))
                {
                    // Failure to load, retry with database cleanup
                    delete_all = true;
                    count      = 0;
                    break;
                }
            }
        }
    } while (delete_all);
}

/** @brief Load the state of a connector from the database */
bool Connectors::loadConnector(Connector& connector)
{
    bool ret = false;

    if (m_find_query)
    {
        m_find_query->reset();
        m_find_query->bind(0, connector.id);
        ret = m_find_query->exec();
        if (ret)
        {
            ret = m_find_query->hasRows();
            if (ret)
            {
                connector.status                    = static_cast<ChargePointStatus>(m_find_query->getInt32(1u));
                connector.last_notified_status      = static_cast<ChargePointStatus>(m_find_query->getInt32(2u));
                connector.transaction_id            = m_find_query->getInt32(3u);
                connector.transaction_id_offline    = m_find_query->getInt32(4u);
                connector.transaction_start         = static_cast<std::time_t>(m_find_query->getInt64(5u));
                connector.transaction_id_tag        = m_find_query->getString(6u);
                connector.reservation_id            = m_find_query->getInt32(7u);
                connector.reservation_id_tag        = m_find_query->getString(8u);
                connector.reservation_parent_id_tag = m_find_query->getString(9u);
                connector.reservation_expiry_date   = static_cast<std::time_t>(m_find_query->getInt64(10u));

                LOG_DEBUG << "Connector " << connector.id << " loaded from database : "
                          << "status = " << ChargePointStatusHelper.toString(connector.status)
                          << " - last_notified_status = " << ChargePointStatusHelper.toString(connector.last_notified_status)
                          << " - transaction_id = " << connector.transaction_id
                          << " - transaction_id_offline = " << connector.transaction_id_offline
                          << " - transaction_start = " << connector.transaction_start.str()
                          << " - transaction_id_tag = " << connector.transaction_id_tag
                          << " - reservation_id = " << connector.reservation_id
                          << " - reservation_id_tag = " << connector.reservation_id_tag
                          << " - reservation_parent_id_tag = " << connector.reservation_parent_id_tag
                          << " - reservation_expiry_date = " << connector.reservation_expiry_date.str();
            }
            else
            {
                LOG_ERROR << "Connector " << connector.id << " not found in database";
            }
        }
        else
        {
            LOG_ERROR << "Could not search for connector " << connector.id << " : " << m_find_query->lastError();
        }
    }

    return ret;
}

/** @brief Save the state of a connector to the database */
bool Connectors::saveConnector(const Connector& connector)
{
    bool ret = false;

    if (m_update_query)
    {
        m_update_query->reset();
        m_update_query->bind(0u, static_cast<int>(connector.status));
        m_update_query->bind(1u, static_cast<int>(connector.last_notified_status));
        m_update_query->bind(2u, connector.transaction_id);
        m_update_query->bind(3u, connector.transaction_id_offline);
        m_update_query->bind(4u, connector.transaction_start);
        m_update_query->bind(5u, connector.transaction_id_tag);
        m_update_query->bind(6u, connector.reservation_id);
        m_update_query->bind(7u, connector.reservation_id_tag);
        m_update_query->bind(8u, connector.reservation_parent_id_tag);
        m_update_query->bind(9u, connector.reservation_expiry_date);
        m_update_query->bind(10u, connector.id);
        ret = m_update_query->exec();
        if (ret)
        {
            LOG_DEBUG << "Connector " << connector.id << " updated in database";
        }
        else
        {
            LOG_ERROR << "Could not update connector " << connector.id << " : " << m_update_query->lastError();
        }
    }

    return ret;
}

/** @brief Create a connector in the database */
bool Connectors::createConnector(const Connector& connector)
{
    bool ret = false;

    if (m_insert_query)
    {
        m_insert_query->reset();
        m_insert_query->bind(0u, connector.id);
        m_insert_query->bind(1u, static_cast<int>(connector.status));
        m_insert_query->bind(2u, static_cast<int>(connector.last_notified_status));
        m_insert_query->bind(3u, connector.transaction_id);
        m_insert_query->bind(4u, connector.transaction_id_offline);
        m_insert_query->bind(5u, connector.transaction_start);
        m_insert_query->bind(6u, connector.transaction_id_tag);
        m_insert_query->bind(7u, connector.reservation_id);
        m_insert_query->bind(8u, connector.reservation_id_tag);
        m_insert_query->bind(9u, connector.reservation_parent_id_tag);
        m_insert_query->bind(10u, connector.reservation_expiry_date);
        ret = m_insert_query->exec();
        if (ret)
        {
            LOG_DEBUG << "Connector " << connector.id << " created in database";
        }
        else
        {
            LOG_ERROR << "Could not create connector " << connector.id << " : " << m_insert_query->lastError();
        }
    }

    return ret;
}

} // namespace chargepoint
} // namespace ocpp
