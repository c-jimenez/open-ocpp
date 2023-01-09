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

#include "MeterValuesManager.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "ChargePointEventsHandlerStub.h"
#include "ConfigManagerStub.h"
#include "Connectors.h"
#include "Database.h"
#include "GenericMessageSender.h"
#include "MessagesConverter.h"
#include "MessagesValidator.h"
#include "MeterValues.h"
#include "OcppConfigStub.h"
#include "RequestFifoStub.h"
#include "RpcStub.h"
#include "StatusManagerStub.h"
#include "TestableTimerPool.h"
#include "TestableWorkerThreadPool.h"
#include "TriggerMessageManagerStub.h"
#include "doctest_wrapper.h"

#include <filesystem>

using namespace ocpp::chargepoint;
using namespace ocpp::config;
using namespace ocpp::database;
using namespace ocpp::helpers;
using namespace ocpp::messages;
using namespace ocpp::rpc;
using namespace ocpp::types;

static constexpr const char* DATABASE_PATH = "/tmp/test.db";

Database                     database;
OcppConfigStub               ocpp_config;
ChargePointEventsHandlerStub event_handler;
RpcStub                      rpc;
TestableWorkerThreadPool     worker_pool;
RequestFifoStub              requests_fifo;
MessagesValidator            msgs_validator;
MessagesConverter            msgs_converter;
GenericMessageSender         msg_sender(rpc, msgs_converter, msgs_validator, std::chrono::milliseconds(1000));
TriggerMessageManagerStub    trigger_mgr;
StatusManagerStub            status_mgr;
ConfigManagerStub            config_mgr;

/** @brief Deserialize a meter value */
static bool deserializeMeterValue(const rapidjson::Document& payload, MeterValuesReq& meter_value)
{
    std::string             error_message;
    std::string             error_code;
    MeterValuesReqConverter meter_value_conv;
    meter_value.meterValue.clear();
    return meter_value_conv.fromJson(payload, meter_value, error_code, error_message);
}

/** @brief Check the clock aligned meter values */
static void checkClockAligned(const std::vector<std::pair<std::string, std::unique_ptr<rapidjson::Document>>>& messages)
{
    MeterValuesReq meter_value_req;

    CHECK_EQ(messages.size(), 3u);
    CHECK_EQ(messages[0].first, METER_VALUES_ACTION);
    CHECK_EQ(messages[1].first, METER_VALUES_ACTION);
    CHECK_EQ(messages[2].first, METER_VALUES_ACTION);

    CHECK(deserializeMeterValue((*messages[0].second), meter_value_req));
    CHECK_EQ(meter_value_req.connectorId, 0);
    CHECK_FALSE(meter_value_req.transactionId.isSet());
    CHECK_EQ(meter_value_req.meterValue.size(), 1u);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue.size(), 3u);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].measurand, Measurand::CurrentImport);
    CHECK_FALSE(meter_value_req.meterValue[0].sampledValue[0].phase.isSet());
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].location, Location::Inlet);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].format, ValueFormat::Raw);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].unit, UnitOfMeasure::A);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].context, ReadingContext::SampleClock);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].value, "20");
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].measurand, Measurand::EnergyActiveImportRegister);
    CHECK_FALSE(meter_value_req.meterValue[0].sampledValue[1].phase.isSet());
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].location, Location::Inlet);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].format, ValueFormat::Raw);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].unit, UnitOfMeasure::kWh);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].context, ReadingContext::SampleClock);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].value, "123");
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].measurand, Measurand::PowerActiveImport);
    CHECK_FALSE(meter_value_req.meterValue[0].sampledValue[2].phase.isSet());
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].location, Location::Inlet);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].format, ValueFormat::Raw);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].unit, UnitOfMeasure::kW);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].context, ReadingContext::SampleClock);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].value, "22");

    CHECK(deserializeMeterValue((*messages[1].second), meter_value_req));
    CHECK_EQ(meter_value_req.connectorId, 1);
    CHECK_FALSE(meter_value_req.transactionId.isSet());
    CHECK_EQ(meter_value_req.meterValue.size(), 1u);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue.size(), 3u);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].measurand, Measurand::CurrentImport);
    CHECK_FALSE(meter_value_req.meterValue[0].sampledValue[0].phase.isSet());
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].location, Location::Outlet);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].format, ValueFormat::Raw);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].unit, UnitOfMeasure::A);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].context, ReadingContext::SampleClock);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].value, "12");
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].measurand, Measurand::EnergyActiveImportRegister);
    CHECK_FALSE(meter_value_req.meterValue[0].sampledValue[1].phase.isSet());
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].location, Location::Outlet);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].format, ValueFormat::Raw);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].unit, UnitOfMeasure::kWh);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].context, ReadingContext::SampleClock);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].value, "100");
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].measurand, Measurand::PowerActiveImport);
    CHECK_FALSE(meter_value_req.meterValue[0].sampledValue[2].phase.isSet());
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].location, Location::Outlet);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].format, ValueFormat::Raw);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].unit, UnitOfMeasure::kW);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].context, ReadingContext::SampleClock);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].value, "6");

    CHECK(deserializeMeterValue((*messages[2].second), meter_value_req));
    CHECK_EQ(meter_value_req.connectorId, 2);
    CHECK_FALSE(meter_value_req.transactionId.isSet());
    CHECK_EQ(meter_value_req.meterValue.size(), 1u);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue.size(), 3u);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].measurand, Measurand::CurrentImport);
    CHECK_FALSE(meter_value_req.meterValue[0].sampledValue[0].phase.isSet());
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].location, Location::Outlet);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].format, ValueFormat::Raw);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].unit, UnitOfMeasure::A);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].context, ReadingContext::SampleClock);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].value, "8");
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].measurand, Measurand::EnergyActiveImportRegister);
    CHECK_FALSE(meter_value_req.meterValue[0].sampledValue[1].phase.isSet());
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].location, Location::Outlet);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].format, ValueFormat::Raw);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].unit, UnitOfMeasure::kWh);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].context, ReadingContext::SampleClock);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].value, "23");
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].measurand, Measurand::PowerActiveImport);
    CHECK_FALSE(meter_value_req.meterValue[0].sampledValue[2].phase.isSet());
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].location, Location::Outlet);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].format, ValueFormat::Raw);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].unit, UnitOfMeasure::kW);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].context, ReadingContext::SampleClock);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].value, "16");
}

/** @brief Check the sampled meter values */
static void checkSampled(const std::vector<std::pair<std::string, std::unique_ptr<rapidjson::Document>>>& messages)
{
    MeterValuesReq meter_value_req;

    CHECK_EQ(messages.size(), 1u);
    CHECK_EQ(messages[0].first, METER_VALUES_ACTION);

    CHECK(deserializeMeterValue((*messages[0].second), meter_value_req));
    CHECK_EQ(meter_value_req.connectorId, 2u);
    CHECK(meter_value_req.transactionId.isSet());
    CHECK_EQ(meter_value_req.transactionId, 12345);
    CHECK_EQ(meter_value_req.meterValue.size(), 1u);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue.size(), 4u);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].measurand, Measurand::CurrentImport);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].phase, Phase::L1);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].location, Location::Outlet);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].format, ValueFormat::Raw);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].unit, UnitOfMeasure::A);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].context, ReadingContext::SamplePeriodic);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].value, "70");

    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].measurand, Measurand::CurrentImport);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].phase, Phase::L2);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].location, Location::Outlet);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].format, ValueFormat::Raw);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].unit, UnitOfMeasure::A);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].context, ReadingContext::SamplePeriodic);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].value, "80");

    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].measurand, Measurand::CurrentImport);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].phase, Phase::L3);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].location, Location::Outlet);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].format, ValueFormat::Raw);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].unit, UnitOfMeasure::A);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].context, ReadingContext::SamplePeriodic);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].value, "90");

    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[3].measurand, Measurand::EnergyActiveImportRegister);
    CHECK_FALSE(meter_value_req.meterValue[0].sampledValue[3].phase.isSet());
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[3].location, Location::Outlet);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[3].format, ValueFormat::Raw);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[3].unit, UnitOfMeasure::kWh);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[3].context, ReadingContext::SamplePeriodic);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[3].value, "23");
}

/** @brief Check the triggered meter values */
static void checkTriggered(const std::vector<std::pair<std::string, std::unique_ptr<rapidjson::Document>>>& messages)
{
    MeterValuesReq meter_value_req;

    CHECK_EQ(messages.size(), 1u);
    CHECK_EQ(messages[0].first, METER_VALUES_ACTION);

    CHECK(deserializeMeterValue((*messages[0].second), meter_value_req));
    CHECK_EQ(meter_value_req.connectorId, 2u);
    CHECK_FALSE(meter_value_req.transactionId.isSet());
    CHECK_EQ(meter_value_req.meterValue.size(), 1u);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue.size(), 4u);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].measurand, Measurand::CurrentImport);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].phase, Phase::L1);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].location, Location::Outlet);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].format, ValueFormat::Raw);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].unit, UnitOfMeasure::A);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].context, ReadingContext::Trigger);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].value, "70");

    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].measurand, Measurand::CurrentImport);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].phase, Phase::L2);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].location, Location::Outlet);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].format, ValueFormat::Raw);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].unit, UnitOfMeasure::A);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].context, ReadingContext::Trigger);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].value, "80");

    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].measurand, Measurand::CurrentImport);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].phase, Phase::L3);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].location, Location::Outlet);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].format, ValueFormat::Raw);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].unit, UnitOfMeasure::A);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].context, ReadingContext::Trigger);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[2].value, "90");

    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[3].measurand, Measurand::EnergyActiveImportRegister);
    CHECK_FALSE(meter_value_req.meterValue[0].sampledValue[3].phase.isSet());
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[3].location, Location::Outlet);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[3].format, ValueFormat::Raw);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[3].unit, UnitOfMeasure::kWh);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[3].context, ReadingContext::Trigger);
    CHECK_EQ(meter_value_req.meterValue[0].sampledValue[3].value, "23");
}

TEST_SUITE("Metervalues component")
{
    TEST_CASE("Setup")
    {
        std::filesystem::remove(DATABASE_PATH);
        CHECK(database.open(DATABASE_PATH));

        CHECK(msgs_validator.load(SCHEMAS_DIR));
    }

    TEST_CASE("Setup config")
    {
        ocpp_config.setConfigValue("NumberOfConnectors", "2");

        ocpp_config.setConfigValue("ClockAlignedDataInterval", "1200");
        ocpp_config.setConfigValue("MeterValuesAlignedData", "Current.Import,Energy.Active.Import.Register,Power.Active.Import");
        ocpp_config.setConfigValue("MeterValuesAlignedDataMaxLength", "5");
        ocpp_config.setConfigValue("MeterValueSampleInterval", "10");
        ocpp_config.setConfigValue("MeterValuesSampledData",
                                   "Current.Import.L1,Current.Import.L2,Current.Import.L3,Energy.Active.Import.Register");
        ocpp_config.setConfigValue("MeterValuesSampledDataMaxLength", "5");
        ocpp_config.setConfigValue("StopTxnAlignedData", "Current.Import,Energy.Active.Import.Register");
        ocpp_config.setConfigValue("StopTxnAlignedDataMaxLength", "5");
        ocpp_config.setConfigValue("StopTxnSampledData",
                                   "Current.Import.L1,Current.Import.L2,Current.Import.L3,Energy.Active.Import.Register");
        ocpp_config.setConfigValue("StopTxnSampledDataMaxLength", "5");
    }

    TEST_CASE("Setup meter values")
    {
        MeterValue value;

        value.sampledValue.emplace_back();
        SampledValue& sampled_value = value.sampledValue.back();
        sampled_value.unit          = UnitOfMeasure::A;
        sampled_value.location      = Location::Inlet;
        sampled_value.format        = ValueFormat::Raw;
        sampled_value.value         = "20";
        event_handler.setMeterValue(0, {Measurand::CurrentImport, Optional<Phase>()}, value);
        sampled_value.location = Location::Outlet;
        sampled_value.value    = "12";
        event_handler.setMeterValue(1, {Measurand::CurrentImport, Optional<Phase>()}, value);
        sampled_value.location = Location::Outlet;
        sampled_value.value    = "8";
        event_handler.setMeterValue(2, {Measurand::CurrentImport, Optional<Phase>()}, value);

        sampled_value.unit     = UnitOfMeasure::kW;
        sampled_value.location = Location::Inlet;
        sampled_value.format   = ValueFormat::Raw;
        sampled_value.value    = "22";
        event_handler.setMeterValue(0, {Measurand::PowerActiveImport, Optional<Phase>()}, value);
        sampled_value.location = Location::Outlet;
        sampled_value.value    = "6";
        event_handler.setMeterValue(1, {Measurand::PowerActiveImport, Optional<Phase>()}, value);
        sampled_value.location = Location::Outlet;
        sampled_value.value    = "16";
        event_handler.setMeterValue(2, {Measurand::PowerActiveImport, Optional<Phase>()}, value);

        sampled_value.unit     = UnitOfMeasure::kWh;
        sampled_value.location = Location::Inlet;
        sampled_value.format   = ValueFormat::Raw;
        sampled_value.value    = "123";
        event_handler.setMeterValue(0, {Measurand::EnergyActiveImportRegister, Optional<Phase>()}, value);
        sampled_value.location = Location::Outlet;
        sampled_value.value    = "100";
        event_handler.setMeterValue(1, {Measurand::EnergyActiveImportRegister, Optional<Phase>()}, value);
        sampled_value.location = Location::Outlet;
        sampled_value.value    = "23";
        event_handler.setMeterValue(2, {Measurand::EnergyActiveImportRegister, Optional<Phase>()}, value);

        sampled_value.unit     = UnitOfMeasure::A;
        sampled_value.location = Location::Inlet;
        sampled_value.format   = ValueFormat::Raw;
        sampled_value.phase    = Phase::L1;
        sampled_value.value    = "10";
        event_handler.setMeterValue(0, {Measurand::CurrentImport, Optional<Phase>(Phase::L1)}, value);
        sampled_value.phase = Phase::L2;
        sampled_value.value = "20";
        event_handler.setMeterValue(0, {Measurand::CurrentImport, Optional<Phase>(Phase::L2)}, value);
        sampled_value.phase = Phase::L3;
        sampled_value.value = "30";
        event_handler.setMeterValue(0, {Measurand::CurrentImport, Optional<Phase>(Phase::L3)}, value);

        sampled_value.location = Location::Outlet;
        sampled_value.phase    = Phase::L1;
        sampled_value.value    = "40";
        event_handler.setMeterValue(1, {Measurand::CurrentImport, Optional<Phase>(Phase::L1)}, value);
        sampled_value.phase = Phase::L2;
        sampled_value.value = "50";
        event_handler.setMeterValue(1, {Measurand::CurrentImport, Optional<Phase>(Phase::L2)}, value);
        sampled_value.phase = Phase::L3;
        sampled_value.value = "60";
        event_handler.setMeterValue(1, {Measurand::CurrentImport, Optional<Phase>(Phase::L3)}, value);

        sampled_value.phase = Phase::L1;
        sampled_value.value = "70";
        event_handler.setMeterValue(2, {Measurand::CurrentImport, Optional<Phase>(Phase::L1)}, value);
        sampled_value.phase = Phase::L2;
        sampled_value.value = "80";
        event_handler.setMeterValue(2, {Measurand::CurrentImport, Optional<Phase>(Phase::L2)}, value);
        sampled_value.phase = Phase::L3;
        sampled_value.value = "90";
        event_handler.setMeterValue(2, {Measurand::CurrentImport, Optional<Phase>(Phase::L3)}, value);

        event_handler.setCallResult("getMeterValue", true);
    }

    TEST_CASE("Clock aligned meter values")
    {
        std::map<std::string, std::string> params;

        TestableTimerPool timer_pool;
        Connectors        connectors(ocpp_config, database, timer_pool);
        connectors.initDatabaseTable();

        MeterValuesManager meter_mgr(ocpp_config,
                                     database,
                                     event_handler,
                                     timer_pool,
                                     worker_pool,
                                     connectors,
                                     msg_sender,
                                     requests_fifo,
                                     status_mgr,
                                     trigger_mgr,
                                     config_mgr);

        // Check periodic timer configuration
        Timer* periodic_timer = timer_pool.getTimer(MeterValuesManager::CLOCK_ALIGNED_TIMER_NAME);
        CHECK_NE(periodic_timer, nullptr);
        CHECK(periodic_timer->isStarted());
        CHECK_FALSE(periodic_timer->isSingleShot());
        CHECK_LE(periodic_timer->getInterval().count(), 1200000);
        CHECK(periodic_timer->getCallback());

        // Connected
        rpc.setConnected(true);
        status_mgr.updateConnectionStatus(true);

        // Rejected by Central System
        status_mgr.forceRegistrationStatus(RegistrationStatus::Rejected);

        // Execute callback
        periodic_timer->getCallback()();
        CHECK(periodic_timer->isStarted());
        CHECK_EQ(periodic_timer->getInterval().count(), 1200000);
        CHECK_FALSE(event_handler.methodCalled("getMeterValue", params));

        // Pending by Central System
        status_mgr.forceRegistrationStatus(RegistrationStatus::Pending);

        // Execute callback
        periodic_timer->getCallback()();
        CHECK_FALSE(event_handler.methodCalled("getMeterValue", params));

        // Accepted by Central System
        status_mgr.forceRegistrationStatus(RegistrationStatus::Accepted);

        // Execute callback
        periodic_timer->getCallback()();
        CHECK(event_handler.methodCalled("getMeterValue", params));

        // Check messages
        checkClockAligned(rpc.getCalls());

        // Clear stubs
        event_handler.clearCalls();
        rpc.clearCalls();

        // Execute callback
        periodic_timer->getCallback()();
        CHECK(event_handler.methodCalled("getMeterValue", params));

        // Check messages
        checkClockAligned(rpc.getCalls());

        // Clear stubs
        event_handler.clearCalls();
        rpc.clearCalls();

        // Disconnected
        rpc.setConnected(false);
        status_mgr.updateConnectionStatus(false);

        // Execute callback
        periodic_timer->getCallback()();
        CHECK(event_handler.methodCalled("getMeterValue", params));

        // Check messages
        size_t      count        = 0;
        const auto& rpc_messages = rpc.getCalls();
        CHECK(rpc_messages.empty());

        std::vector<std::pair<std::string, std::unique_ptr<rapidjson::Document>>> fifo_messages;
        while (!requests_fifo.empty())
        {
            unsigned int         connector_id = 0;
            std::string          action;
            rapidjson::Document* payload = new rapidjson::Document();
            CHECK(requests_fifo.front(connector_id, action, *payload));
            CHECK_EQ(connector_id, count);
            fifo_messages.emplace_back(action, payload);
            requests_fifo.pop();
            count++;
        }
        checkClockAligned(fifo_messages);

        // Clear stubs
        event_handler.clearCalls();
        rpc.clearCalls();

        // Disable clock aligned values
        ocpp_config.setConfigValue("ClockAlignedDataInterval", "0");
        meter_mgr.configurationValueChanged("ClockAlignedDataInterval");

        // Check periodic timer configuration
        CHECK_FALSE(periodic_timer->isStarted());

        // Re-enable clock aligned values
        ocpp_config.setConfigValue("ClockAlignedDataInterval", "100");
        meter_mgr.configurationValueChanged("ClockAlignedDataInterval");

        // Check periodic timer configuration
        CHECK(periodic_timer->isStarted());
        CHECK_FALSE(periodic_timer->isSingleShot());
        CHECK_LE(periodic_timer->getInterval().count(), 100000);
        CHECK(periodic_timer->getCallback());
    }

    TEST_CASE("Sampled meter values")
    {
        std::map<std::string, std::string> params;

        TestableTimerPool timer_pool;
        Connectors        connectors(ocpp_config, database, timer_pool);
        connectors.initDatabaseTable();

        MeterValuesManager meter_mgr(ocpp_config,
                                     database,
                                     event_handler,
                                     timer_pool,
                                     worker_pool,
                                     connectors,
                                     msg_sender,
                                     requests_fifo,
                                     status_mgr,
                                     trigger_mgr,
                                     config_mgr);

        // Check sample timers configuration
        Timer* sample_timer0 = &connectors.getConnector(0u)->meter_values_timer;
        Timer* sample_timer1 = &connectors.getConnector(1u)->meter_values_timer;
        Timer* sample_timer2 = &connectors.getConnector(2u)->meter_values_timer;
        CHECK_FALSE(sample_timer0->isStarted());
        CHECK_FALSE(sample_timer1->isStarted());
        CHECK_FALSE(sample_timer2->isStarted());

        // Connected
        rpc.setConnected(true);
        status_mgr.updateConnectionStatus(true);

        // Accepted by Central System
        status_mgr.forceRegistrationStatus(RegistrationStatus::Accepted);

        // Start transaction on connector 2
        connectors.getConnector(2u)->transaction_id = 12345;
        meter_mgr.startSampledMeterValues(2u);

        // Check sample timers configuration
        CHECK_FALSE(sample_timer0->isStarted());
        CHECK_FALSE(sample_timer1->isStarted());
        CHECK(sample_timer2->isStarted());
        CHECK_FALSE(sample_timer2->isSingleShot());
        CHECK_LE(sample_timer2->getInterval().count(), 10000);
        CHECK(sample_timer2->getCallback());

        // Execute callback
        sample_timer2->getCallback()();
        CHECK(sample_timer2->isStarted());
        CHECK_EQ(sample_timer2->getInterval().count(), 10000);
        CHECK(event_handler.methodCalled("getMeterValue", params));

        // Check messages
        checkSampled(rpc.getCalls());

        // Clear stubs
        event_handler.clearCalls();
        rpc.clearCalls();

        // Execute callback
        sample_timer2->getCallback()();
        CHECK(event_handler.methodCalled("getMeterValue", params));

        // Check messages
        checkSampled(rpc.getCalls());

        // Clear stubs
        event_handler.clearCalls();
        rpc.clearCalls();

        // Disconnected
        rpc.setConnected(false);
        status_mgr.updateConnectionStatus(false);

        // Execute callback
        sample_timer2->getCallback()();
        CHECK(event_handler.methodCalled("getMeterValue", params));

        // Check messages
        const auto& rpc_messages = rpc.getCalls();
        CHECK(rpc_messages.empty());

        std::vector<std::pair<std::string, std::unique_ptr<rapidjson::Document>>> fifo_messages;
        while (!requests_fifo.empty())
        {
            unsigned int         connector_id = 0;
            std::string          action;
            rapidjson::Document* payload = new rapidjson::Document();
            CHECK(requests_fifo.front(connector_id, action, *payload));
            CHECK_EQ(connector_id, 2u);
            fifo_messages.emplace_back(action, payload);
            requests_fifo.pop();
        }
        checkSampled(fifo_messages);

        // Clear stubs
        event_handler.clearCalls();
        rpc.clearCalls();

        // Stop transaction on connector 2
        meter_mgr.stopSampledMeterValues(2u);

        // Check sample timers configuration
        CHECK_FALSE(sample_timer0->isStarted());
        CHECK_FALSE(sample_timer1->isStarted());
        CHECK_FALSE(sample_timer2->isStarted());

        // Disable sampled values
        ocpp_config.setConfigValue("MeterValueSampleInterval", "0");

        // Start transaction on connector 1
        connectors.getConnector(1u)->transaction_id = 54321;
        meter_mgr.startSampledMeterValues(1u);

        // Check sample timers configuration
        CHECK_FALSE(sample_timer0->isStarted());
        CHECK_FALSE(sample_timer1->isStarted());
        CHECK_FALSE(sample_timer2->isStarted());

        // Re-enable sampled values
        ocpp_config.setConfigValue("MeterValueSampleInterval", "10");
    }

    TEST_CASE("Stop transaction meter values")
    {
        std::map<std::string, std::string> params;

        TestableTimerPool timer_pool;
        Connectors        connectors(ocpp_config, database, timer_pool);
        connectors.initDatabaseTable();

        MeterValuesManager meter_mgr(ocpp_config,
                                     database,
                                     event_handler,
                                     timer_pool,
                                     worker_pool,
                                     connectors,
                                     msg_sender,
                                     requests_fifo,
                                     status_mgr,
                                     trigger_mgr,
                                     config_mgr);

        // Get timers
        Timer* sample_timer1  = &connectors.getConnector(1u)->meter_values_timer;
        Timer* periodic_timer = timer_pool.getTimer(MeterValuesManager::CLOCK_ALIGNED_TIMER_NAME);

        // Connected
        rpc.setConnected(true);
        status_mgr.updateConnectionStatus(true);

        // Accepted by Central System
        status_mgr.forceRegistrationStatus(RegistrationStatus::Accepted);

        // Start transaction on connector 1
        connectors.getConnector(1u)->transaction_id = 789;
        meter_mgr.startSampledMeterValues(1u);

        // Execute callbacks
        periodic_timer->getCallback()();
        sample_timer1->getCallback()();
        sample_timer1->getCallback()();
        periodic_timer->getCallback()();

        // Stop transaction on connector 1
        meter_mgr.stopSampledMeterValues(1u);

        // Check values
        std::vector<MeterValue> meter_values;
        meter_mgr.getTxStopMeterValues(1u, meter_values);
        CHECK_EQ(meter_values.size(), 4u);

        CHECK_EQ(meter_values[0].sampledValue.size(), 2u);
        CHECK_EQ(meter_values[0].sampledValue[0].measurand, Measurand::CurrentImport);
        CHECK_FALSE(meter_values[0].sampledValue[0].phase.isSet());
        CHECK_EQ(meter_values[0].sampledValue[0].location, Location::Outlet);
        CHECK_EQ(meter_values[0].sampledValue[0].format, ValueFormat::Raw);
        CHECK_EQ(meter_values[0].sampledValue[0].unit, UnitOfMeasure::A);
        CHECK_EQ(meter_values[0].sampledValue[0].context, ReadingContext::SampleClock);
        CHECK_EQ(meter_values[0].sampledValue[0].value, "12");
        CHECK_EQ(meter_values[0].sampledValue[1].measurand, Measurand::EnergyActiveImportRegister);
        CHECK_FALSE(meter_values[0].sampledValue[1].phase.isSet());
        CHECK_EQ(meter_values[0].sampledValue[1].location, Location::Outlet);
        CHECK_EQ(meter_values[0].sampledValue[1].format, ValueFormat::Raw);
        CHECK_EQ(meter_values[0].sampledValue[1].unit, UnitOfMeasure::kWh);
        CHECK_EQ(meter_values[0].sampledValue[1].context, ReadingContext::SampleClock);
        CHECK_EQ(meter_values[0].sampledValue[1].value, "100");

        CHECK_EQ(meter_values[1].sampledValue.size(), 4u);
        CHECK_EQ(meter_values[1].sampledValue[0].measurand, Measurand::CurrentImport);
        CHECK_EQ(meter_values[1].sampledValue[0].phase, Phase::L1);
        CHECK_EQ(meter_values[1].sampledValue[0].location, Location::Outlet);
        CHECK_EQ(meter_values[1].sampledValue[0].format, ValueFormat::Raw);
        CHECK_EQ(meter_values[1].sampledValue[0].unit, UnitOfMeasure::A);
        CHECK_EQ(meter_values[1].sampledValue[0].context, ReadingContext::SamplePeriodic);
        CHECK_EQ(meter_values[1].sampledValue[0].value, "40");
        CHECK_EQ(meter_values[1].sampledValue[1].measurand, Measurand::CurrentImport);
        CHECK_EQ(meter_values[1].sampledValue[1].phase, Phase::L2);
        CHECK_EQ(meter_values[1].sampledValue[1].location, Location::Outlet);
        CHECK_EQ(meter_values[1].sampledValue[1].format, ValueFormat::Raw);
        CHECK_EQ(meter_values[1].sampledValue[1].unit, UnitOfMeasure::A);
        CHECK_EQ(meter_values[1].sampledValue[1].context, ReadingContext::SamplePeriodic);
        CHECK_EQ(meter_values[1].sampledValue[1].value, "50");
        CHECK_EQ(meter_values[1].sampledValue[2].measurand, Measurand::CurrentImport);
        CHECK_EQ(meter_values[1].sampledValue[2].phase, Phase::L3);
        CHECK_EQ(meter_values[1].sampledValue[2].location, Location::Outlet);
        CHECK_EQ(meter_values[1].sampledValue[2].format, ValueFormat::Raw);
        CHECK_EQ(meter_values[1].sampledValue[2].unit, UnitOfMeasure::A);
        CHECK_EQ(meter_values[1].sampledValue[2].context, ReadingContext::SamplePeriodic);
        CHECK_EQ(meter_values[1].sampledValue[2].value, "60");
        CHECK_EQ(meter_values[1].sampledValue[3].measurand, Measurand::EnergyActiveImportRegister);
        CHECK_FALSE(meter_values[1].sampledValue[3].phase.isSet());
        CHECK_EQ(meter_values[1].sampledValue[3].location, Location::Outlet);
        CHECK_EQ(meter_values[1].sampledValue[3].format, ValueFormat::Raw);
        CHECK_EQ(meter_values[1].sampledValue[3].unit, UnitOfMeasure::kWh);
        CHECK_EQ(meter_values[1].sampledValue[3].context, ReadingContext::SamplePeriodic);
        CHECK_EQ(meter_values[1].sampledValue[3].value, "100");

        CHECK_EQ(meter_values[2].sampledValue.size(), 4u);
        CHECK_EQ(meter_values[2].sampledValue[0].measurand, Measurand::CurrentImport);
        CHECK_EQ(meter_values[2].sampledValue[0].phase, Phase::L1);
        CHECK_EQ(meter_values[2].sampledValue[0].location, Location::Outlet);
        CHECK_EQ(meter_values[2].sampledValue[0].format, ValueFormat::Raw);
        CHECK_EQ(meter_values[2].sampledValue[0].unit, UnitOfMeasure::A);
        CHECK_EQ(meter_values[2].sampledValue[0].context, ReadingContext::SamplePeriodic);
        CHECK_EQ(meter_values[2].sampledValue[0].value, "40");
        CHECK_EQ(meter_values[2].sampledValue[1].measurand, Measurand::CurrentImport);
        CHECK_EQ(meter_values[2].sampledValue[1].phase, Phase::L2);
        CHECK_EQ(meter_values[2].sampledValue[1].location, Location::Outlet);
        CHECK_EQ(meter_values[2].sampledValue[1].format, ValueFormat::Raw);
        CHECK_EQ(meter_values[2].sampledValue[1].unit, UnitOfMeasure::A);
        CHECK_EQ(meter_values[2].sampledValue[1].context, ReadingContext::SamplePeriodic);
        CHECK_EQ(meter_values[2].sampledValue[1].value, "50");
        CHECK_EQ(meter_values[2].sampledValue[2].measurand, Measurand::CurrentImport);
        CHECK_EQ(meter_values[2].sampledValue[2].phase, Phase::L3);
        CHECK_EQ(meter_values[2].sampledValue[2].location, Location::Outlet);
        CHECK_EQ(meter_values[2].sampledValue[2].format, ValueFormat::Raw);
        CHECK_EQ(meter_values[2].sampledValue[2].unit, UnitOfMeasure::A);
        CHECK_EQ(meter_values[2].sampledValue[2].context, ReadingContext::SamplePeriodic);
        CHECK_EQ(meter_values[2].sampledValue[2].value, "60");
        CHECK_EQ(meter_values[2].sampledValue[3].measurand, Measurand::EnergyActiveImportRegister);
        CHECK_FALSE(meter_values[2].sampledValue[3].phase.isSet());
        CHECK_EQ(meter_values[2].sampledValue[3].location, Location::Outlet);
        CHECK_EQ(meter_values[2].sampledValue[3].format, ValueFormat::Raw);
        CHECK_EQ(meter_values[2].sampledValue[3].unit, UnitOfMeasure::kWh);
        CHECK_EQ(meter_values[2].sampledValue[3].context, ReadingContext::SamplePeriodic);
        CHECK_EQ(meter_values[2].sampledValue[3].value, "100");

        CHECK_EQ(meter_values[3].sampledValue.size(), 2u);
        CHECK_EQ(meter_values[3].sampledValue[0].measurand, Measurand::CurrentImport);
        CHECK_FALSE(meter_values[3].sampledValue[0].phase.isSet());
        CHECK_EQ(meter_values[3].sampledValue[0].location, Location::Outlet);
        CHECK_EQ(meter_values[3].sampledValue[0].format, ValueFormat::Raw);
        CHECK_EQ(meter_values[3].sampledValue[0].unit, UnitOfMeasure::A);
        CHECK_EQ(meter_values[3].sampledValue[0].context, ReadingContext::SampleClock);
        CHECK_EQ(meter_values[3].sampledValue[0].value, "12");
        CHECK_EQ(meter_values[3].sampledValue[1].measurand, Measurand::EnergyActiveImportRegister);
        CHECK_FALSE(meter_values[3].sampledValue[1].phase.isSet());
        CHECK_EQ(meter_values[3].sampledValue[1].location, Location::Outlet);
        CHECK_EQ(meter_values[3].sampledValue[1].format, ValueFormat::Raw);
        CHECK_EQ(meter_values[3].sampledValue[1].unit, UnitOfMeasure::kWh);
        CHECK_EQ(meter_values[3].sampledValue[1].context, ReadingContext::SampleClock);
        CHECK_EQ(meter_values[3].sampledValue[1].value, "100");

        // Disable stop transaction values
        ocpp_config.setConfigValue("StopTxnAlignedData", "");
        ocpp_config.setConfigValue("StopTxnSampledData", "");

        // Start transaction on connector 1
        connectors.getConnector(1u)->transaction_id = 987;
        meter_mgr.startSampledMeterValues(1u);

        // Execute callbacks
        periodic_timer->getCallback()();
        sample_timer1->getCallback()();
        sample_timer1->getCallback()();
        periodic_timer->getCallback()();

        // Stop transaction on connector 1
        meter_mgr.stopSampledMeterValues(1u);

        // Check values
        meter_mgr.getTxStopMeterValues(1u, meter_values);
        CHECK(meter_values.empty());

        // Clear stubs
        event_handler.clearCalls();
        rpc.clearCalls();
    }

    TEST_CASE("Triggered meter values")
    {
        std::map<std::string, std::string> params;

        TestableTimerPool timer_pool;
        Connectors        connectors(ocpp_config, database, timer_pool);
        connectors.initDatabaseTable();

        MeterValuesManager meter_mgr(ocpp_config,
                                     database,
                                     event_handler,
                                     timer_pool,
                                     worker_pool,
                                     connectors,
                                     msg_sender,
                                     requests_fifo,
                                     status_mgr,
                                     trigger_mgr,
                                     config_mgr);

        // Connected
        rpc.setConnected(true);
        status_mgr.updateConnectionStatus(true);

        // Accepted by Central System
        status_mgr.forceRegistrationStatus(RegistrationStatus::Accepted);

        // Trigger meter values on connector 2
        CHECK(meter_mgr.onTriggerMessage(MessageTrigger::MeterValues, Optional<unsigned int>(2u)));
        CHECK(event_handler.methodCalled("getMeterValue", params));

        // Check messages
        checkTriggered(rpc.getCalls());

        // Clear stubs
        event_handler.clearCalls();
        rpc.clearCalls();

        // Extended trigger meter values on connector 2
        CHECK(meter_mgr.onTriggerMessage(MessageTriggerEnumType::MeterValues, Optional<unsigned int>(2u)));
        CHECK(event_handler.methodCalled("getMeterValue", params));

        // Check messages
        checkTriggered(rpc.getCalls());

        // Clear stubs
        event_handler.clearCalls();
        rpc.clearCalls();

        // Trigger meter values without connector id
        CHECK_FALSE(meter_mgr.onTriggerMessage(MessageTrigger::MeterValues, Optional<unsigned int>()));
        CHECK_FALSE(event_handler.methodCalled("getMeterValue", params));

        // Check messages
        CHECK(rpc.getCalls().empty());

        // Extended trigger meter values without connector id
        CHECK_FALSE(meter_mgr.onTriggerMessage(MessageTriggerEnumType::MeterValues, Optional<unsigned int>()));
        CHECK_FALSE(event_handler.methodCalled("getMeterValue", params));

        // Check messages
        CHECK(rpc.getCalls().empty());
    }

    TEST_CASE("Custom meter values")
    {
        std::map<std::string, std::string> params;

        TestableTimerPool timer_pool;
        Connectors        connectors(ocpp_config, database, timer_pool);
        connectors.initDatabaseTable();

        MeterValuesManager meter_mgr(ocpp_config,
                                     database,
                                     event_handler,
                                     timer_pool,
                                     worker_pool,
                                     connectors,
                                     msg_sender,
                                     requests_fifo,
                                     status_mgr,
                                     trigger_mgr,
                                     config_mgr);

        // Accepted by Central System
        status_mgr.forceRegistrationStatus(RegistrationStatus::Accepted);

        // Response
        rapidjson::Document json_resp;
        json_resp.Parse("{}");
        rpc.setResponse(json_resp);

        // Send meter values on connector 2
        std::vector<MeterValue> meter_values;
        meter_values.emplace_back();
        MeterValue& meters1 = meter_values.back();
        meters1.timestamp   = DateTime::now().timestamp();
        meters1.sampledValue.emplace_back();
        SampledValue& sampled1 = meters1.sampledValue.back();
        sampled1.measurand     = Measurand::Temperature;
        sampled1.unit          = UnitOfMeasure::Celsius;
        sampled1.value         = "1234";
        sampled1.context       = ReadingContext::InterruptionBegin;
        meters1.sampledValue.emplace_back();
        SampledValue& sampled2 = meters1.sampledValue.back();
        sampled2.measurand     = Measurand::Frequency;
        sampled2.value         = "50";
        sampled2.context       = ReadingContext::Other;
        meter_values.emplace_back();
        MeterValue& meters2 = meter_values.back();
        meters2.timestamp   = DateTime::now().timestamp();
        meters2.sampledValue.emplace_back();
        SampledValue& sampled3 = meters2.sampledValue.back();
        sampled3.measurand     = Measurand::PowerFactor;
        sampled3.unit          = UnitOfMeasure::Percent;
        sampled3.value         = "90";
        sampled3.context       = ReadingContext::InterruptionEnd;
        meters2.sampledValue.emplace_back();
        SampledValue& sampled4 = meters2.sampledValue.back();
        sampled4.measurand     = Measurand::RPM;
        sampled4.value         = "3000";
        sampled4.context       = ReadingContext::Other;
        CHECK(meter_mgr.sendMeterValues(2u, meter_values));

        // Check messages
        MeterValuesReq meter_value_req;
        const auto&    messages = rpc.getCalls();

        CHECK_EQ(messages.size(), 1u);
        CHECK_EQ(messages[0].first, METER_VALUES_ACTION);

        CHECK(deserializeMeterValue((*messages[0].second), meter_value_req));
        CHECK_EQ(meter_value_req.connectorId, 2u);
        CHECK_FALSE(meter_value_req.transactionId.isSet());
        CHECK_EQ(meter_value_req.meterValue.size(), 2u);
        CHECK_EQ(meter_value_req.meterValue[0].sampledValue.size(), 2u);
        CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].measurand, Measurand::Temperature);
        CHECK_FALSE(meter_value_req.meterValue[0].sampledValue[0].phase.isSet());
        CHECK_FALSE(meter_value_req.meterValue[0].sampledValue[0].location.isSet());
        CHECK_FALSE(meter_value_req.meterValue[0].sampledValue[0].format.isSet());
        CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].unit, UnitOfMeasure::Celsius);
        CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].context, ReadingContext::InterruptionBegin);
        CHECK_EQ(meter_value_req.meterValue[0].sampledValue[0].value, "1234");
        CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].measurand, Measurand::Frequency);
        CHECK_FALSE(meter_value_req.meterValue[0].sampledValue[1].phase.isSet());
        CHECK_FALSE(meter_value_req.meterValue[0].sampledValue[1].location.isSet());
        CHECK_FALSE(meter_value_req.meterValue[0].sampledValue[1].format.isSet());
        CHECK_FALSE(meter_value_req.meterValue[0].sampledValue[1].unit.isSet());
        CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].context, ReadingContext::Other);
        CHECK_EQ(meter_value_req.meterValue[0].sampledValue[1].value, "50");
        CHECK_EQ(meter_value_req.meterValue[1].sampledValue.size(), 2u);
        CHECK_EQ(meter_value_req.meterValue[1].sampledValue[0].measurand, Measurand::PowerFactor);
        CHECK_FALSE(meter_value_req.meterValue[1].sampledValue[0].phase.isSet());
        CHECK_FALSE(meter_value_req.meterValue[1].sampledValue[0].location.isSet());
        CHECK_FALSE(meter_value_req.meterValue[1].sampledValue[0].format.isSet());
        CHECK_EQ(meter_value_req.meterValue[1].sampledValue[0].unit, UnitOfMeasure::Percent);
        CHECK_EQ(meter_value_req.meterValue[1].sampledValue[0].context, ReadingContext::InterruptionEnd);
        CHECK_EQ(meter_value_req.meterValue[1].sampledValue[0].value, "90");
        CHECK_EQ(meter_value_req.meterValue[1].sampledValue[1].measurand, Measurand::RPM);
        CHECK_FALSE(meter_value_req.meterValue[1].sampledValue[1].phase.isSet());
        CHECK_FALSE(meter_value_req.meterValue[1].sampledValue[1].location.isSet());
        CHECK_FALSE(meter_value_req.meterValue[1].sampledValue[1].format.isSet());
        CHECK_FALSE(meter_value_req.meterValue[1].sampledValue[1].unit.isSet());
        CHECK_EQ(meter_value_req.meterValue[1].sampledValue[1].context, ReadingContext::Other);
        CHECK_EQ(meter_value_req.meterValue[1].sampledValue[1].value, "3000");

        // Clear stubs
        event_handler.clearCalls();
        rpc.clearCalls();
    }

    TEST_CASE("Cleanup")
    {
        CHECK(database.close());
        std::filesystem::remove(DATABASE_PATH);
    }
}
