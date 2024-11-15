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

#ifndef OPENOCPP_OCPP20_CONNECTORS20_H
#define OPENOCPP_OCPP20_CONNECTORS20_H

#include "Database.h"
#include "IConnectors20.h"
#include "IDeviceModel20.h"

namespace ocpp
{
namespace chargepoint
{
namespace ocpp20
{

/** @brief Manage the EVSE and their connectors of a Charge Point */
class Connectors : public IConnectors
{
  public:
    /** @brief Constructor */
    Connectors(IDeviceModel& device_model, ocpp::database::Database& database, ocpp::helpers::ITimerPool& timer_pool);

    /** @brief Destructor */
    virtual ~Connectors();

    /** @copydoc Evse* IConnectors::getEvse(unsigned int) */
    Evse* getEvse(unsigned int id) override;

    /** @copydoc Evse* IConnectors::getEvse(unsigned int) */
    const Evse* getEvse(unsigned int id) const override;

    /** @copydoc Connector* IConnectors::getConnector(unsigned int, unsigned int) */
    Connector* getConnector(unsigned int evse_id, unsigned int id) override;

    /** @copydoc const std::vector<Evse*>& IConnectors::getEvses() */
    const std::vector<Evse*>& getEvses() const override { return m_evses; }

    /** @copydoc const std::vector<Connector*>* IConnectors::getConnectors(unsigned int) */
    const std::vector<Connector*>* getConnectors(unsigned int id) const override;

    /** @copydoc bool IConnectors::saveEvse(unsigned int) */
    bool saveEvse(unsigned int evse_id) override;

    /** @copydoc bool IConnectors::saveConnector(unsigned int, unsigned int) */
    bool saveConnector(unsigned int evse_id, unsigned int id) override;

    /** @copydoc void IConnectors::resetConnectors() */
    void resetConnectors() override;

    /** @brief Initialize the database table */
    void initDatabaseTable();

  private:
    /** @brief Device model */
    IDeviceModel& m_device_model;
    /** @brief Charge point's database */
    ocpp::database::Database& m_database;
    /** @brief Timer pool */
    ocpp::helpers::ITimerPool& m_timer_pool;

    /** @brief List of available EVSEs */
    std::vector<Evse*> m_evses;

    /** @brief Query to look for an EVSE */
    std::unique_ptr<ocpp::database::Database::Query> m_evse_find_query;
    /** @brief Query to insert an EVSE */
    std::unique_ptr<ocpp::database::Database::Query> m_evse_insert_query;
    /** @brief Query to update an EVSE */
    std::unique_ptr<ocpp::database::Database::Query> m_evse_update_query;

    /** @brief Query to look for a connector */
    std::unique_ptr<ocpp::database::Database::Query> m_connector_find_query;
    /** @brief Query to insert a connector */
    std::unique_ptr<ocpp::database::Database::Query> m_connector_insert_query;
    /** @brief Query to update a connector */
    std::unique_ptr<ocpp::database::Database::Query> m_connector_update_query;

    /** @brief Release EVSE layout memory */
    void releaseEvseLayout();

    /** @brief Initialize the EVSE table */
    void initEvsesTable();
    /** @brief Initialize the connectors table */
    void initConnectorsTable();

    /** @brief Load the EVSEs layout from the device model */
    void loadEvsesLayout();
    /** @brief Load the connectors states from the database */
    void loadConnectors();

    /** @brief Load the state of an EVSE from the database */
    void loadEvse(Evse& evse);
    /** @brief Load the state of a connector from the database */
    void loadConnector(Evse& evse, Connector& connector);

    /** @brief Save the state of an EVSE to the database */
    void saveEvse(const Evse& evse);
    /** @brief Save the state of a connector to the database */
    void saveConnector(const Evse& evse, const Connector& connector);
};

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_OCPP20_CONNECTORS20_H
