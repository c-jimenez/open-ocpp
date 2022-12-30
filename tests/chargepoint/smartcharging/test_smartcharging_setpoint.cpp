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

    void clearAllProfiles(SmartChargingManager & smartcharging_mgr)
    {
        ClearChargingProfileReq  clearprofiles_req;
        ClearChargingProfileConf clearprofiles_conf;
        std::string              error_code;
        std::string              error_message;
        smartcharging_mgr.handleMessage(clearprofiles_req, clearprofiles_conf, error_code, error_message);
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

    TEST_CASE("1 profiles (Recurring - daily) - 1 TxDefaultProfile connector 0 (active)")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, event_handler, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);
        clearAllProfiles(smartcharging_mgr);

        DateTime now = DateTime::now();

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Recurring;
        profile1.recurrencyKind         = RecurrencyKindType::Daily;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 5.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 13.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 25.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.startSchedule    = DateTime(now.timestamp() - 100);
        profile1.chargingSchedule.minChargingRate  = 10.f;
        profile1.chargingSchedule.duration         = 300;
        CHECK(installProfile(0, profile1, smartcharging_mgr));

        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> charge_point_setpoint;
        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> connector_setpoint;
        CHECK(smartcharging_mgr.getSetpoint(1u, charge_point_setpoint, connector_setpoint, ChargingRateUnitType::A));

        CHECK_FALSE(charge_point_setpoint.isSet());

        CHECK(connector_setpoint.isSet());
        CHECK_EQ(connector_setpoint.value().value, 13.f);
        CHECK_EQ(connector_setpoint.value().number_phases, 2);
        CHECK(connector_setpoint.value().min_charging_rate.isSet());
        CHECK_EQ(connector_setpoint.value().min_charging_rate.value(), 10.f);
    }

    TEST_CASE("1 profiles (Recurring - daily) - 1 TxDefaultProfile connector 0 (inactive)")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, event_handler, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);
        clearAllProfiles(smartcharging_mgr);

        DateTime now = DateTime::now();

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Recurring;
        profile1.recurrencyKind         = RecurrencyKindType::Daily;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 5.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 13.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 25.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.startSchedule    = DateTime(now.timestamp() + 100);
        profile1.chargingSchedule.minChargingRate  = 10.f;
        profile1.chargingSchedule.duration         = 300;
        CHECK(installProfile(0, profile1, smartcharging_mgr));

        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> charge_point_setpoint;
        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> connector_setpoint;
        CHECK(smartcharging_mgr.getSetpoint(1u, charge_point_setpoint, connector_setpoint, ChargingRateUnitType::A));

        CHECK_FALSE(charge_point_setpoint.isSet());
        CHECK_FALSE(connector_setpoint.isSet());
    }

    TEST_CASE("1 profiles (Recurring - weekly) - 1 TxDefaultProfile connector 0 (active)")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, event_handler, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);
        clearAllProfiles(smartcharging_mgr);

        DateTime now = DateTime::now();

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Recurring;
        profile1.recurrencyKind         = RecurrencyKindType::Weekly;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 5.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 13.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 25.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.startSchedule    = DateTime(now.timestamp() - 100);
        profile1.chargingSchedule.minChargingRate  = 10.f;
        profile1.chargingSchedule.duration         = 300;
        CHECK(installProfile(0, profile1, smartcharging_mgr));

        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> charge_point_setpoint;
        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> connector_setpoint;
        CHECK(smartcharging_mgr.getSetpoint(1u, charge_point_setpoint, connector_setpoint, ChargingRateUnitType::A));

        CHECK_FALSE(charge_point_setpoint.isSet());

        CHECK(connector_setpoint.isSet());
        CHECK_EQ(connector_setpoint.value().value, 13.f);
        CHECK_EQ(connector_setpoint.value().number_phases, 2);
        CHECK(connector_setpoint.value().min_charging_rate.isSet());
        CHECK_EQ(connector_setpoint.value().min_charging_rate.value(), 10.f);
    }

    TEST_CASE("1 profiles (Recurring - weekly) - 1 TxDefaultProfile connector 0 (inactive)")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, event_handler, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);
        clearAllProfiles(smartcharging_mgr);

        DateTime now = DateTime::now();

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Recurring;
        profile1.recurrencyKind         = RecurrencyKindType::Weekly;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 5.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 13.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 25.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.startSchedule    = DateTime(now.timestamp() - 100 - 86400); // Yesterday
        profile1.chargingSchedule.minChargingRate  = 10.f;
        profile1.chargingSchedule.duration         = 300;
        CHECK(installProfile(0, profile1, smartcharging_mgr));

        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> charge_point_setpoint;
        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> connector_setpoint;
        CHECK(smartcharging_mgr.getSetpoint(1u, charge_point_setpoint, connector_setpoint, ChargingRateUnitType::A));

        CHECK_FALSE(charge_point_setpoint.isSet());
        CHECK_FALSE(connector_setpoint.isSet());
    }

    TEST_CASE("2 profiles (Absolute) - 1 TxDefaultProfile connector 0 + 1 TxDefaultProfile connector 1")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, event_handler, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);
        clearAllProfiles(smartcharging_mgr);

        DateTime now = DateTime::now();

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Absolute;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 5.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 13.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 25.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.startSchedule    = DateTime(now.timestamp() - 100);
        profile1.chargingSchedule.minChargingRate  = 10.f;
        CHECK(installProfile(0, profile1, smartcharging_mgr));

        ChargingProfile profile2;
        profile2.chargingProfileId      = 2;
        profile2.stackLevel             = 5;
        profile2.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile2.chargingProfileKind    = ChargingProfileKindType::Absolute;

        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 10.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile2.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile2.chargingSchedule.startSchedule    = DateTime(now.timestamp() - 100);
        profile2.chargingSchedule.minChargingRate  = 8.f;
        CHECK(installProfile(1, profile2, smartcharging_mgr));

        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> charge_point_setpoint;
        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> connector_setpoint;
        CHECK(smartcharging_mgr.getSetpoint(1u, charge_point_setpoint, connector_setpoint, ChargingRateUnitType::A));

        CHECK_FALSE(charge_point_setpoint.isSet());

        CHECK(connector_setpoint.isSet());
        CHECK_EQ(connector_setpoint.value().value, 10.f);
        CHECK_EQ(connector_setpoint.value().number_phases, 2);
        CHECK(connector_setpoint.value().min_charging_rate.isSet());
        CHECK_EQ(connector_setpoint.value().min_charging_rate.value(), 8.f);
    }

    TEST_CASE("2 profiles (Absolute) - 1 ChargePointMaxProfile (limitation) + 1 TxDefaultProfile connector 0")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, event_handler, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);
        clearAllProfiles(smartcharging_mgr);

        DateTime now = DateTime::now();

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Absolute;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 5.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 13.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 25.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.startSchedule    = DateTime(now.timestamp() - 100);
        profile1.chargingSchedule.minChargingRate  = 10.f;
        CHECK(installProfile(0, profile1, smartcharging_mgr));

        ChargingProfile profile2;
        profile2.chargingProfileId      = 3;
        profile2.stackLevel             = 4;
        profile2.chargingProfilePurpose = ChargingProfilePurposeType::ChargePointMaxProfile;
        profile2.chargingProfileKind    = ChargingProfileKindType::Absolute;

        charging_period.limit        = 8.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 20.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 64.f;
        charging_period.startPeriod  = 500;
        charging_period.numberPhases = 3;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile2.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile2.chargingSchedule.startSchedule    = DateTime(now.timestamp() - 100);
        profile2.chargingSchedule.minChargingRate  = 6.f;
        CHECK(installProfile(0, profile2, smartcharging_mgr));

        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> charge_point_setpoint;
        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> connector_setpoint;
        CHECK(smartcharging_mgr.getSetpoint(1u, charge_point_setpoint, connector_setpoint, ChargingRateUnitType::A));

        CHECK(charge_point_setpoint.isSet());
        CHECK_EQ(charge_point_setpoint.value().value, 8.f);
        CHECK_EQ(charge_point_setpoint.value().number_phases, 1);
        CHECK(charge_point_setpoint.value().min_charging_rate.isSet());
        CHECK_EQ(charge_point_setpoint.value().min_charging_rate.value(), 6.f);

        CHECK(connector_setpoint.isSet());
        CHECK_EQ(connector_setpoint.value().value, 8.f);
        CHECK_EQ(connector_setpoint.value().number_phases, 1);
        CHECK(connector_setpoint.value().min_charging_rate.isSet());
        CHECK_EQ(connector_setpoint.value().min_charging_rate.value(), 6.f);
    }

    TEST_CASE("2 profiles (Relative) - 1 TxDefaultProfile connector 0 + 1 TxDefaultProfile connector 1 (not valid from)")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, event_handler, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);
        clearAllProfiles(smartcharging_mgr);

        DateTime now = DateTime::now();

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Relative;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 5.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 13.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 25.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.minChargingRate  = 10.f;
        CHECK(installProfile(0, profile1, smartcharging_mgr));

        ChargingProfile profile2;
        profile2.chargingProfileId      = 2;
        profile2.stackLevel             = 5;
        profile2.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile2.chargingProfileKind    = ChargingProfileKindType::Relative;
        profile2.validFrom              = DateTime(now.timestamp() + 100);

        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 10.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile2.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile2.chargingSchedule.startSchedule    = DateTime(now.timestamp() - 100);
        profile2.chargingSchedule.minChargingRate  = 8.f;
        CHECK(installProfile(1, profile2, smartcharging_mgr));

        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> charge_point_setpoint;
        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> connector_setpoint;
        CHECK(smartcharging_mgr.getSetpoint(1u, charge_point_setpoint, connector_setpoint, ChargingRateUnitType::A));

        CHECK_FALSE(charge_point_setpoint.isSet());

        CHECK(connector_setpoint.isSet());
        CHECK_EQ(connector_setpoint.value().value, 5.f);
        CHECK_EQ(connector_setpoint.value().number_phases, 1);
        CHECK(connector_setpoint.value().min_charging_rate.isSet());
        CHECK_EQ(connector_setpoint.value().min_charging_rate.value(), 10.f);
    }

    TEST_CASE("2 profiles (Relative) - 1 TxDefaultProfile connector 0 + 1 TxDefaultProfile connector 1 (not valid to)")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, event_handler, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);
        clearAllProfiles(smartcharging_mgr);

        DateTime now = DateTime::now();

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Relative;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 5.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 13.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 25.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.minChargingRate  = 10.f;
        CHECK(installProfile(0, profile1, smartcharging_mgr));

        ChargingProfile profile2;
        profile2.chargingProfileId      = 2;
        profile2.stackLevel             = 5;
        profile2.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile2.chargingProfileKind    = ChargingProfileKindType::Relative;
        profile2.validTo                = DateTime(now.timestamp() - 100);

        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 10.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile2.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile2.chargingSchedule.startSchedule    = DateTime(now.timestamp() - 100);
        profile2.chargingSchedule.minChargingRate  = 8.f;
        CHECK(installProfile(1, profile2, smartcharging_mgr));

        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> charge_point_setpoint;
        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> connector_setpoint;
        CHECK(smartcharging_mgr.getSetpoint(1u, charge_point_setpoint, connector_setpoint, ChargingRateUnitType::A));

        CHECK_FALSE(charge_point_setpoint.isSet());

        CHECK(connector_setpoint.isSet());
        CHECK_EQ(connector_setpoint.value().value, 5.f);
        CHECK_EQ(connector_setpoint.value().number_phases, 1);
        CHECK(connector_setpoint.value().min_charging_rate.isSet());
        CHECK_EQ(connector_setpoint.value().min_charging_rate.value(), 10.f);
    }

    TEST_CASE("3 profiles (Absolute) - 1 ChargePointMaxProfile + 1 TxDefaultProfile connector 0 + 1 TxDefaultProfile connector 1")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, event_handler, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);
        clearAllProfiles(smartcharging_mgr);

        DateTime now = DateTime::now();

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Absolute;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 5.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 13.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 25.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.startSchedule    = DateTime(now.timestamp() - 100);
        profile1.chargingSchedule.minChargingRate  = 10.f;
        CHECK(installProfile(0, profile1, smartcharging_mgr));

        ChargingProfile profile2;
        profile2.chargingProfileId      = 2;
        profile2.stackLevel             = 5;
        profile2.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile2.chargingProfileKind    = ChargingProfileKindType::Absolute;

        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 10.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile2.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile2.chargingSchedule.startSchedule    = DateTime(now.timestamp() - 100);
        profile2.chargingSchedule.minChargingRate  = 8.f;
        CHECK(installProfile(1, profile2, smartcharging_mgr));

        ChargingProfile profile3;
        profile3.chargingProfileId      = 3;
        profile3.stackLevel             = 4;
        profile3.chargingProfilePurpose = ChargingProfilePurposeType::ChargePointMaxProfile;
        profile3.chargingProfileKind    = ChargingProfileKindType::Absolute;

        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 2;
        profile3.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 20.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile3.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 64.f;
        charging_period.startPeriod  = 500;
        charging_period.numberPhases = 3;
        profile3.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile3.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile3.chargingSchedule.startSchedule    = DateTime(now.timestamp() - 100);
        profile3.chargingSchedule.minChargingRate  = 6.f;
        CHECK(installProfile(0, profile3, smartcharging_mgr));

        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> charge_point_setpoint;
        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> connector_setpoint;
        CHECK(smartcharging_mgr.getSetpoint(1u, charge_point_setpoint, connector_setpoint, ChargingRateUnitType::A));

        CHECK(charge_point_setpoint.isSet());
        CHECK_EQ(charge_point_setpoint.value().value, 16.f);
        CHECK_EQ(charge_point_setpoint.value().number_phases, 2);
        CHECK(charge_point_setpoint.value().min_charging_rate.isSet());
        CHECK_EQ(charge_point_setpoint.value().min_charging_rate.value(), 6.f);

        CHECK(connector_setpoint.isSet());
        CHECK_EQ(connector_setpoint.value().value, 10.f);
        CHECK_EQ(connector_setpoint.value().number_phases, 2);
        CHECK(connector_setpoint.value().min_charging_rate.isSet());
        CHECK_EQ(connector_setpoint.value().min_charging_rate.value(), 8.f);
    }

    TEST_CASE(
        "3 profiles (Relative - no charge) - 1 ChargePointMaxProfile + 1 TxDefaultProfile connector 0 + 1 TxDefaultProfile connector 1")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, event_handler, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);
        clearAllProfiles(smartcharging_mgr);

        Connector* connector         = connectors.getConnector(1);
        connector->transaction_id    = 0;
        connector->transaction_start = 0;

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Relative;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 5.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 13.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 25.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.minChargingRate  = 10.f;
        CHECK(installProfile(0, profile1, smartcharging_mgr));

        ChargingProfile profile2;
        profile2.chargingProfileId      = 2;
        profile2.stackLevel             = 5;
        profile2.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile2.chargingProfileKind    = ChargingProfileKindType::Relative;

        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 10.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile2.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile2.chargingSchedule.minChargingRate  = 8.f;
        CHECK(installProfile(1, profile2, smartcharging_mgr));

        ChargingProfile profile3;
        profile3.chargingProfileId      = 3;
        profile3.stackLevel             = 4;
        profile3.chargingProfilePurpose = ChargingProfilePurposeType::ChargePointMaxProfile;
        profile3.chargingProfileKind    = ChargingProfileKindType::Relative;

        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 2;
        profile3.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 20.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile3.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 64.f;
        charging_period.startPeriod  = 500;
        charging_period.numberPhases = 3;
        profile3.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile3.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile3.chargingSchedule.minChargingRate  = 6.f;
        CHECK(installProfile(0, profile3, smartcharging_mgr));

        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> charge_point_setpoint;
        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> connector_setpoint;
        CHECK(smartcharging_mgr.getSetpoint(1u, charge_point_setpoint, connector_setpoint, ChargingRateUnitType::A));

        CHECK(charge_point_setpoint.isSet());
        CHECK_EQ(charge_point_setpoint.value().value, 16.f);
        CHECK_EQ(charge_point_setpoint.value().number_phases, 2);
        CHECK(charge_point_setpoint.value().min_charging_rate.isSet());
        CHECK_EQ(charge_point_setpoint.value().min_charging_rate.value(), 6.f);

        CHECK(connector_setpoint.isSet());
        CHECK_EQ(connector_setpoint.value().value, 16.f);
        CHECK_EQ(connector_setpoint.value().number_phases, 1);
        CHECK(connector_setpoint.value().min_charging_rate.isSet());
        CHECK_EQ(connector_setpoint.value().min_charging_rate.value(), 8.f);
    }

    TEST_CASE(
        "3 profiles (Relative - charging) - 1 ChargePointMaxProfile + 1 TxDefaultProfile connector 0 + 1 TxDefaultProfile connector 1")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, event_handler, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);
        clearAllProfiles(smartcharging_mgr);

        Connector* connector         = connectors.getConnector(1);
        connector->transaction_id    = 1;
        connector->transaction_start = DateTime(DateTime::now().timestamp() - 200);

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Relative;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 5.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 13.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 25.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.minChargingRate  = 10.f;
        CHECK(installProfile(0, profile1, smartcharging_mgr));

        ChargingProfile profile2;
        profile2.chargingProfileId      = 2;
        profile2.stackLevel             = 5;
        profile2.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile2.chargingProfileKind    = ChargingProfileKindType::Relative;

        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 10.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile2.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile2.chargingSchedule.minChargingRate  = 8.f;
        CHECK(installProfile(1, profile2, smartcharging_mgr));

        ChargingProfile profile3;
        profile3.chargingProfileId      = 3;
        profile3.stackLevel             = 4;
        profile3.chargingProfilePurpose = ChargingProfilePurposeType::ChargePointMaxProfile;
        profile3.chargingProfileKind    = ChargingProfileKindType::Relative;

        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 2;
        profile3.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 20.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile3.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 64.f;
        charging_period.startPeriod  = 500;
        charging_period.numberPhases = 3;
        profile3.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile3.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile3.chargingSchedule.minChargingRate  = 6.f;
        CHECK(installProfile(0, profile3, smartcharging_mgr));

        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> charge_point_setpoint;
        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> connector_setpoint;
        CHECK(smartcharging_mgr.getSetpoint(1u, charge_point_setpoint, connector_setpoint, ChargingRateUnitType::A));

        CHECK(charge_point_setpoint.isSet());
        CHECK_EQ(charge_point_setpoint.value().value, 20.f);
        CHECK_EQ(charge_point_setpoint.value().number_phases, 3);
        CHECK(charge_point_setpoint.value().min_charging_rate.isSet());
        CHECK_EQ(charge_point_setpoint.value().min_charging_rate.value(), 6.f);

        CHECK(connector_setpoint.isSet());
        CHECK_EQ(connector_setpoint.value().value, 16.f);
        CHECK_EQ(connector_setpoint.value().number_phases, 3);
        CHECK(connector_setpoint.value().min_charging_rate.isSet());
        CHECK_EQ(connector_setpoint.value().min_charging_rate.value(), 8.f);
    }

    TEST_CASE("3 profiles (Relative - charging) - 1 ChargePointMaxProfile + 1 TxProfile connector 1 + 1 TxDefaultProfile connector 1")
    {
        SmartChargingManager smartcharging_mgr(
            stack_config, ocpp_config, database, event_handler, timer_pool, worker_pool, connectors, msgs_converter, msg_dispatcher);
        clearAllProfiles(smartcharging_mgr);

        Connector* connector         = connectors.getConnector(1);
        connector->transaction_id    = 1;
        connector->transaction_start = DateTime(DateTime::now().timestamp() - 200);

        ChargingProfile profile1;
        profile1.chargingProfileId      = 1;
        profile1.stackLevel             = 5;
        profile1.chargingProfilePurpose = ChargingProfilePurposeType::TxProfile;
        profile1.chargingProfileKind    = ChargingProfileKindType::Relative;

        ChargingSchedulePeriod charging_period;
        charging_period.limit        = 5.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 13.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 25.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile1.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile1.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile1.chargingSchedule.minChargingRate  = 10.f;
        CHECK(installProfile(1, profile1, smartcharging_mgr));

        ChargingProfile profile2;
        profile2.chargingProfileId      = 2;
        profile2.stackLevel             = 5;
        profile2.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        profile2.chargingProfileKind    = ChargingProfileKindType::Relative;

        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 1;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 10.f;
        charging_period.startPeriod  = 50;
        charging_period.numberPhases = 2;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile2.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile2.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile2.chargingSchedule.minChargingRate  = 8.f;
        CHECK(installProfile(1, profile2, smartcharging_mgr));

        ChargingProfile profile3;
        profile3.chargingProfileId      = 3;
        profile3.stackLevel             = 4;
        profile3.chargingProfilePurpose = ChargingProfilePurposeType::ChargePointMaxProfile;
        profile3.chargingProfileKind    = ChargingProfileKindType::Relative;

        charging_period.limit        = 16.f;
        charging_period.startPeriod  = 0;
        charging_period.numberPhases = 2;
        profile3.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 32.f;
        charging_period.startPeriod  = 200;
        charging_period.numberPhases = 3;
        profile3.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        charging_period.limit        = 64.f;
        charging_period.startPeriod  = 500;
        charging_period.numberPhases = 3;
        profile3.chargingSchedule.chargingSchedulePeriod.push_back(charging_period);
        profile3.chargingSchedule.chargingRateUnit = ChargingRateUnitType::A;
        profile3.chargingSchedule.minChargingRate  = 6.f;
        CHECK(installProfile(0, profile3, smartcharging_mgr));

        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> charge_point_setpoint;
        ocpp::types::Optional<ocpp::types::SmartChargingSetpoint> connector_setpoint;
        CHECK(smartcharging_mgr.getSetpoint(1u, charge_point_setpoint, connector_setpoint, ChargingRateUnitType::A));

        CHECK(charge_point_setpoint.isSet());
        CHECK_EQ(charge_point_setpoint.value().value, 32.f);
        CHECK_EQ(charge_point_setpoint.value().number_phases, 3);
        CHECK(charge_point_setpoint.value().min_charging_rate.isSet());
        CHECK_EQ(charge_point_setpoint.value().min_charging_rate.value(), 6.f);

        CHECK(connector_setpoint.isSet());
        CHECK_EQ(connector_setpoint.value().value, 25.f);
        CHECK_EQ(connector_setpoint.value().number_phases, 3);
        CHECK(connector_setpoint.value().min_charging_rate.isSet());
        CHECK_EQ(connector_setpoint.value().min_charging_rate.value(), 10.f);
    }

    TEST_CASE("Cleanup")
    {
        CHECK(database.close());
        std::filesystem::remove(DATABASE_PATH);
    }
}
