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

#include "ProfileDatabase.h"
#include "ChargingProfile.h"
#include "ChargingProfileConverter.h"
#include "Connectors.h"
#include "IOcppConfig.h"

#include <algorithm>
#include <sstream>
#include <vector>

using namespace ocpp::database;
using namespace ocpp::messages;
using namespace ocpp::messages::ocpp16;
using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace chargepoint
{

/** @brief Constructor */
ProfileDatabase::ProfileDatabase(ocpp::config::IOcppConfig& ocpp_config, ocpp::database::Database& database)
    : m_ocpp_config(ocpp_config),
      m_database(database),
      m_delete_query(),
      m_insert_query(),
      m_chargepoint_max_profiles(),
      m_txdefault_profiles(),
      m_tx_profiles()
{
    initDatabaseTable();
    load();
}

/** @brief Destructor */
ProfileDatabase::~ProfileDatabase() { }

/** @brief Clear one or multiple charging profiles with match criteria
 *         (if none specified, all the profiles are cleared) */
bool ProfileDatabase::clear(ocpp::types::Optional<int>                                             id,
                            ocpp::types::Optional<unsigned int>                                    connector_id,
                            ocpp::types::Optional<ocpp::types::ocpp16::ChargingProfilePurposeType> purpose,
                            ocpp::types::Optional<unsigned int>                                    level)
{
    bool ret = false;

    // Clear all ?
    if (!id.isSet() && !connector_id.isSet() && !purpose.isSet() && !level.isSet())
    {
        // Check existing profiles
        if (!m_chargepoint_max_profiles.empty() || !m_txdefault_profiles.empty() || !m_tx_profiles.empty())
        {
            // Clear lists
            m_chargepoint_max_profiles.clear();
            m_txdefault_profiles.clear();
            m_tx_profiles.clear();

            // Clear database
            auto query = m_database.query("DELETE FROM ChargingProfiles WHERE TRUE;");
            if (query)
            {
                query->exec();
            }

            ret = true;
        }
    }
    else if (id.isSet())
    {
        // Clear selected profile only
        for (ChargingProfileList* profiles_list : {&m_chargepoint_max_profiles, &m_txdefault_profiles, &m_tx_profiles})
        {
            auto iter = std::find_if(profiles_list->begin(),
                                     profiles_list->end(),
                                     [&id](const ChargingProfileInfo& profile) { return (profile.second.chargingProfileId == id); });
            if (iter != profiles_list->end())
            {
                // Erase from list
                profiles_list->erase(iter);

                // Erase from database
                if (m_delete_query)
                {
                    m_delete_query->bind(0, id);
                    m_delete_query->exec();
                    m_delete_query->reset();
                }
                ret = true;
                break;
            }
        }
    }
    else
    {
        // Selected profiles purposes
        std::vector<ChargingProfileList*> profiles_lists;
        if (purpose.isSet())
        {
            switch (purpose)
            {
                case ChargingProfilePurposeType::ChargePointMaxProfile:
                    profiles_lists.push_back(&m_chargepoint_max_profiles);
                    break;

                case ChargingProfilePurposeType::TxDefaultProfile:
                    profiles_lists.push_back(&m_txdefault_profiles);
                    break;

                case ChargingProfilePurposeType::TxProfile:
                // Intended fallthrough
                default:
                    profiles_lists.push_back(&m_tx_profiles);
                    break;
            }
        }
        else
        {
            profiles_lists = {&m_chargepoint_max_profiles, &m_txdefault_profiles, &m_tx_profiles};
        }

        // Search into selected lists
        for (ChargingProfileList* profiles_list : profiles_lists)
        {
            // Select profiles
            std::vector<ChargingProfileList::iterator> profiles_to_erase;
            for (auto iter = profiles_list->begin(); iter != profiles_list->end(); iter++)
            {
                bool match_connector = (!connector_id.isSet() || (iter->first == connector_id));
                bool match_level     = (!level.isSet() || (iter->second.stackLevel == level));
                if (match_connector && match_level)
                {
                    profiles_to_erase.push_back(iter);
                    ret = true;
                }
            }

            // Erase profiles
            for (auto& iter : profiles_to_erase)
            {
                // Erase in database
                if (m_delete_query)
                {
                    m_delete_query->bind(0, iter->second.chargingProfileId);
                    m_delete_query->exec();
                    m_delete_query->reset();
                }

                // Erase in list
                profiles_list->erase(iter);
            }
        }
    }

    return ret;
}

/** @brief Install a charging profile */
bool ProfileDatabase::install(unsigned int connector_id, const ocpp::types::ocpp16::ChargingProfile& profile)
{
    bool ret = false;

    // Get the profiles list
    ChargingProfileList* profiles_list;
    switch (profile.chargingProfilePurpose)
    {
        case ChargingProfilePurposeType::ChargePointMaxProfile:
        {
            profiles_list = &m_chargepoint_max_profiles;
        }
        break;

        case ChargingProfilePurposeType::TxDefaultProfile:
        {
            profiles_list = &m_txdefault_profiles;
        }
        break;

        case ChargingProfilePurposeType::TxProfile:
        // Intended fallthrough
        default:
        {
            profiles_list = &m_tx_profiles;
        }
        break;
    }

    // Check if a profile with the same connector and stack level exists
    auto iter_profile =
        std::find_if(profiles_list->begin(),
                     profiles_list->end(),
                     [connector_id, &profile](const ChargingProfileInfo& profile_info)
                     { return ((profile_info.first == connector_id) && (profile_info.second.stackLevel == profile.stackLevel)); });
    if (iter_profile != profiles_list->end())
    {
        // Erase existing profile
        if (m_delete_query)
        {
            m_delete_query->bind(0, iter_profile->second.chargingProfileId);
            m_delete_query->exec();
            m_delete_query->reset();
        }
        profiles_list->erase(iter_profile);
    }

    // Check maximum number of installed profiles
    size_t installed_profiles_count = m_chargepoint_max_profiles.size() + m_txdefault_profiles.size() + m_tx_profiles.size();
    if (installed_profiles_count < m_ocpp_config.maxChargingProfilesInstalled())
    {
        // Insert into list
        profiles_list->insert({connector_id, profile});

        // Insert into database
        if (m_insert_query)
        {
            // Serialize profile
            std::string profile_str = serialize(profile);

            // Insert
            m_insert_query->bind(0, profile.chargingProfileId);
            m_insert_query->bind(1, connector_id);
            m_insert_query->bind(2, profile_str);
            m_insert_query->exec();
            m_insert_query->reset();
        }

        ret = true;
    }

    return ret;
}

/** @brief Assign the pending TxProfile of a connector to a transaction */
void ProfileDatabase::assignPendingTxProfiles(unsigned int connector_id, int transaction_id)
{
    std::vector<int> profiles_to_remove;

    // Look for pending profiles
    for (const auto& profile : m_tx_profiles)
    {
        if (((profile.first == Connectors::CONNECTOR_ID_CHARGE_POINT) || (profile.first == connector_id)) &&
            !profile.second.transactionId.isSet())
        {
            // If no connector set, remove the profile completly
            // so that it won't be used again
            if (profile.first == Connectors::CONNECTOR_ID_CHARGE_POINT)
            {
                profiles_to_remove.push_back(profile.second.chargingProfileId);
            }

            // Assign transaction to a new profile
            ChargingProfile assigned_profile = profile.second;
            assigned_profile.transactionId   = transaction_id;

            // Replace existing with assigned profile
            install(connector_id, assigned_profile);
        }
    }

    // Remove profiles
    for (int profile_id : profiles_to_remove)
    {
        clear(profile_id);
    }
}

/** @brief Initialize the database table */
void ProfileDatabase::initDatabaseTable()
{
    // Create database
    auto query = m_database.query("CREATE TABLE IF NOT EXISTS ChargingProfiles ("
                                  "[id]	INTEGER,"
                                  "[connector]	INTEGER,"
                                  "[profile] VARCHAR(1024),"
                                  "PRIMARY KEY([id]));");
    if (query.get())
    {
        query->exec();
    }

    // Create parametrized queries
    m_delete_query = m_database.query("DELETE FROM ChargingProfiles WHERE id=?;");
    m_insert_query = m_database.query("INSERT INTO ChargingProfiles VALUES (?, ?, ?);");
}

/** @brief Load profiles from the database */
void ProfileDatabase::load()
{
    // Query all stored profiles
    auto query = m_database.query("SELECT * FROM ChargingProfiles WHERE TRUE;");
    if (query.get())
    {
        if (query->exec() && query->hasRows())
        {
            do
            {
                // Extract table data
                int          id          = query->getInt32(0);
                unsigned int connector   = query->getUInt32(1);
                std::string  profile_str = query->getString(2);

                // Deserialize profile
                ChargingProfileInfo profile;
                profile.first = connector;
                if (deserialize(profile_str, profile.second) && (profile.second.chargingProfileId == id))
                {
                    // Add the profile to the corresponding list
                    switch (profile.second.chargingProfilePurpose)
                    {
                        case ChargingProfilePurposeType::ChargePointMaxProfile:
                        {
                            m_chargepoint_max_profiles.insert(profile);
                        }
                        break;

                        case ChargingProfilePurposeType::TxDefaultProfile:
                        {
                            m_txdefault_profiles.insert(profile);
                        }
                        break;

                        case ChargingProfilePurposeType::TxProfile:
                        // Intended fallthrough
                        default:
                        {
                            m_tx_profiles.insert(profile);
                        }
                        break;
                    }
                }
            } while (query->next());
        }
    }
}

/** @brief Serialize a profile to a string */
std::string ProfileDatabase::serialize(const ocpp::types::ocpp16::ChargingProfile& profile)
{
    std::string profile_str;

    rapidjson::Document      profile_json(rapidjson::kObjectType);
    ChargingProfileConverter charging_profile_converter;
    charging_profile_converter.setAllocator(&profile_json.GetAllocator());
    charging_profile_converter.toJson(profile, profile_json);

    rapidjson::StringBuffer                    buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.SetMaxDecimalPlaces(1); // OCPP decimals have 1 digit precision
    profile_json.Accept(writer);
    profile_str = buffer.GetString();
    return profile_str;
}

/** @brief Deserialize a profile from a string */
bool ProfileDatabase::deserialize(const std::string& profile_str, ocpp::types::ocpp16::ChargingProfile& profile)
{
    std::string         error_code;
    std::string         error_message;
    rapidjson::Document profile_json;
    profile_json.Parse(profile_str.c_str());
    ChargingProfileConverter charging_profile_converter;
    charging_profile_converter.setAllocator(&profile_json.GetAllocator());
    return charging_profile_converter.fromJson(profile_json, profile, error_code, error_message);
}

} // namespace chargepoint
} // namespace ocpp
