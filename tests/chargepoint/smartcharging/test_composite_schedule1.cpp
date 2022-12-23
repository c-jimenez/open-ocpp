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

TEST_SUITE("Get composite schedule - single OCPP profile")
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

    TEST_CASE("1 relative profile - no duration")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);

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
        CHECK(installProfile(1, profile1, smartcharging_mgr));

        ChargingSchedule schedule;
        DateTime         now = DateTime::now();
        CHECK(getCompositeSchedule(1, 3600, ChargingRateUnitType::A, schedule, smartcharging_mgr));

        CHECK_EQ(schedule.duration, 3600);
        CHECK_EQ(schedule.chargingRateUnit, ChargingRateUnitType::A);
        CHECK_GE(schedule.startSchedule.value(), now);
        CHECK_LE(schedule.startSchedule.value(), DateTime(now.timestamp() + 1));
        CHECK_EQ(schedule.chargingSchedulePeriod.size(), profile1.chargingSchedule.chargingSchedulePeriod.size());

        for (size_t i = 0; i < profile1.chargingSchedule.chargingSchedulePeriod.size(); i++)
        {
            CHECK_EQ(schedule.chargingSchedulePeriod[i].startPeriod, profile1.chargingSchedule.chargingSchedulePeriod[i].startPeriod);
            CHECK_EQ(schedule.chargingSchedulePeriod[i].limit, profile1.chargingSchedule.chargingSchedulePeriod[i].limit);
            CHECK_EQ(schedule.chargingSchedulePeriod[i].numberPhases, profile1.chargingSchedule.chargingSchedulePeriod[i].numberPhases);
        }
    }

    TEST_CASE("1 relative profile - no duration - rate unit conversion")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);

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
        CHECK(installProfile(1, profile1, smartcharging_mgr));

        ChargingSchedule schedule;
        DateTime         now = DateTime::now();
        CHECK(getCompositeSchedule(1, 3600, ChargingRateUnitType::W, schedule, smartcharging_mgr));

        CHECK_EQ(schedule.duration, 3600);
        CHECK_EQ(schedule.chargingRateUnit, ChargingRateUnitType::W);
        CHECK_GE(schedule.startSchedule.value(), now);
        CHECK_LE(schedule.startSchedule.value(), DateTime(now.timestamp() + 1));
        CHECK_EQ(schedule.chargingSchedulePeriod.size(), profile1.chargingSchedule.chargingSchedulePeriod.size());

        for (size_t i = 0; i < profile1.chargingSchedule.chargingSchedulePeriod.size(); i++)
        {
            CHECK_EQ(schedule.chargingSchedulePeriod[i].startPeriod, profile1.chargingSchedule.chargingSchedulePeriod[i].startPeriod);
            CHECK_EQ(schedule.chargingSchedulePeriod[i].limit,
                     stack_config.operatingVoltage() * profile1.chargingSchedule.chargingSchedulePeriod[i].limit *
                         profile1.chargingSchedule.chargingSchedulePeriod[i].numberPhases);
            CHECK_EQ(schedule.chargingSchedulePeriod[i].numberPhases, profile1.chargingSchedule.chargingSchedulePeriod[i].numberPhases);
        }
    }

    TEST_CASE("1 relative profile - duration less than asked")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);

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
        profile1.chargingSchedule.duration         = 2500;
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        CHECK(installProfile(1, profile1, smartcharging_mgr));

        ChargingSchedule schedule;
        DateTime         now = DateTime::now();
        CHECK(getCompositeSchedule(1, 3600, ChargingRateUnitType::A, schedule, smartcharging_mgr));

        CHECK_EQ(schedule.duration, 2500);
        CHECK_EQ(schedule.chargingRateUnit, ChargingRateUnitType::A);
        CHECK_GE(schedule.startSchedule.value(), now);
        CHECK_LE(schedule.startSchedule.value(), DateTime(now.timestamp() + 1));
        CHECK_EQ(schedule.chargingSchedulePeriod.size(), profile1.chargingSchedule.chargingSchedulePeriod.size());

        for (size_t i = 0; i < profile1.chargingSchedule.chargingSchedulePeriod.size(); i++)
        {
            CHECK_EQ(schedule.chargingSchedulePeriod[i].startPeriod, profile1.chargingSchedule.chargingSchedulePeriod[i].startPeriod);
            CHECK_EQ(schedule.chargingSchedulePeriod[i].limit, profile1.chargingSchedule.chargingSchedulePeriod[i].limit);
            CHECK_EQ(schedule.chargingSchedulePeriod[i].numberPhases, profile1.chargingSchedule.chargingSchedulePeriod[i].numberPhases);
        }
    }

    TEST_CASE("1 relative profile - duration more than asked")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);

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
        profile1.chargingSchedule.duration         = 5000;
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        CHECK(installProfile(1, profile1, smartcharging_mgr));

        ChargingSchedule schedule;
        DateTime         now = DateTime::now();
        CHECK(getCompositeSchedule(1, 3600, ChargingRateUnitType::A, schedule, smartcharging_mgr));

        CHECK_EQ(schedule.duration, 3600);
        CHECK_EQ(schedule.chargingRateUnit, ChargingRateUnitType::A);
        CHECK_GE(schedule.startSchedule.value(), now);
        CHECK_LE(schedule.startSchedule.value(), DateTime(now.timestamp() + 1));
        CHECK_EQ(schedule.chargingSchedulePeriod.size(), profile1.chargingSchedule.chargingSchedulePeriod.size());

        for (size_t i = 0; i < profile1.chargingSchedule.chargingSchedulePeriod.size(); i++)
        {
            CHECK_EQ(schedule.chargingSchedulePeriod[i].startPeriod, profile1.chargingSchedule.chargingSchedulePeriod[i].startPeriod);
            CHECK_EQ(schedule.chargingSchedulePeriod[i].limit, profile1.chargingSchedule.chargingSchedulePeriod[i].limit);
            CHECK_EQ(schedule.chargingSchedulePeriod[i].numberPhases, profile1.chargingSchedule.chargingSchedulePeriod[i].numberPhases);
        }
    }

    TEST_CASE("1 absolute profile - after - no duration")
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
        profile1.chargingSchedule.startSchedule    = DateTime(now.timestamp() + 100);
        CHECK(installProfile(1, profile1, smartcharging_mgr));

        ChargingSchedule schedule;
        CHECK(getCompositeSchedule(1, 3600, ChargingRateUnitType::A, schedule, smartcharging_mgr));

        CHECK_EQ(schedule.duration, 3500);
        CHECK_EQ(schedule.chargingRateUnit, ChargingRateUnitType::A);
        CHECK_GE(schedule.startSchedule.value(), profile1.chargingSchedule.startSchedule.value());
        CHECK_LE(schedule.startSchedule.value(), DateTime(profile1.chargingSchedule.startSchedule.value().timestamp() + 1));
        CHECK_EQ(schedule.chargingSchedulePeriod.size(), profile1.chargingSchedule.chargingSchedulePeriod.size());

        for (size_t i = 0; i < profile1.chargingSchedule.chargingSchedulePeriod.size(); i++)
        {
            CHECK_EQ(schedule.chargingSchedulePeriod[i].startPeriod, profile1.chargingSchedule.chargingSchedulePeriod[i].startPeriod);
            CHECK_EQ(schedule.chargingSchedulePeriod[i].limit, profile1.chargingSchedule.chargingSchedulePeriod[i].limit);
            CHECK_EQ(schedule.chargingSchedulePeriod[i].numberPhases, profile1.chargingSchedule.chargingSchedulePeriod[i].numberPhases);
        }
    }

    TEST_CASE("1 absolute profile - before - no duration")
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
        profile1.chargingSchedule.startSchedule    = DateTime(now.timestamp() - 100);
        CHECK(installProfile(1, profile1, smartcharging_mgr));

        ChargingSchedule schedule;
        CHECK(getCompositeSchedule(1, 3600, ChargingRateUnitType::A, schedule, smartcharging_mgr));

        CHECK_EQ(schedule.duration, 3600);
        CHECK_EQ(schedule.chargingRateUnit, ChargingRateUnitType::A);
        CHECK_GE(schedule.startSchedule.value(), now);
        CHECK_LE(schedule.startSchedule.value(), DateTime(now.timestamp() + 1));
        CHECK_EQ(schedule.chargingSchedulePeriod.size(), profile1.chargingSchedule.chargingSchedulePeriod.size());

        CHECK_EQ(schedule.chargingSchedulePeriod[0].startPeriod, profile1.chargingSchedule.chargingSchedulePeriod[0].startPeriod);
        CHECK_EQ(schedule.chargingSchedulePeriod[0].limit, profile1.chargingSchedule.chargingSchedulePeriod[0].limit);
        CHECK_EQ(schedule.chargingSchedulePeriod[0].numberPhases, profile1.chargingSchedule.chargingSchedulePeriod[0].numberPhases);
        for (size_t i = 1; i < profile1.chargingSchedule.chargingSchedulePeriod.size(); i++)
        {
            CHECK_EQ(schedule.chargingSchedulePeriod[i].startPeriod, profile1.chargingSchedule.chargingSchedulePeriod[i].startPeriod - 100);
            CHECK_EQ(schedule.chargingSchedulePeriod[i].limit, profile1.chargingSchedule.chargingSchedulePeriod[i].limit);
            CHECK_EQ(schedule.chargingSchedulePeriod[i].numberPhases, profile1.chargingSchedule.chargingSchedulePeriod[i].numberPhases);
        }
    }

    TEST_CASE("1 absolute profile - before - duration less than asked")
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
        profile1.chargingSchedule.startSchedule    = DateTime(now.timestamp() - 100);
        profile1.chargingSchedule.duration         = 2000;
        CHECK(installProfile(1, profile1, smartcharging_mgr));

        ChargingSchedule schedule;
        CHECK(getCompositeSchedule(1, 3600, ChargingRateUnitType::A, schedule, smartcharging_mgr));

        CHECK_EQ(schedule.duration, 1900);
        CHECK_EQ(schedule.chargingRateUnit, ChargingRateUnitType::A);
        CHECK_GE(schedule.startSchedule.value(), now);
        CHECK_LE(schedule.startSchedule.value(), DateTime(now.timestamp() + 1));
        CHECK_EQ(schedule.chargingSchedulePeriod.size(), profile1.chargingSchedule.chargingSchedulePeriod.size());

        CHECK_EQ(schedule.chargingSchedulePeriod[0].startPeriod, profile1.chargingSchedule.chargingSchedulePeriod[0].startPeriod);
        CHECK_EQ(schedule.chargingSchedulePeriod[0].limit, profile1.chargingSchedule.chargingSchedulePeriod[0].limit);
        CHECK_EQ(schedule.chargingSchedulePeriod[0].numberPhases, profile1.chargingSchedule.chargingSchedulePeriod[0].numberPhases);
        for (size_t i = 1; i < profile1.chargingSchedule.chargingSchedulePeriod.size(); i++)
        {
            CHECK_EQ(schedule.chargingSchedulePeriod[i].startPeriod, profile1.chargingSchedule.chargingSchedulePeriod[i].startPeriod - 100);
            CHECK_EQ(schedule.chargingSchedulePeriod[i].limit, profile1.chargingSchedule.chargingSchedulePeriod[i].limit);
            CHECK_EQ(schedule.chargingSchedulePeriod[i].numberPhases, profile1.chargingSchedule.chargingSchedulePeriod[i].numberPhases);
        }
    }

    TEST_CASE("Cleanup")
    {
        CHECK(database.close());
        std::filesystem::remove(DATABASE_PATH);
    }
}
