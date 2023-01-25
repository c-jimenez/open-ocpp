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

#include "AuthentCache.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "Database.h"
#include "OcppConfigStub.h"
#include "ProfileDatabase.h"
#include "doctest_wrapper.h"

#include <filesystem>

using namespace ocpp::chargepoint;
using namespace ocpp::config;
using namespace ocpp::database;
using namespace ocpp::messages;
using namespace ocpp::types;

static constexpr const char* DATABASE_PATH = "/tmp/test.db";

Database database;

TEST_SUITE("Profile database")
{
    TEST_CASE("Setup")
    {
        std::filesystem::remove(DATABASE_PATH);
        CHECK(database.open(DATABASE_PATH));
    }

    TEST_CASE("Profile installation")
    {
        OcppConfigStub ocpp_config;

        ocpp_config.setConfigValue("MaxChargingProfilesInstalled", "5");

        ProfileDatabase profile_db(ocpp_config, database);

        std::vector<ChargingProfile*> profiles;

        ChargingProfile profile1;
        profile1.chargingProfileId = 1;
        profile1.stackLevel        = 5;
        profiles.push_back(&profile1);

        ChargingProfile profile2;
        profile2.chargingProfileId = 2;
        profile2.stackLevel        = 4;
        profiles.push_back(&profile2);

        ChargingProfile profile3;
        profile3.chargingProfileId = 3;
        profile3.stackLevel        = 1;
        profiles.push_back(&profile3);

        ChargingProfile profile4;
        profile4.chargingProfileId = 4;
        profile4.stackLevel        = 3;
        profiles.push_back(&profile4);

        ChargingProfile profile5;
        profile5.chargingProfileId = 5;
        profile5.stackLevel        = 2;
        profiles.push_back(&profile5);

        ChargingProfile profile6;
        profile6.chargingProfileId                 = 6;
        profile6.stackLevel                        = 6;
        profile6.chargingProfileKind               = ChargingProfileKindType::Absolute;
        profile6.chargingProfilePurpose            = ChargingProfilePurposeType::TxProfile;
        profile6.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;

        // Tx profiles
        for (size_t i = 0; i < profiles.size(); i++)
        {
            profiles[i]->chargingProfileKind               = ChargingProfileKindType::Absolute;
            profiles[i]->chargingProfilePurpose            = ChargingProfilePurposeType::TxProfile;
            profiles[i]->chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
            CHECK(profile_db.install(1u, *profiles[i]));
        }
        CHECK_FALSE(profile_db.install(1u, profile6));
        CHECK_EQ(profile_db.txProfiles().size(), 5u);
        CHECK_EQ(profile_db.txDefaultProfiles().size(), 0u);
        CHECK_EQ(profile_db.chargePointMaxProfiles().size(), 0u);
        size_t level = 5u;
        for (auto iter = profile_db.txProfiles().cbegin(); iter != profile_db.txProfiles().cend(); ++iter)
        {
            CHECK_EQ(iter->first, 1u);
            CHECK_EQ(iter->second.stackLevel, level);
            level--;
        }

        // Clear profiles
        profile_db.clear(Optional<int>(), 1u);
        CHECK_EQ(profile_db.txProfiles().size(), 0u);
        CHECK_EQ(profile_db.txDefaultProfiles().size(), 0u);
        CHECK_EQ(profile_db.chargePointMaxProfiles().size(), 0u);

        // TxDefault profiles
        for (size_t i = 0; i < profiles.size(); i++)
        {
            profiles[i]->chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
            CHECK(profile_db.install(1u, *profiles[i]));
        }
        CHECK_FALSE(profile_db.install(1u, profile6));
        CHECK_EQ(profile_db.txProfiles().size(), 0u);
        CHECK_EQ(profile_db.txDefaultProfiles().size(), 5u);
        CHECK_EQ(profile_db.chargePointMaxProfiles().size(), 0u);
        level = 5u;
        for (auto iter = profile_db.txDefaultProfiles().cbegin(); iter != profile_db.txDefaultProfiles().cend(); ++iter)
        {
            CHECK_EQ(iter->first, 1u);
            CHECK_EQ(iter->second.stackLevel, level);
            level--;
        }

        // Clear profiles
        profile_db.clear(Optional<int>(), 1u);
        CHECK_EQ(profile_db.txProfiles().size(), 0u);
        CHECK_EQ(profile_db.txDefaultProfiles().size(), 0u);
        CHECK_EQ(profile_db.chargePointMaxProfiles().size(), 0u);

        // ChargePointMax profiles
        for (size_t i = 0; i < profiles.size(); i++)
        {
            profiles[i]->chargingProfilePurpose = ChargingProfilePurposeType::ChargePointMaxProfile;
            CHECK(profile_db.install(1u, *profiles[i]));
        }
        CHECK_FALSE(profile_db.install(1u, profile6));
        CHECK_EQ(profile_db.txProfiles().size(), 0u);
        CHECK_EQ(profile_db.txDefaultProfiles().size(), 0u);
        CHECK_EQ(profile_db.chargePointMaxProfiles().size(), 5u);
        level = 5u;
        for (auto iter = profile_db.chargePointMaxProfiles().cbegin(); iter != profile_db.chargePointMaxProfiles().cend(); ++iter)
        {
            CHECK_EQ(iter->first, 1u);
            CHECK_EQ(iter->second.stackLevel, level);
            level--;
        }

        // Clear profiles
        profile_db.clear(Optional<int>(), 1u);
        CHECK_EQ(profile_db.txProfiles().size(), 0u);
        CHECK_EQ(profile_db.txDefaultProfiles().size(), 0u);
        CHECK_EQ(profile_db.chargePointMaxProfiles().size(), 0u);
    }

    TEST_CASE("Profile stacking")
    {
        OcppConfigStub ocpp_config;

        ocpp_config.setConfigValue("MaxChargingProfilesInstalled", "5");

        ProfileDatabase profile_db(ocpp_config, database);

        std::vector<ChargingProfile*> profiles;

        ChargingProfile profile1;
        profile1.stackLevel = 5;
        profiles.push_back(&profile1);

        ChargingProfile profile2;
        profile2.stackLevel = 5;
        profiles.push_back(&profile2);

        ChargingProfile profile3;
        profile3.stackLevel = 3;
        profiles.push_back(&profile3);

        ChargingProfile profile4;
        profile4.stackLevel = 3;
        profiles.push_back(&profile4);

        ChargingProfile profile5;
        profile5.stackLevel = 2;
        profiles.push_back(&profile5);

        for (size_t i = 0; i < profiles.size(); i++)
        {
            profiles[i]->chargingProfileId                 = static_cast<int>(i);
            profiles[i]->chargingProfileKind               = ChargingProfileKindType::Absolute;
            profiles[i]->chargingProfilePurpose            = ChargingProfilePurposeType::TxProfile;
            profiles[i]->chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        }

        CHECK(profile_db.install(0u, profile5));
        CHECK(profile_db.install(1u, profile3));
        CHECK(profile_db.install(0u, profile1));
        CHECK(profile_db.install(0u, profile4));
        CHECK(profile_db.install(1u, profile2));
        CHECK_EQ(profile_db.txProfiles().size(), 5u);

        // Stacking order
        auto iter = profile_db.txProfiles().cbegin();

        CHECK_EQ(iter->first, 1u);
        CHECK_EQ(iter->second.chargingProfileId, 1);
        CHECK_EQ(iter->second.stackLevel, 5);
        ++iter;
        CHECK_EQ(iter->first, 0u);
        CHECK_EQ(iter->second.chargingProfileId, 0);
        CHECK_EQ(iter->second.stackLevel, 5);
        ++iter;
        CHECK_EQ(iter->first, 1u);
        CHECK_EQ(iter->second.chargingProfileId, 2);
        CHECK_EQ(iter->second.stackLevel, 3);
        ++iter;
        CHECK_EQ(iter->first, 0u);
        CHECK_EQ(iter->second.chargingProfileId, 3);
        CHECK_EQ(iter->second.stackLevel, 3);
        ++iter;
        CHECK_EQ(iter->first, 0u);
        CHECK_EQ(iter->second.chargingProfileId, 4);
        CHECK_EQ(iter->second.stackLevel, 2);
        ++iter;

        // Clear profiles
        profile_db.clear(Optional<int>(), 0u);
        profile_db.clear(Optional<int>(), 1u);
        CHECK_EQ(profile_db.txProfiles().size(), 0u);
        CHECK_EQ(profile_db.txDefaultProfiles().size(), 0u);
        CHECK_EQ(profile_db.chargePointMaxProfiles().size(), 0u);
    }

    TEST_CASE("Cleanup")
    {
        CHECK(database.close());
        std::filesystem::remove(DATABASE_PATH);
    }
}
