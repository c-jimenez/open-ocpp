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

#ifndef OPENOCPP_ISMARTCHARGINGMANAGER_H
#define OPENOCPP_ISMARTCHARGINGMANAGER_H

#include "ChargingProfile.h"
#include "SmartChargingSetpoint.h"

namespace ocpp
{
namespace chargepoint
{

/** @brief Interface for the smart charging manager of the charge point */
class ISmartChargingManager
{
  public:
    /** @brief Destructor */
    virtual ~ISmartChargingManager() { }

    /**
     * @brief Get the smart charging setpoints for a connector and the whole charge point
     * @param connector_id Id of the connector
     * @param charge_point_setpoint Setpoint of the whole charge point (not set if no active profile)
     * @param connector_setpoint Setpoint of the given connector (not set if no active profile)
     * @param unit Setpoint unit (A or W)
     * @return true if the setpoints have been computed, false otherwise
     */
    virtual bool getSetpoint(unsigned int                                                       connector_id,
                             ocpp::types::Optional<ocpp::types::ocpp16::SmartChargingSetpoint>& charge_point_setpoint,
                             ocpp::types::Optional<ocpp::types::ocpp16::SmartChargingSetpoint>& connector_setpoint,
                             ocpp::types::ocpp16::ChargingRateUnitType unit = ocpp::types::ocpp16::ChargingRateUnitType::A) = 0;

    /**
     * @brief Install a TxProfile charging profile on a connector
     * @param connector_id Id of the connector targeted by the charging profile
     * @param profile Charging profile to install
     * @return true if the charging profile has been installed, false otherwise
     */
    virtual bool installTxProfile(unsigned int connector_id, const ocpp::types::ocpp16::ChargingProfile& profile) = 0;

    /**
     * @brief Assign the pending TxProfile of a connector to a transaction
     * @param connector_id Id of the connector targeted by the charging profile
     * @param transaction_id Transaction to associate with the profile
     */
    virtual void assignPendingTxProfiles(unsigned int connector_id, int transaction_id) = 0;

    /**
     * @brief Clear all the TxProfile charging profiles on a connector
     * @param connector_id Id of the connector
     */
    virtual void clearTxProfiles(unsigned int connector_id) = 0;
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_ISMARTCHARGINGMANAGER_H
