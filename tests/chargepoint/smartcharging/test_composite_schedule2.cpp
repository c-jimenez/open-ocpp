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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "ChargePointConfigStub.h"
#include "ChargePointEventsHandlerStub.h"
#include "Connectors.h"
#include "Database.h"
#include "GenericMessageSender.h"
#include "MessageDispatcherStub.h"
#include "MessagesConverter.h"
#include "MessagesValidator.h"
#include "OcppConfigStub.h"
#include "RpcStub.h"
#include "SmartChargingManager.h"
#include "TestableTimerPool.h"
#include "TestableWorkerThreadPool.h"
#include "doctest.h"

#include <filesystem>

using namespace ocpp::chargepoint;
using namespace ocpp::config;
using namespace ocpp::database;
using namespace ocpp::messages;
using namespace ocpp::types;
using namespace ocpp::rpc;
using namespace ocpp::helpers;

static constexpr const char* DATABASE_PATH = "/tmp/test.db";

Database                     database;
OcppConfigStub               ocpp_config;
ChargePointConfigStub        stack_config;
ChargePointEventsHandlerStub event_handler;
TestableWorkerThreadPool     worker_pool;
RpcStub                      rpc;
MessagesValidator            msgs_validator;
MessagesConverter            msgs_converter;
MessageDispatcherStub        msg_dispatcher;
GenericMessageSender         msg_sender(rpc, msgs_converter, msgs_validator, std::chrono::milliseconds(1000));
TestableTimerPool            timer_pool;
Connectors                   connectors(ocpp_config, database, timer_pool);

TEST_SUITE("Get composite schedule - multiple OCPP profiles")
{
    TEST_CASE("Setup")
    {
        std::filesystem::remove(DATABASE_PATH);
        CHECK(database.open(DATABASE_PATH));

        stack_config.setConfigValue("OperatingVoltage", "230");
        ocpp_config.setConfigValue("NumberOfConnectors", "2");
        ocpp_config.setConfigValue("ChargingScheduleMaxPeriods", "10");
        ocpp_config.setConfigValue("ChargeProfileMaxStackLevel", "10");
        ocpp_config.setConfigValue("MaxChargingProfilesInstalled", "10");
        ocpp_config.setConfigValue("ChargingScheduleAllowedChargingRateUnit", "Current,Power");

        connectors.initDatabaseTable();
    }

    bool installProfile(unsigned int connector_id, const ChargingProfile& profile, SmartChargingManager& smartcharging_mgr)
    {
        SetChargingProfileReq setprofile_req;
        setprofile_req.connectorId        = connector_id;
        setprofile_req.csChargingProfiles = profile;
        SetChargingProfileConf setprofile_conf;
        setprofile_conf.status = ChargingProfileStatus::Rejected;
        std::string error_code;
        std::string error_message;
        smartcharging_mgr.handleMessage(setprofile_req, setprofile_conf, error_code, error_message);
        return (setprofile_conf.status == ChargingProfileStatus::Accepted);
    }

    bool getCompositeSchedule(unsigned int          connector_id,
                              unsigned int          duration,
                              ChargingRateUnitType  rate_unit,
                              ChargingSchedule&     schedule,
                              SmartChargingManager& smartcharging_mgr)
    {
        GetCompositeScheduleReq getcomposite_req;
        getcomposite_req.connectorId      = connector_id;
        getcomposite_req.duration         = duration;
        getcomposite_req.chargingRateUnit = rate_unit;
        GetCompositeScheduleConf getcomposite_conf;
        std::string              error_code;
        std::string              error_message;
        smartcharging_mgr.handleMessage(getcomposite_req, getcomposite_conf, error_code, error_message);
        schedule = getcomposite_conf.chargingSchedule;
        return (getcomposite_conf.status == GetCompositeScheduleStatus::Accepted);
    }

    TEST_CASE("2 profiles - overlapping before")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);

        DateTime now = DateTime::now();

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Absolute;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 10.f;
        charging_period.startPeriod  = 1000;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 32.f;
        charging_period.startPeriod  = 1700;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.startSchedule    = DateTime(now.timestamp() + 300);
        profile1.chargingSchedule.duration         = 2000;
        CHECK(installProfile(1, profile1, smartcharging_mgr));

        ChargingProfile profile2;
        profile2.chargingProfileId      = 2;
        profile2.stackLevel             = 4;
        profile2.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile2.chargingProfileKind    = ChargingProfileKindType::Relative;

        charging_period.limit        = 8.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 2;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 20.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 18.f;
        charging_period.startPeriod  = 500;
        charging_period.numberPhases = 3;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile2.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        CHECK(installProfile(1, profile2, smartcharging_mgr));

        ChargingSchedule schedule;
        CHECK(getCompositeSchedule(1, 3600, ChargingRateUnitType::A, schedule, smartcharging_mgr));

        CHECK_EQ(schedule.duration, 3600);
        CHECK_EQ(schedule.chargingRateUnit, ChargingRateUnitType::A);
        CHECK_GE(schedule.startSchedule.value(), now);
        CHECK_LE(schedule.startSchedule.value(), DateTime(now.timestamp() + 1));
        CHECK_EQ(schedule.chargingSchedulePeriod.size(), 6);

        CHECK_EQ(schedule.chargingSchedulePeriod[0].startPeriod, 0);
        CHECK_EQ(schedule.chargingSchedulePeriod[0].limit, 8.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[0].numberPhases.value(), 2);
        CHECK_EQ(schedule.chargingSchedulePeriod[1].startPeriod, 200);
        CHECK_EQ(schedule.chargingSchedulePeriod[1].limit, 20.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[1].numberPhases.value(), 3);
        CHECK_EQ(schedule.chargingSchedulePeriod[2].startPeriod, 300);
        CHECK_EQ(schedule.chargingSchedulePeriod[2].limit, 16.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[2].numberPhases.value(), 1);
        CHECK_EQ(schedule.chargingSchedulePeriod[3].startPeriod, 1300);
        CHECK_EQ(schedule.chargingSchedulePeriod[3].limit, 10.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[3].numberPhases.value(), 2);
        CHECK_EQ(schedule.chargingSchedulePeriod[4].startPeriod, 2000);
        CHECK_EQ(schedule.chargingSchedulePeriod[4].limit, 32.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[4].numberPhases.value(), 3);
        CHECK_EQ(schedule.chargingSchedulePeriod[5].startPeriod, 2300);
        CHECK_EQ(schedule.chargingSchedulePeriod[5].limit, 18.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[5].numberPhases.value(), 3);
    }

    TEST_CASE("2 profiles - contiguous before")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);

        DateTime now = DateTime::now();

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Absolute;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 10.f;
        charging_period.startPeriod  = 1000;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 32.f;
        charging_period.startPeriod  = 1700;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.startSchedule    = DateTime(now.timestamp() + 300);
        profile1.chargingSchedule.duration         = 2000;
        CHECK(installProfile(1, profile1, smartcharging_mgr));

        ChargingProfile profile2;
        profile2.chargingProfileId      = 2;
        profile2.stackLevel             = 4;
        profile2.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile2.chargingProfileKind    = ChargingProfileKindType::Relative;

        charging_period.limit        = 8.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 2;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 20.f;
        charging_period.startPeriod  = 300;
        charging_period.numberPhases = 3;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 18.f;
        charging_period.startPeriod  = 500;
        charging_period.numberPhases = 3;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile2.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        CHECK(installProfile(1, profile2, smartcharging_mgr));

        ChargingSchedule schedule;
        CHECK(getCompositeSchedule(1, 3600, ChargingRateUnitType::A, schedule, smartcharging_mgr));

        CHECK_EQ(schedule.duration, 3600);
        CHECK_EQ(schedule.chargingRateUnit, ChargingRateUnitType::A);
        CHECK_GE(schedule.startSchedule.value(), now);
        CHECK_LE(schedule.startSchedule.value(), DateTime(now.timestamp() + 1));
        CHECK_EQ(schedule.chargingSchedulePeriod.size(), 5);

        CHECK_EQ(schedule.chargingSchedulePeriod[0].startPeriod, 0);
        CHECK_EQ(schedule.chargingSchedulePeriod[0].limit, 8.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[0].numberPhases.value(), 2);
        CHECK_EQ(schedule.chargingSchedulePeriod[1].startPeriod, 300);
        CHECK_EQ(schedule.chargingSchedulePeriod[1].limit, 16.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[1].numberPhases.value(), 1);
        CHECK_EQ(schedule.chargingSchedulePeriod[2].startPeriod, 1300);
        CHECK_EQ(schedule.chargingSchedulePeriod[2].limit, 10.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[2].numberPhases.value(), 2);
        CHECK_EQ(schedule.chargingSchedulePeriod[3].startPeriod, 2000);
        CHECK_EQ(schedule.chargingSchedulePeriod[3].limit, 32.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[3].numberPhases.value(), 3);
        CHECK_EQ(schedule.chargingSchedulePeriod[4].startPeriod, 2300);
        CHECK_EQ(schedule.chargingSchedulePeriod[4].limit, 18.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[4].numberPhases.value(), 3);
    }

    TEST_CASE("2 profiles - overlapping after")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);

        DateTime now = DateTime::now();

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Relative;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 10.f;
        charging_period.startPeriod  = 1000;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 32.f;
        charging_period.startPeriod  = 1700;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.duration         = 2000;
        CHECK(installProfile(1, profile1, smartcharging_mgr));

        ChargingProfile profile2;
        profile2.chargingProfileId      = 2;
        profile2.stackLevel             = 4;
        profile2.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile2.chargingProfileKind    = ChargingProfileKindType::Absolute;

        charging_period.limit        = 8.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 2;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 20.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 1;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 18.f;
        charging_period.startPeriod  = 500;
        charging_period.numberPhases = 3;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile2.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile2.chargingSchedule.startSchedule    = DateTime(now.timestamp() + 1600);
        CHECK(installProfile(1, profile2, smartcharging_mgr));

        ChargingSchedule schedule;
        CHECK(getCompositeSchedule(1, 3600, ChargingRateUnitType::A, schedule, smartcharging_mgr));

        CHECK_EQ(schedule.duration, 3600);
        CHECK_EQ(schedule.chargingRateUnit, ChargingRateUnitType::A);
        CHECK_GE(schedule.startSchedule.value(), now);
        CHECK_LE(schedule.startSchedule.value(), DateTime(now.timestamp() + 1));
        CHECK_EQ(schedule.chargingSchedulePeriod.size(), 5);

        CHECK_EQ(schedule.chargingSchedulePeriod[0].startPeriod, 0);
        CHECK_EQ(schedule.chargingSchedulePeriod[0].limit, 16.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[0].numberPhases.value(), 1);
        CHECK_EQ(schedule.chargingSchedulePeriod[1].startPeriod, 1000);
        CHECK_EQ(schedule.chargingSchedulePeriod[1].limit, 10.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[1].numberPhases.value(), 2);
        CHECK_EQ(schedule.chargingSchedulePeriod[2].startPeriod, 1700);
        CHECK_EQ(schedule.chargingSchedulePeriod[2].limit, 32.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[2].numberPhases.value(), 3);
        CHECK_EQ(schedule.chargingSchedulePeriod[3].startPeriod, 2000);
        CHECK_EQ(schedule.chargingSchedulePeriod[3].limit, 20.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[3].numberPhases.value(), 1);
        CHECK_EQ(schedule.chargingSchedulePeriod[4].startPeriod, 2100);
        CHECK_EQ(schedule.chargingSchedulePeriod[4].limit, 18.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[4].numberPhases.value(), 3);
    }

    TEST_CASE("2 profiles - contiguous after")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);

        DateTime now = DateTime::now();

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Relative;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 10.f;
        charging_period.startPeriod  = 1000;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 32.f;
        charging_period.startPeriod  = 1700;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.duration         = 2000;
        CHECK(installProfile(1, profile1, smartcharging_mgr));

        ChargingProfile profile2;
        profile2.chargingProfileId      = 2;
        profile2.stackLevel             = 4;
        profile2.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile2.chargingProfileKind    = ChargingProfileKindType::Absolute;

        charging_period.limit        = 8.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 2;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 20.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 1;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 18.f;
        charging_period.startPeriod  = 500;
        charging_period.numberPhases = 3;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile2.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile2.chargingSchedule.startSchedule    = DateTime(now.timestamp() + 1800);
        CHECK(installProfile(1, profile2, smartcharging_mgr));

        ChargingSchedule schedule;
        CHECK(getCompositeSchedule(1, 3600, ChargingRateUnitType::A, schedule, smartcharging_mgr));

        CHECK_EQ(schedule.duration, 3600);
        CHECK_EQ(schedule.chargingRateUnit, ChargingRateUnitType::A);
        CHECK_GE(schedule.startSchedule.value(), now);
        CHECK_LE(schedule.startSchedule.value(), DateTime(now.timestamp() + 1));
        CHECK_EQ(schedule.chargingSchedulePeriod.size(), 5);

        CHECK_EQ(schedule.chargingSchedulePeriod[0].startPeriod, 0);
        CHECK_EQ(schedule.chargingSchedulePeriod[0].limit, 16.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[0].numberPhases.value(), 1);
        CHECK_EQ(schedule.chargingSchedulePeriod[1].startPeriod, 1000);
        CHECK_EQ(schedule.chargingSchedulePeriod[1].limit, 10.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[1].numberPhases.value(), 2);
        CHECK_EQ(schedule.chargingSchedulePeriod[2].startPeriod, 1700);
        CHECK_EQ(schedule.chargingSchedulePeriod[2].limit, 32.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[2].numberPhases.value(), 3);
        CHECK_EQ(schedule.chargingSchedulePeriod[3].startPeriod, 2000);
        CHECK_EQ(schedule.chargingSchedulePeriod[3].limit, 20.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[3].numberPhases.value(), 1);
        CHECK_EQ(schedule.chargingSchedulePeriod[4].startPeriod, 2300);
        CHECK_EQ(schedule.chargingSchedulePeriod[4].limit, 18.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[4].numberPhases.value(), 3);
    }

    TEST_CASE("2 profiles - reference + 1 period overlapping")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);

        DateTime now = DateTime::now();

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Absolute;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 10.f;
        charging_period.startPeriod  = 1000;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 32.f;
        charging_period.startPeriod  = 1700;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.startSchedule    = DateTime(now.timestamp() + 200);
        profile1.chargingSchedule.duration         = 2000;
        CHECK(installProfile(1, profile1, smartcharging_mgr));

        ChargingProfile profile2;
        profile2.chargingProfileId      = 2;
        profile2.stackLevel             = 4;
        profile2.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile2.chargingProfileKind    = ChargingProfileKindType::Relative;

        charging_period.limit        = 8.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 2;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile2.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        CHECK(installProfile(1, profile2, smartcharging_mgr));

        ChargingSchedule schedule;
        CHECK(getCompositeSchedule(1, 3600, ChargingRateUnitType::A, schedule, smartcharging_mgr));

        CHECK_EQ(schedule.duration, 3600);
        CHECK_EQ(schedule.chargingRateUnit, ChargingRateUnitType::A);
        CHECK_GE(schedule.startSchedule.value(), now);
        CHECK_LE(schedule.startSchedule.value(), DateTime(now.timestamp() + 1));
        CHECK_EQ(schedule.chargingSchedulePeriod.size(), 5);

        CHECK_EQ(schedule.chargingSchedulePeriod[0].startPeriod, 0);
        CHECK_EQ(schedule.chargingSchedulePeriod[0].limit, 8.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[0].numberPhases.value(), 2);
        CHECK_EQ(schedule.chargingSchedulePeriod[1].startPeriod, 200);
        CHECK_EQ(schedule.chargingSchedulePeriod[1].limit, 16.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[1].numberPhases.value(), 1);
        CHECK_EQ(schedule.chargingSchedulePeriod[2].startPeriod, 1200);
        CHECK_EQ(schedule.chargingSchedulePeriod[2].limit, 10.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[2].numberPhases.value(), 2);
        CHECK_EQ(schedule.chargingSchedulePeriod[3].startPeriod, 1900);
        CHECK_EQ(schedule.chargingSchedulePeriod[3].limit, 32.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[4].startPeriod, 2200);
        CHECK_EQ(schedule.chargingSchedulePeriod[4].limit, 8.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[4].numberPhases.value(), 2);
    }

    TEST_CASE("2 profiles - reference + 1 period same start")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);

        DateTime now = DateTime::now();

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Relative;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 10.f;
        charging_period.startPeriod  = 1000;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 32.f;
        charging_period.startPeriod  = 1700;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.duration         = 2000;
        CHECK(installProfile(1, profile1, smartcharging_mgr));

        ChargingProfile profile2;
        profile2.chargingProfileId      = 2;
        profile2.stackLevel             = 4;
        profile2.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile2.chargingProfileKind    = ChargingProfileKindType::Relative;

        charging_period.limit        = 8.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 2;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile2.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        CHECK(installProfile(1, profile2, smartcharging_mgr));

        ChargingSchedule schedule;
        CHECK(getCompositeSchedule(1, 3600, ChargingRateUnitType::A, schedule, smartcharging_mgr));

        CHECK_EQ(schedule.duration, 3600);
        CHECK_EQ(schedule.chargingRateUnit, ChargingRateUnitType::A);
        CHECK_GE(schedule.startSchedule.value(), now);
        CHECK_LE(schedule.startSchedule.value(), DateTime(now.timestamp() + 1));
        CHECK_EQ(schedule.chargingSchedulePeriod.size(), 4);

        CHECK_EQ(schedule.chargingSchedulePeriod[0].startPeriod, 0);
        CHECK_EQ(schedule.chargingSchedulePeriod[0].limit, 16.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[0].numberPhases.value(), 1);
        CHECK_EQ(schedule.chargingSchedulePeriod[1].startPeriod, 1000);
        CHECK_EQ(schedule.chargingSchedulePeriod[1].limit, 10.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[1].numberPhases.value(), 2);
        CHECK_EQ(schedule.chargingSchedulePeriod[2].startPeriod, 1700);
        CHECK_EQ(schedule.chargingSchedulePeriod[2].limit, 32.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[3].startPeriod, 2000);
        CHECK_EQ(schedule.chargingSchedulePeriod[3].limit, 8.f);
        CHECK_EQ(schedule.chargingSchedulePeriod[3].numberPhases.value(), 2);
    }

    TEST_CASE("2 profiles - reference + non contiguous before")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);

        DateTime now = DateTime::now();

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Absolute;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 10.f;
        charging_period.startPeriod  = 1000;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 32.f;
        charging_period.startPeriod  = 1700;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.startSchedule    = DateTime(now.timestamp() + 200);
        CHECK(installProfile(1, profile1, smartcharging_mgr));

        ChargingProfile profile2;
        profile2.chargingProfileId      = 2;
        profile2.stackLevel             = 4;
        profile2.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile2.chargingProfileKind    = ChargingProfileKindType::Relative;

        charging_period.limit        = 8.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 2;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile2.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile2.chargingSchedule.duration         = 100;
        CHECK(installProfile(1, profile2, smartcharging_mgr));

        ChargingSchedule schedule;
        CHECK_FALSE(getCompositeSchedule(1, 3600, ChargingRateUnitType::A, schedule, smartcharging_mgr));
    }

    TEST_CASE("2 profiles - reference + non contiguous after")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);

        DateTime now = DateTime::now();

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Relative;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 10.f;
        charging_period.startPeriod  = 1000;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 32.f;
        charging_period.startPeriod  = 1700;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.duration         = 2000;
        CHECK(installProfile(1, profile1, smartcharging_mgr));

        ChargingProfile profile2;
        profile2.chargingProfileId      = 2;
        profile2.stackLevel             = 4;
        profile2.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile2.chargingProfileKind    = ChargingProfileKindType::Absolute;

        charging_period.limit        = 8.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 2;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile2.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile2.chargingSchedule.startSchedule    = DateTime(now.timestamp() + 2100);
        CHECK(installProfile(1, profile2, smartcharging_mgr));

        ChargingSchedule schedule;
        CHECK_FALSE(getCompositeSchedule(1, 3600, ChargingRateUnitType::A, schedule, smartcharging_mgr));
    }

    TEST_CASE("Cleanup")
    {
        CHECK(database.close());
        std::filesystem::remove(DATABASE_PATH);
    }
}
