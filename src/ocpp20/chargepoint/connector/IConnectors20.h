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

#ifndef OPENOCPP_OCPP20_ICONNECTORS20_H
#define OPENOCPP_OCPP20_ICONNECTORS20_H

#include "Evse20.h"

#include <vector>

namespace ocpp
{
namespace chargepoint
{
namespace ocpp20
{

/** @brief Interface for the component which manages the EVSE and their connectors of a Charge Point */
class IConnectors
{
  public:
    /** @brief Destructor */
    virtual ~IConnectors() { }

    /**
     * @brief Get the EVSE with the specified id
     * @param id Id of the EVSE
     * @return Requested EVSE if it exists, nullptr otherwise
     */
    virtual Evse* getEvse(unsigned int id) = 0;

    /**
     * @brief Get the EVSE with the specified id
     * @param id Id of the EVSE
     * @return Requested EVSE if it exists, nullptr otherwise
     */
    virtual const Evse* getEvse(unsigned int id) const = 0;

    /**
     * @brief Get the connector with the specified id
     * @param evse_id Id of the EVSE of the connector
     * @param id Id of the connector
     * @return Requested connector if it exists, nullptr otherwise
     */
    virtual Connector* getConnector(unsigned int evse_id, unsigned int id) = 0;

    /**
     * @brief Get the list of the EVSEs
     * @return List of the EVSEs
     */
    virtual const std::vector<Evse*>& getEvses() const = 0;

    /**
     * @brief Get the list of the connectors of an EVSE
     * @param id Id of the EVSE
     * @return List of the connectors of the EVSE
     */
    virtual const std::vector<Connector*>* getConnectors(unsigned int id) const = 0;

    /**
     * @brief Save the state of an EVSE to the database
     * @param evse_id Id of the EVSE
     * @return true if the state has been saved, false otherwise
     */
    virtual bool saveEvse(unsigned int evse_id) = 0;

    /**
     * @brief Save the state of a connector to the database
     * @param evse_id Id of the EVSE of the connector
     * @param id Id of the connector
     * @return true if the state has been saved, false otherwise
     */
    virtual bool saveConnector(unsigned int evse_id, unsigned int id) = 0;

    /** @brief Reset the state of all the connectors */
    virtual void resetConnectors() = 0;
};

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_OCPP20_ICONNECTORS20_H
