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

#include "Connectors20.h"
#include "Logger.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp20;

namespace ocpp
{
namespace chargepoint
{
namespace ocpp20
{

/** @brief Constructor */
Connectors::Connectors(IDeviceModel& device_model, ocpp::database::Database& database, ocpp::helpers::ITimerPool& timer_pool)
    : m_device_model(device_model),
      m_database(database),
      m_timer_pool(timer_pool),
      m_evses(),
      m_evse_find_query(),
      m_evse_insert_query(),
      m_evse_update_query(),
      m_connector_find_query(),
      m_connector_insert_query(),
      m_connector_update_query()
{
}

/** @brief Destructor */
Connectors::~Connectors()
{
    releaseEvseLayout();
}

/** @copydoc Evse* IConnectors::getEvse(unsigned int) */
Evse* Connectors::getEvse(unsigned int id)
{
    Evse* evse = nullptr;
    if ((id > 0) && (id <= m_evses.size()))
    {
        evse = m_evses[id - 1u];
    }
    return evse;
}

/** @copydoc Evse* IConnectors::getEvse(unsigned int) */
const Evse* Connectors::getEvse(unsigned int id) const
{
    Connectors* non_const_this = const_cast<Connectors*>(this);
    return non_const_this->getEvse(id);
}

/** @copydoc Connector* IConnectors::getConnector(unsigned int, unsigned int) */
Connector* Connectors::getConnector(unsigned int evse_id, unsigned int id)
{
    Connector* connector = nullptr;
    Evse*      evse      = getEvse(evse_id);
    if (evse)
    {
        if ((id > 0) && (id <= evse->connectors.size()))
        {
            connector = evse->connectors[id - 1u];
        }
    }
    return connector;
}

/** @copydoc const std::vector<Connector>* IConnectors::getConnectors(unsigned int) */
const std::vector<Connector*>* Connectors::getConnectors(unsigned int id) const
{
    const std::vector<Connector*>* connectors = nullptr;
    const Evse*                    evse       = getEvse(id);
    if (evse)
    {
        connectors = &evse->connectors;
    }
    return connectors;
}

/** @copydoc bool IConnectors::saveEvse(unsigned int) */
bool Connectors::saveEvse(unsigned int evse_id)
{
    bool ret = false;

    Evse* evse = getEvse(evse_id);
    if (evse)
    {
        saveEvse(*evse);
        ret = true;
    }

    return ret;
}

/** @copydoc bool IConnectors::saveConnector(unsigned int, unsigned int) */
bool Connectors::saveConnector(unsigned int evse_id, unsigned int id)
{
    bool ret = false;

    Evse*      evse      = getEvse(evse_id);
    Connector* connector = getConnector(evse_id, id);
    if (evse && connector)
    {
        saveConnector(*evse, *connector);
        ret = true;
    }

    return ret;
}

/** @copydoc void IConnectors::resetConnectors() */
void Connectors::resetConnectors()
{
    // Delete all EVSE data in memory
    releaseEvseLayout();

    // Delete database data
    auto query = m_database.query("DELETE FROM Evses WHERE TRUE;");
    if (query)
    {
        query->exec();
    }
    query = m_database.query("DELETE FROM Connectors WHERE TRUE;");
    if (query)
    {
        query->exec();
    }

    // Load the EVSEs layout from the device model
    loadEvsesLayout();

    // Load the connectors state
    loadConnectors();
}

/** @brief Initialize the database table */
void Connectors::initDatabaseTable()
{
    // Delete all EVSE data in memory
    releaseEvseLayout();

    // Init EVSEs and connectors tables
    initEvsesTable();
    initConnectorsTable();

    // Load the EVSEs layout from the device model
    loadEvsesLayout();

    // Load the connectors state
    loadConnectors();
}

/** @brief Release EVSE layout memory */
void Connectors::releaseEvseLayout()
{
    for (auto& evse : m_evses)
    {
        for (auto& connector : evse->connectors)
        {
            delete connector;
        }
        delete evse;
    }
    m_evses.clear();
}

/** @brief Initialize the EVSE table */
void Connectors::initEvsesTable()
{
    // Create database
    auto query = m_database.query("CREATE TABLE IF NOT EXISTS Evses ("
                                  "[id] INT UNSIGNED,"
                                  "[status] INT,"
                                  "[transaction_id] VARCHAR(36),"
                                  "[transaction_id_offline] VARCHAR(36),"
                                  "[transaction_start] BIGINT,"
                                  "[transaction_id_token] VARCHAR(36),"
                                  "[transaction_group_id_token] VARCHAR(36),"
                                  "PRIMARY KEY([id]));");
    if (query)
    {
        if (!query->exec())
        {
            LOG_ERROR << "Could not create EVSEs table : " << query->lastError();
        }
    }

    // Create parametrized queries
    m_evse_find_query   = m_database.query("SELECT * FROM Evses WHERE id=?;");
    m_evse_insert_query = m_database.query("INSERT INTO Evses VALUES (?, ?, ?, ?, ?, ?, ?);");
    m_evse_update_query = m_database.query("UPDATE Evses SET [status]=?, [transaction_id]=?, "
                                           "[transaction_id_offline]=?, [transaction_start]=?, [transaction_id_token]=?, "
                                           "[transaction_group_id_token]=? WHERE id=?;");
}

/** @brief Initialize the connectors table */
void Connectors::initConnectorsTable()
{
    // Create database
    auto query = m_database.query("CREATE TABLE IF NOT EXISTS Connectors ("
                                  "[evse_id] INT UNSIGNED,"
                                  "[id] INT UNSIGNED,"
                                  "[status] INT,"
                                  "PRIMARY KEY([id],[evse_id]));");
    if (query)
    {
        if (!query->exec())
        {
            LOG_ERROR << "Could not create connectors table : " << query->lastError();
        }
    }

    // Create parametrized queries
    m_connector_find_query   = m_database.query("SELECT * FROM Connectors WHERE evse_id=? AND id=?;");
    m_connector_insert_query = m_database.query("INSERT INTO Connectors VALUES (?, ?, ?);");
    m_connector_update_query = m_database.query("UPDATE Connectors SET [status]=? WHERE evse_id=? AND id=?;");
}

/** @brief Load the EVSEs layout from the device model */
void Connectors::loadEvsesLayout()
{
    // Load EVSEs
    unsigned int evse_count = 0u;
    bool         end        = false;
    do
    {
        GetVariableDataType requested_var;
        requested_var.component.name.assign("EVSE");
        requested_var.component.evse.value().id = evse_count + 1u;
        requested_var.variable.name.assign("Available");
        GetVariableResultType var = m_device_model.getVariable(requested_var);
        if ((var.attributeStatus == GetVariableStatusEnumType::Accepted) && (var.attributeValue.value().str() == "true"))
        {
            evse_count++;
        }
        else
        {
            end = true;
        }
    } while (!end);

    // Load connectors
    for (unsigned evse_id = 1u; evse_id <= evse_count; evse_id++)
    {
        unsigned int connector_count = 0u;
        end                          = false;
        do
        {
            GetVariableDataType requested_var;
            requested_var.component.name.assign("Connector");
            requested_var.component.evse.value().id                  = evse_id;
            requested_var.component.evse.value().connectorId.value() = connector_count + 1u;
            requested_var.variable.name.assign("Available");
            GetVariableResultType var = m_device_model.getVariable(requested_var);
            if ((var.attributeStatus == GetVariableStatusEnumType::Accepted) && (var.attributeValue.value().str() == "true"))
            {
                connector_count++;
            }
            else
            {
                end = true;
            }
        } while (!end);

        m_evses.push_back(new Evse(evse_id, m_timer_pool, connector_count));
    }

    LOG_INFO << m_evses.size() << " EVSE(s) found in device model";
    for (const auto& evse : m_evses)
    {
        LOG_INFO << "EVSE " << evse->id << ": " << evse->connectors.size() << " connector(s)";
    }
}

/** @brief Load the connectors states from the database */
void Connectors::loadConnectors()
{
    // Check the number of EVSEs in database
    unsigned int count = 0;
    auto         query = m_database.query("SELECT count(id) FROM Evses WHERE TRUE;");
    if (query && query->exec())
    {
        count = query->getUInt32(0);
        query->reset();
    }
    if (count != m_evses.size())
    {
        // Reset all database data
        LOG_WARNING << count << " EVSE(s) found in database / " << m_evses.size()
                    << " EVSE(s) declared in device model, reset all connectors data in database";
        query = m_database.query("DELETE FROM Evses WHERE TRUE;");
        if (query)
        {
            query->exec();
        }
        query = m_database.query("DELETE FROM Connectors WHERE TRUE;");
        if (query)
        {
            query->exec();
        }
    }

    // Load EVSEs data from database
    for (auto& evse : m_evses)
    {
        loadEvse(*evse);
    }
}

/** @brief Load the state of an EVSE from the database */
void Connectors::loadEvse(Evse& evse)
{
    if (m_evse_find_query && m_evse_insert_query)
    {
        // Check if the EVSE exists in database
        m_evse_find_query->bind(0, evse.id);
        if (m_evse_find_query->exec() && m_evse_find_query->hasRows())
        {
            // Check the number of connectors in database
            unsigned int count = 0;
            auto         query = m_database.query("SELECT count(id) FROM Connectors WHERE evse_id=?;");
            if (query)
            {
                query->bind(0, evse.id);
                if (query->exec())
                {
                    count = query->getUInt32(0);
                }
            }
            if (count != evse.connectors.size())
            {
                // Reset EVSE connector database data
                LOG_WARNING << count << " connector(s) found in database for EVSE " << evse.id << " / " << evse.connectors.size()
                            << " connector(s) declared in device model, reset all connectors data in database for this EVSE";
                query = m_database.query("DELETE FROM Connectors WHERE evse_id=?;");
                if (query)
                {
                    query->bind(0, evse.id);
                    query->exec();
                }
            }

            // Load EVSE data
            evse.status                     = static_cast<ConnectorStatusEnumType>(m_evse_find_query->getInt32(1u));
            evse.transaction_id             = m_evse_find_query->getString(2u);
            evse.transaction_id_offline     = m_evse_find_query->getString(3u);
            evse.transaction_start          = static_cast<std::time_t>(m_evse_find_query->getInt64(4u));
            evse.transaction_id_token       = m_evse_find_query->getString(5u);
            evse.transaction_group_id_token = m_evse_find_query->getString(6u);
        }
        else
        {
            // Create EVSE
            m_evse_insert_query->bind(0u, evse.id);
            m_evse_insert_query->bind(1u, static_cast<int>(evse.status));
            m_evse_insert_query->bind(2u, evse.transaction_id);
            m_evse_insert_query->bind(3u, evse.transaction_id_offline);
            m_evse_insert_query->bind(4u, static_cast<int64_t>(evse.transaction_start));
            m_evse_insert_query->bind(5u, evse.transaction_id_token);
            m_evse_insert_query->bind(6u, evse.transaction_group_id_token);
            m_evse_insert_query->exec();
            m_evse_insert_query->reset();
        }
        m_evse_find_query->reset();
    }

    // Load connectors data from database
    for (auto& connector : evse.connectors)
    {
        loadConnector(evse, *connector);
    }
}

/** @brief Load the state of a connector from the database */
void Connectors::loadConnector(Evse& evse, Connector& connector)
{
    if (m_connector_find_query && m_connector_insert_query)
    {
        // Check if the connector exists in database
        m_connector_find_query->bind(0, evse.id);
        m_connector_find_query->bind(1, connector.id);
        if (m_connector_find_query->exec() && m_connector_find_query->hasRows())
        {
            // Load connector data
            connector.status = static_cast<ConnectorStatusEnumType>(m_connector_find_query->getInt32(2u));
        }
        else
        {
            // Create connector
            m_connector_insert_query->bind(0u, evse.id);
            m_connector_insert_query->bind(1u, connector.id);
            m_connector_insert_query->bind(2u, static_cast<int>(connector.status));
            m_connector_insert_query->exec();
            m_connector_insert_query->reset();
        }
        m_connector_find_query->reset();
    }
}

/** @brief Save the state of an EVSE to the database */
void Connectors::saveEvse(const Evse& evse)
{
    if (m_evse_update_query)
    {
        m_evse_update_query->bind(0u, static_cast<int>(evse.status));
        m_evse_update_query->bind(1u, evse.transaction_id);
        m_evse_update_query->bind(2u, evse.transaction_id_offline);
        m_evse_update_query->bind(3u, static_cast<int64_t>(evse.transaction_start.timestamp()));
        m_evse_update_query->bind(4u, evse.transaction_id_token);
        m_evse_update_query->bind(5u, evse.transaction_group_id_token);
        m_evse_update_query->bind(6u, evse.id);
        if (m_evse_update_query->exec())
        {
            LOG_DEBUG << "EVSE " << evse.id << " updated in database";
        }
        else
        {
            LOG_ERROR << "Could not update EVSE " << evse.id << " : " << m_connector_update_query->lastError();
        }
        m_evse_update_query->reset();
    }
}

/** @brief Save the state of a connector to the database */
void Connectors::saveConnector(const Evse& evse, const Connector& connector)
{
    if (m_connector_update_query)
    {
        m_connector_update_query->bind(0u, static_cast<int>(connector.status));
        m_connector_update_query->bind(1u, evse.id);
        m_connector_update_query->bind(2u, connector.id);
        if (m_connector_update_query->exec())
        {
            LOG_DEBUG << "EVSE " << evse.id << " Connector " << connector.id << " updated in database";
        }
        else
        {
            LOG_ERROR << "Could not update EVSE " << evse.id << " Connector " << connector.id << " : "
                      << m_connector_update_query->lastError();
        }
        m_connector_update_query->reset();
    }
}

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp
