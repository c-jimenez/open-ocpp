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

#ifndef OPENOCPP_CONNECTORS_H
#define OPENOCPP_CONNECTORS_H

#include "Connector.h"
#include "Database.h"

#include <vector>

namespace ocpp
{
// Forward declarations
namespace config
{
class IOcppConfig;
} // namespace config

// Main namespace
namespace chargepoint
{

/** @brief Manage the connectors of a Charge Point */
class Connectors
{
  public:
    /** @brief Constructor */
    Connectors(ocpp::config::IOcppConfig& ocpp_config, ocpp::database::Database& database, ocpp::helpers::ITimerPool& timer_pool);

    /** @brief Destructor */
    virtual ~Connectors();

    /**
     * @brief Indicate if a connector id is valid
     * @param id Id of the connector
     * @return true is the id is valid, false otherwise
     */
    bool isValid(unsigned int id) const;

    /**
     * @brief Get the number of connectors
     * @return Number of connectors at the start of the stack
     */
    unsigned int getCount() const;

    /**
     * @brief Get the connector with the specified id
     * @param id Id of the connector
     * @return Requested connector if it exists, nullptr otherwise
     */
    Connector* getConnector(unsigned int id);

    /**
     * @brief Get the list of the connectors
     * @return List of the connectors
     */
    const std::vector<Connector*>& getConnectors() const { return m_connectors; }

    /**
     * @brief Get the connector associated with the charge point
     * @return Connector associated with the charge point
     */
    Connector& getChargePointConnector();

    /** @brief Initialize the database table */
    void initDatabaseTable();

    /**
     * @brief Save the state of a connector to the database
     * @param id Id of the connector
     */
    bool saveConnector(unsigned int id);

    /** @brief Reset the state of all connectors */
    void resetConnectors();

    /** @brief Connector id for the charge point */
    static constexpr const unsigned int CONNECTOR_ID_CHARGE_POINT = 0;

  private:
    /** @brief Standard OCPP configuration */
    ocpp::config::IOcppConfig& m_ocpp_config;
    /** @brief Charge point's database */
    ocpp::database::Database& m_database;
    /** @brief Timer pool */
    ocpp::helpers::ITimerPool& m_timer_pool;

    /** @brief List of available connectors */
    std::vector<Connector*> m_connectors;

    /** @brief Query to look for a connector */
    std::unique_ptr<ocpp::database::Database::Query> m_find_query;
    /** @brief Query to insert a connector */
    std::unique_ptr<ocpp::database::Database::Query> m_insert_query;
    /** @brief Query to update a connector */
    std::unique_ptr<ocpp::database::Database::Query> m_update_query;

    /** @brief Load the connectors states from the database */
    void loadConnectors();
    /** @brief Load the state of a connector from the database */
    bool loadConnector(Connector& connector);
    /** @brief Save the state of a connector to the database */
    bool saveConnector(const Connector& connector);
    /** @brief Create a connector in the database */
    bool createConnector(const Connector& connector);
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_CONNECTORS_H
