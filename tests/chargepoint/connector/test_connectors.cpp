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

#include "Connectors.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "Database.h"
#include "OcppConfigStub.h"
#include "TestableTimerPool.h"
#include "doctest_wrapper.h"

#include <filesystem>

using namespace ocpp::config;
using namespace ocpp::types;
using namespace ocpp::helpers;
using namespace ocpp::database;
using namespace ocpp::chargepoint;

static constexpr const char* DATABASE_PATH = "/tmp/test.db";

Database database;

TEST_SUITE("Authentication cache")
{
    TEST_CASE("Setup")
    {
        std::filesystem::remove(DATABASE_PATH);
        CHECK(database.open(DATABASE_PATH));
    }

    TEST_CASE("Standard operations")
    {
        OcppConfigStub    ocpp_config;
        TestableTimerPool timer_pool;

        // Initialize for 1 connector
        ocpp_config.setConfigValue("NumberOfConnectors", "2");

        Connectors connectors(ocpp_config, database, timer_pool);
        connectors.initDatabaseTable();

        // Check number of initialized connectors
        const std::vector<Connector*>& cons = connectors.getConnectors();
        CHECK_EQ(connectors.getCount(), 2u);
        CHECK_EQ(cons.size(), 3u);

        // Check connectors
        Connector* con = connectors.getConnector(0u);
        CHECK_NE(con, nullptr);
        CHECK(connectors.isValid(0u));
        CHECK_EQ(con->id, 0u);
        CHECK_EQ(con->status, ChargePointStatus::Available);
        CHECK_EQ(con->last_notified_status, ChargePointStatus::Available);
        CHECK_EQ(con->transaction_id, 0);
        CHECK_EQ(con->transaction_id_offline, 0);
        CHECK_EQ(con->transaction_start.timestamp(), 0);
        CHECK_EQ(con->transaction_id_tag, "");
        CHECK_EQ(con->transaction_parent_id_tag, "");
        CHECK_EQ(con->reservation_id, 0);
        CHECK_EQ(con->reservation_id_tag, "");
        CHECK_EQ(con->reservation_parent_id_tag, "");
        CHECK_EQ(con->reservation_expiry_date.timestamp(), 0);

        con = connectors.getConnector(1u);
        CHECK_NE(con, nullptr);
        CHECK(connectors.isValid(1u));
        CHECK_EQ(con->id, 1u);
        CHECK_EQ(con->status, ChargePointStatus::Available);
        CHECK_EQ(con->last_notified_status, ChargePointStatus::Available);
        CHECK_EQ(con->transaction_id, 0);
        CHECK_EQ(con->transaction_id_offline, 0);
        CHECK_EQ(con->transaction_start.timestamp(), 0);
        CHECK_EQ(con->transaction_id_tag, "");
        CHECK_EQ(con->transaction_parent_id_tag, "");
        CHECK_EQ(con->reservation_id, 0);
        CHECK_EQ(con->reservation_id_tag, "");
        CHECK_EQ(con->reservation_parent_id_tag, "");
        CHECK_EQ(con->reservation_expiry_date.timestamp(), 0);

        con = connectors.getConnector(2u);
        CHECK_NE(con, nullptr);
        CHECK(connectors.isValid(2u));
        CHECK_EQ(con->id, 2u);
        CHECK_EQ(con->status, ChargePointStatus::Available);
        CHECK_EQ(con->last_notified_status, ChargePointStatus::Available);
        CHECK_EQ(con->transaction_id, 0);
        CHECK_EQ(con->transaction_id_offline, 0);
        CHECK_EQ(con->transaction_start.timestamp(), 0);
        CHECK_EQ(con->transaction_id_tag, "");
        CHECK_EQ(con->transaction_parent_id_tag, "");
        CHECK_EQ(con->reservation_id, 0u);
        CHECK_EQ(con->reservation_id_tag, "");
        CHECK_EQ(con->reservation_parent_id_tag, "");
        CHECK_EQ(con->reservation_expiry_date.timestamp(), 0);

        con = connectors.getConnector(3u);
        CHECK_EQ(con, nullptr);
        CHECK_FALSE(connectors.isValid(3u));

        // Update connector 0
        con                            = connectors.getConnector(0u);
        con->status                    = ChargePointStatus::Reserved;
        con->last_notified_status      = ChargePointStatus::Unavailable;
        con->transaction_id            = 1234;
        con->transaction_id_offline    = 5678;
        con->transaction_start         = DateTime(12345678ll);
        con->transaction_id_tag        = "TAG1";
        con->transaction_parent_id_tag = "PARENT_TAG1";
        con->reservation_id            = 9876;
        con->reservation_id_tag        = "TAG2";
        con->reservation_parent_id_tag = "PARENT_TAG2";
        con->reservation_expiry_date   = DateTime(87654321ll);

        // Update connector 1
        con                            = connectors.getConnector(1u);
        con->status                    = ChargePointStatus::Preparing;
        con->last_notified_status      = ChargePointStatus::Charging;
        con->transaction_id            = 5678;
        con->transaction_id_offline    = 1234;
        con->transaction_start         = DateTime(87654321ll);
        con->transaction_id_tag        = "TAG3";
        con->transaction_parent_id_tag = "PARENT_TAG3";
        con->reservation_id            = 4567;
        con->reservation_id_tag        = "TAG4";
        con->reservation_parent_id_tag = "PARENT_TAG4";
        con->reservation_expiry_date   = DateTime(12345678ll);

        // Update connector 2
        con                            = connectors.getConnector(2u);
        con->status                    = ChargePointStatus::SuspendedEV;
        con->last_notified_status      = ChargePointStatus::SuspendedEVSE;
        con->transaction_id            = 6789;
        con->transaction_id_offline    = 3456;
        con->transaction_start         = DateTime(987654321ll);
        con->transaction_id_tag        = "TAG5";
        con->transaction_parent_id_tag = "PARENT_TAG5";
        con->reservation_id            = 1234;
        con->reservation_id_tag        = "TAG6";
        con->reservation_parent_id_tag = "PARENT_TAG6";
        con->reservation_expiry_date   = DateTime(123456789ll);

        // Save connectors
        CHECK(connectors.saveConnector(0u));
        CHECK(connectors.saveConnector(1u));
        CHECK(connectors.saveConnector(2u));

        // Reload stored data
        Connectors connectors2(ocpp_config, database, timer_pool);
        connectors2.initDatabaseTable();

        // Check number of initialized connectors
        const std::vector<Connector*>& cons2 = connectors2.getConnectors();
        CHECK_EQ(connectors2.getCount(), 2u);
        CHECK_EQ(cons2.size(), 3u);

        // Check connectors
        con = connectors2.getConnector(0u);
        CHECK_NE(con, nullptr);
        CHECK(connectors2.isValid(0u));
        CHECK_EQ(con->id, 0u);
        CHECK_EQ(con->status, ChargePointStatus::Reserved);
        CHECK_EQ(con->last_notified_status, ChargePointStatus::Unavailable);
        CHECK_EQ(con->transaction_id, 1234);
        CHECK_EQ(con->transaction_id_offline, 5678);
        CHECK_EQ(con->transaction_start.timestamp(), 12345678ll);
        CHECK_EQ(con->transaction_id_tag, "TAG1");
        CHECK_EQ(con->transaction_parent_id_tag, "PARENT_TAG1");
        CHECK_EQ(con->reservation_id, 9876);
        CHECK_EQ(con->reservation_id_tag, "TAG2");
        CHECK_EQ(con->reservation_parent_id_tag, "PARENT_TAG2");
        CHECK_EQ(con->reservation_expiry_date.timestamp(), 87654321ll);

        con = connectors2.getConnector(1u);
        CHECK_NE(con, nullptr);
        CHECK(connectors2.isValid(1u));
        CHECK_EQ(con->id, 1u);
        CHECK_EQ(con->status, ChargePointStatus::Preparing);
        CHECK_EQ(con->last_notified_status, ChargePointStatus::Charging);
        CHECK_EQ(con->transaction_id, 5678);
        CHECK_EQ(con->transaction_id_offline, 1234);
        CHECK_EQ(con->transaction_start.timestamp(), 87654321ll);
        CHECK_EQ(con->transaction_id_tag, "TAG3");
        CHECK_EQ(con->transaction_parent_id_tag, "PARENT_TAG3");
        CHECK_EQ(con->reservation_id, 4567);
        CHECK_EQ(con->reservation_id_tag, "TAG4");
        CHECK_EQ(con->reservation_parent_id_tag, "PARENT_TAG4");
        CHECK_EQ(con->reservation_expiry_date.timestamp(), 12345678ll);

        con = connectors2.getConnector(2u);
        CHECK_NE(con, nullptr);
        CHECK(connectors2.isValid(2u));
        CHECK_EQ(con->id, 2u);
        CHECK_EQ(con->status, ChargePointStatus::SuspendedEV);
        CHECK_EQ(con->last_notified_status, ChargePointStatus::SuspendedEVSE);
        CHECK_EQ(con->transaction_id, 6789);
        CHECK_EQ(con->transaction_id_offline, 3456);
        CHECK_EQ(con->transaction_start.timestamp(), 987654321ll);
        CHECK_EQ(con->transaction_id_tag, "TAG5");
        CHECK_EQ(con->transaction_parent_id_tag, "PARENT_TAG5");
        CHECK_EQ(con->reservation_id, 1234);
        CHECK_EQ(con->reservation_id_tag, "TAG6");
        CHECK_EQ(con->reservation_parent_id_tag, "PARENT_TAG6");
        CHECK_EQ(con->reservation_expiry_date.timestamp(), 123456789ll);

        // Reset connectors data
        connectors2.resetConnectors();

        // Check connectors
        con = connectors2.getConnector(0u);
        CHECK_NE(con, nullptr);
        CHECK(connectors2.isValid(0u));
        CHECK_EQ(con->id, 0u);
        CHECK_EQ(con->status, ChargePointStatus::Available);
        CHECK_EQ(con->last_notified_status, ChargePointStatus::Available);
        CHECK_EQ(con->transaction_id, 0);
        CHECK_EQ(con->transaction_id_offline, 0);
        CHECK_EQ(con->transaction_start.timestamp(), 0);
        CHECK_EQ(con->transaction_id_tag, "");
        CHECK_EQ(con->transaction_parent_id_tag, "");
        CHECK_EQ(con->reservation_id, 0);
        CHECK_EQ(con->reservation_id_tag, "");
        CHECK_EQ(con->reservation_parent_id_tag, "");
        CHECK_EQ(con->reservation_expiry_date.timestamp(), 0);

        con = connectors2.getConnector(1u);
        CHECK_NE(con, nullptr);
        CHECK(connectors2.isValid(1u));
        CHECK_EQ(con->id, 1u);
        CHECK_EQ(con->status, ChargePointStatus::Available);
        CHECK_EQ(con->last_notified_status, ChargePointStatus::Available);
        CHECK_EQ(con->transaction_id, 0);
        CHECK_EQ(con->transaction_id_offline, 0);
        CHECK_EQ(con->transaction_start.timestamp(), 0);
        CHECK_EQ(con->transaction_id_tag, "");
        CHECK_EQ(con->transaction_parent_id_tag, "");
        CHECK_EQ(con->reservation_id, 0);
        CHECK_EQ(con->reservation_id_tag, "");
        CHECK_EQ(con->reservation_parent_id_tag, "");
        CHECK_EQ(con->reservation_expiry_date.timestamp(), 0);

        con = connectors2.getConnector(2u);
        CHECK_NE(con, nullptr);
        CHECK(connectors2.isValid(2u));
        CHECK_EQ(con->id, 2u);
        CHECK_EQ(con->status, ChargePointStatus::Available);
        CHECK_EQ(con->last_notified_status, ChargePointStatus::Available);
        CHECK_EQ(con->transaction_id, 0);
        CHECK_EQ(con->transaction_id_offline, 0);
        CHECK_EQ(con->transaction_start.timestamp(), 0);
        CHECK_EQ(con->transaction_id_tag, "");
        CHECK_EQ(con->transaction_parent_id_tag, "");
        CHECK_EQ(con->reservation_id, 0u);
        CHECK_EQ(con->reservation_id_tag, "");
        CHECK_EQ(con->reservation_parent_id_tag, "");
        CHECK_EQ(con->reservation_expiry_date.timestamp(), 0);

        con = connectors2.getConnector(3u);
        CHECK_EQ(con, nullptr);
        CHECK_FALSE(connectors2.isValid(3u));

        // Reload from database
        Connectors connectors3(ocpp_config, database, timer_pool);
        connectors3.initDatabaseTable();

        // Check number of initialized connectors
        const std::vector<Connector*>& cons3 = connectors.getConnectors();
        CHECK_EQ(connectors3.getCount(), 2u);
        CHECK_EQ(cons3.size(), 3u);

        // Check connectors
        con = connectors3.getConnector(0u);
        CHECK_NE(con, nullptr);
        CHECK(connectors3.isValid(0u));
        CHECK_EQ(con->id, 0u);
        CHECK_EQ(con->status, ChargePointStatus::Available);
        CHECK_EQ(con->last_notified_status, ChargePointStatus::Available);
        CHECK_EQ(con->transaction_id, 0);
        CHECK_EQ(con->transaction_id_offline, 0);
        CHECK_EQ(con->transaction_start.timestamp(), 0);
        CHECK_EQ(con->transaction_id_tag, "");
        CHECK_EQ(con->transaction_parent_id_tag, "");
        CHECK_EQ(con->reservation_id, 0);
        CHECK_EQ(con->reservation_id_tag, "");
        CHECK_EQ(con->reservation_parent_id_tag, "");
        CHECK_EQ(con->reservation_expiry_date.timestamp(), 0);

        con = connectors3.getConnector(1u);
        CHECK_NE(con, nullptr);
        CHECK(connectors3.isValid(1u));
        CHECK_EQ(con->id, 1u);
        CHECK_EQ(con->status, ChargePointStatus::Available);
        CHECK_EQ(con->last_notified_status, ChargePointStatus::Available);
        CHECK_EQ(con->transaction_id, 0);
        CHECK_EQ(con->transaction_id_offline, 0);
        CHECK_EQ(con->transaction_start.timestamp(), 0);
        CHECK_EQ(con->transaction_id_tag, "");
        CHECK_EQ(con->transaction_parent_id_tag, "");
        CHECK_EQ(con->reservation_id, 0);
        CHECK_EQ(con->reservation_id_tag, "");
        CHECK_EQ(con->reservation_parent_id_tag, "");
        CHECK_EQ(con->reservation_expiry_date.timestamp(), 0);

        con = connectors3.getConnector(2u);
        CHECK_NE(con, nullptr);
        CHECK(connectors3.isValid(2u));
        CHECK_EQ(con->id, 2u);
        CHECK_EQ(con->status, ChargePointStatus::Available);
        CHECK_EQ(con->last_notified_status, ChargePointStatus::Available);
        CHECK_EQ(con->transaction_id, 0);
        CHECK_EQ(con->transaction_id_offline, 0);
        CHECK_EQ(con->transaction_start.timestamp(), 0);
        CHECK_EQ(con->transaction_id_tag, "");
        CHECK_EQ(con->transaction_parent_id_tag, "");
        CHECK_EQ(con->reservation_id, 0u);
        CHECK_EQ(con->reservation_id_tag, "");
        CHECK_EQ(con->reservation_parent_id_tag, "");
        CHECK_EQ(con->reservation_expiry_date.timestamp(), 0);

        con = connectors3.getConnector(3u);
        CHECK_EQ(con, nullptr);
        CHECK_FALSE(connectors3.isValid(3u));

        // Update connector 0
        con                            = connectors3.getConnector(0u);
        con->status                    = ChargePointStatus::Reserved;
        con->last_notified_status      = ChargePointStatus::Unavailable;
        con->transaction_id            = 1234;
        con->transaction_id_offline    = 5678;
        con->transaction_start         = DateTime(12345678ll);
        con->transaction_id_tag        = "TAG1";
        con->transaction_parent_id_tag = "PARENT_TAG1";
        con->reservation_id            = 9876;
        con->reservation_id_tag        = "TAG2";
        con->reservation_parent_id_tag = "PARENT_TAG2";
        con->reservation_expiry_date   = DateTime(87654321ll);

        // Update connector 1
        con                            = connectors3.getConnector(1u);
        con->status                    = ChargePointStatus::Preparing;
        con->last_notified_status      = ChargePointStatus::Charging;
        con->transaction_id            = 5678;
        con->transaction_id_offline    = 1234;
        con->transaction_start         = DateTime(87654321ll);
        con->transaction_id_tag        = "TAG3";
        con->transaction_parent_id_tag = "PARENT_TAG3";
        con->reservation_id            = 4567;
        con->reservation_id_tag        = "TAG4";
        con->reservation_parent_id_tag = "PARENT_TAG4";
        con->reservation_expiry_date   = DateTime(12345678ll);

        // Update connector 2
        con                            = connectors3.getConnector(2u);
        con->status                    = ChargePointStatus::SuspendedEV;
        con->last_notified_status      = ChargePointStatus::SuspendedEVSE;
        con->transaction_id            = 6789;
        con->transaction_id_offline    = 3456;
        con->transaction_start         = DateTime(987654321ll);
        con->transaction_id_tag        = "TAG5";
        con->transaction_parent_id_tag = "PARENT_TAG5";
        con->reservation_id            = 1234;
        con->reservation_id_tag        = "TAG6";
        con->reservation_parent_id_tag = "PARENT_TAG6";
        con->reservation_expiry_date   = DateTime(123456789ll);

        // Save connectors
        CHECK(connectors3.saveConnector(0u));
        CHECK(connectors3.saveConnector(1u));
        CHECK(connectors3.saveConnector(2u));

        // Change number of connectors
        ocpp_config.setConfigValue("NumberOfConnectors", "1");

        Connectors connectors4(ocpp_config, database, timer_pool);
        connectors4.initDatabaseTable();

        // Check connectors
        con = connectors4.getConnector(0u);
        CHECK_NE(con, nullptr);
        CHECK(connectors4.isValid(0u));
        CHECK_EQ(con->id, 0u);
        CHECK_EQ(con->status, ChargePointStatus::Available);
        CHECK_EQ(con->last_notified_status, ChargePointStatus::Available);
        CHECK_EQ(con->transaction_id, 0);
        CHECK_EQ(con->transaction_id_offline, 0);
        CHECK_EQ(con->transaction_start.timestamp(), 0);
        CHECK_EQ(con->transaction_id_tag, "");
        CHECK_EQ(con->transaction_parent_id_tag, "");
        CHECK_EQ(con->reservation_id, 0);
        CHECK_EQ(con->reservation_id_tag, "");
        CHECK_EQ(con->reservation_parent_id_tag, "");
        CHECK_EQ(con->reservation_expiry_date.timestamp(), 0);

        con = connectors4.getConnector(1u);
        CHECK_NE(con, nullptr);
        CHECK(connectors4.isValid(1u));
        CHECK_EQ(con->id, 1u);
        CHECK_EQ(con->status, ChargePointStatus::Available);
        CHECK_EQ(con->last_notified_status, ChargePointStatus::Available);
        CHECK_EQ(con->transaction_id, 0);
        CHECK_EQ(con->transaction_id_offline, 0);
        CHECK_EQ(con->transaction_start.timestamp(), 0);
        CHECK_EQ(con->transaction_id_tag, "");
        CHECK_EQ(con->transaction_parent_id_tag, "");
        CHECK_EQ(con->reservation_id, 0);
        CHECK_EQ(con->reservation_id_tag, "");
        CHECK_EQ(con->reservation_parent_id_tag, "");
        CHECK_EQ(con->reservation_expiry_date.timestamp(), 0);

        con = connectors4.getConnector(2u);
        CHECK_EQ(con, nullptr);
        CHECK_FALSE(connectors4.isValid(2u));
    }

    TEST_CASE("Cleanup")
    {
        CHECK(database.close());
        std::filesystem::remove(DATABASE_PATH);
    }
}
