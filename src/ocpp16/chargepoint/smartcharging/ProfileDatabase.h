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

#ifndef OPENOCPP_PROFILEDATABASE_H
#define OPENOCPP_PROFILEDATABASE_H

#include "ChargingProfile.h"
#include "Database.h"

#include <memory>
#include <set>

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

/** @brief Handle persistency of charging profiles */
class ProfileDatabase
{
  public:
    /** @brief Constructor */
    ProfileDatabase(ocpp::config::IOcppConfig& ocpp_config, ocpp::database::Database& database);

    /** @brief Destructor */
    virtual ~ProfileDatabase();

    // ProfileDatabase interface

    /** @brief Stores a profile alongside its target connector */
    typedef std::pair<unsigned int, ocpp::types::ocpp16::ChargingProfile> ChargingProfileInfo;
    /** @brief Allow sorting of profiles by stack level and connector id*/
    struct ChargingProfileInfoLess
    {
        bool operator()(const ChargingProfileInfo& lhs, const ChargingProfileInfo& rhs) const
        {
            return ((lhs.second.stackLevel > rhs.second.stackLevel) || (lhs.first > rhs.first));
        }
    };
    /** @brief List of charging profiles stored by stack level */
    typedef std::multiset<ChargingProfileInfo, ChargingProfileInfoLess> ChargingProfileList;

    /**
     * @brief Clear one or multiple charging profiles with match criteria
     *        (if none specified, all the profiles are cleared)
     * @param id The ID of the charging profile to clear
     * @param connector_id Specifies the ID of the connector for which to clear charging profiles
     * @param purpose Specifies to purpose of the charging profiles that will be cleared
     * @param level Specifies the stackLevel for which charging profiles will be cleared
     * @return true if at least 1 profile matched the criteria, false otherwise
     */
    bool clear(ocpp::types::Optional<int>                                             id,
               ocpp::types::Optional<unsigned int>                                    connector_id = ocpp::types::Optional<unsigned int>(),
               ocpp::types::Optional<ocpp::types::ocpp16::ChargingProfilePurposeType> purpose =
                   ocpp::types::Optional<ocpp::types::ocpp16::ChargingProfilePurposeType>(),
               ocpp::types::Optional<unsigned int> level = ocpp::types::Optional<unsigned int>());

    /**
     * @brief Install a charging profile
     * @param connector_id Id of the connector targeted by the charging profile
     * @param profile Charging profile to install
     * @return true if the charging profile has been installed, false otherwise
     */
    bool install(unsigned int connector_id, const ocpp::types::ocpp16::ChargingProfile& profile);

    /**
     * @brief Assign the pending TxProfile of a connector to a transaction
     * @param connector_id Id of the connector targeted by the charging profile
     * @param transaction_id Transaction to associate with the profile
     */
    void assignPendingTxProfiles(unsigned int connector_id, int transaction_id);

    /** @brief ChargePointMaxProfile stack */
    const ChargingProfileList& chargePointMaxProfiles() const { return m_chargepoint_max_profiles; }

    /** @brief TxDefaultProfile stack */
    const ChargingProfileList& txDefaultProfiles() const { return m_txdefault_profiles; }

    /** @brief TxProfile stack */
    const ChargingProfileList& txProfiles() const { return m_tx_profiles; }

  private:
    /** @brief Standard OCPP configuration */
    ocpp::config::IOcppConfig& m_ocpp_config;
    /** @brief Charge point's database */
    ocpp::database::Database& m_database;

    /** @brief Query to delete a profile */
    std::unique_ptr<ocpp::database::Database::Query> m_delete_query;
    /** @brief Query to insert a profile */
    std::unique_ptr<ocpp::database::Database::Query> m_insert_query;

    /** @brief ChargePointMaxProfile stack */
    ChargingProfileList m_chargepoint_max_profiles;
    /** @brief TxDefaultProfile stack */
    ChargingProfileList m_txdefault_profiles;
    /** @brief TxProfile stack */
    ChargingProfileList m_tx_profiles;

    /** @brief Initialize the database table */
    void initDatabaseTable();

    /** @brief Load profiles from the database */
    void load();

    /** @brief Serialize a profile to a string */
    std::string serialize(const ocpp::types::ocpp16::ChargingProfile& profile);
    /** @brief Deserialize a profile from a string */
    bool deserialize(const std::string& profile_str, ocpp::types::ocpp16::ChargingProfile& profile);
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_PROFILEDATABASE_H
