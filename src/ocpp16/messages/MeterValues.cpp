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

#include "MeterValues.h"
#include "IRpc.h"
#include "MeterValueConverter.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{
/** @brief Helper to convert a ReadingContext enum to string */
const EnumToStringFromString<ReadingContext> ReadingContextHelper = {{ReadingContext::InterruptionBegin, "Interruption.Begin"},
                                                                     {ReadingContext::InterruptionEnd, "Interruption.End"},
                                                                     {ReadingContext::Other, "Other"},
                                                                     {ReadingContext::SampleClock, "Sample.Clock"},
                                                                     {ReadingContext::SamplePeriodic, "Sample.Periodic"},
                                                                     {ReadingContext::TransactionBegin, "Transaction.Begin"},
                                                                     {ReadingContext::TransactionEnd, "Transaction.End"},
                                                                     {ReadingContext::Trigger, "Trigger"}};
/** @brief Helper to convert a ValueFormat enum to string */
const EnumToStringFromString<ValueFormat> ValueFormatHelper = {{ValueFormat::Raw, "Raw"}, {ValueFormat::SignedData, "SignedData"}};
/** @brief Helper to convert a Measurand enum to string */
const EnumToStringFromString<Measurand> MeasurandHelper = {{Measurand::Current, "Current"},
                                                           {Measurand::CurrentImport, "Current.Import"},
                                                           {Measurand::CurrentOffered, "Current.Offered"},
                                                           {Measurand::EnergyActiveExportInterval, "Energy.Active.Export.Interval"},
                                                           {Measurand::EnergyActiveExportRegister, "Energy.Active.Export.Register"},
                                                           {Measurand::EnergyActiveImportInterval, "Energy.Active.Import.Interval"},
                                                           {Measurand::EnergyActiveImportRegister, "Energy.Active.Import.Register"},
                                                           {Measurand::EnergyReactiveExportInterval, "Energy.Reactive.Export.Interval"},
                                                           {Measurand::EnergyReactiveExportRegister, "Energy.Reactive.Export.Register"},
                                                           {Measurand::EnergyReactiveImportInterval, "Energy.Reactive.Import.Interval"},
                                                           {Measurand::EnergyReactiveImportRegister, "Energy.Reactive.Import.Register"},
                                                           {Measurand::Frequency, "Frequency"},
                                                           {Measurand::PowerActiveExport, "Power.Active.Export"},
                                                           {Measurand::PowerActiveImport, "Power.Active.Import"},
                                                           {Measurand::PowerFactor, "Power.Factor"},
                                                           {Measurand::PowerOffered, "Power.Offered"},
                                                           {Measurand::PowerReactiveExport, "Power.Reactive.Export"},
                                                           {Measurand::PowerReactiveImport, "Power.Reactive.Import"},
                                                           {Measurand::RPM, "RPM"},
                                                           {Measurand::SoC, "SoC"},
                                                           {Measurand::Temperature, "Temperature"},
                                                           {Measurand::Voltage, "Voltage"}};
/** @brief Helper to convert a Phase enum to string */
const EnumToStringFromString<Phase> PhaseHelper = {{Phase::L1, "L1"},
                                                   {Phase::L2, "L2"},
                                                   {Phase::L3, "L3"},
                                                   {Phase::N, "N"},
                                                   {Phase::L1N, "L1-N"},
                                                   {Phase::L2N, "L2-N"},
                                                   {Phase::L3N, "L3-N"},
                                                   {Phase::L1L2, "L1-L2"},
                                                   {Phase::L2L3, "L2-L3"},
                                                   {Phase::L3L1, "L3-L1"}};
/** @brief Helper to convert a Location enum to string */
const EnumToStringFromString<Location> LocationHelper = {
    {Location::Body, "Body"}, {Location::Cable, "Cable"}, {Location::EV, "EV"}, {Location::Inlet, "Inlet"}, {Location::Outlet, "Outlet"}};
/** @brief Helper to convert a UnitOfMeasure enum to string */
const EnumToStringFromString<UnitOfMeasure> UnitOfMeasureHelper = {{UnitOfMeasure::A, "A"},
                                                                   {UnitOfMeasure::Celsius, "Celsius"},
                                                                   {UnitOfMeasure::Fahrenheit, "Fahrenheit"},
                                                                   {UnitOfMeasure::K, "K"},
                                                                   {UnitOfMeasure::kVA, "kVA"},
                                                                   {UnitOfMeasure::kvar, "kvar"},
                                                                   {UnitOfMeasure::kvarh, "kvarh"},
                                                                   {UnitOfMeasure::kW, "kW"},
                                                                   {UnitOfMeasure::kWh, "kWh"},
                                                                   {UnitOfMeasure::Percent, "Percent"},
                                                                   {UnitOfMeasure::V, "V"},
                                                                   {UnitOfMeasure::VA, "VA"},
                                                                   {UnitOfMeasure::var, "var"},
                                                                   {UnitOfMeasure::varh, "varh"},
                                                                   {UnitOfMeasure::W, "W"},
                                                                   {UnitOfMeasure::Wh, "Wh"}};
} // namespace ocpp16
} // namespace types

namespace messages
{
namespace ocpp16
{
/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool MeterValuesReqConverter::fromJson(const rapidjson::Value& json,
                                       MeterValuesReq&         data,
                                       std::string&            error_code,
                                       std::string&            error_message)
{
    bool ret = extract(json, "connectorId", data.connectorId, error_message);
    extract(json, "transactionId", data.transactionId);

    const rapidjson::Value& meterValue = json["meterValue"];
    MeterValueConverter     metervalue_converter;
    for (auto it_meter = meterValue.Begin(); ret && (it_meter != meterValue.End()); ++it_meter)
    {
        data.meterValue.emplace_back();
        MeterValue& meter_value = data.meterValue.back();
        ret                     = ret && metervalue_converter.fromJson(*it_meter, meter_value, error_code, error_message);
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool MeterValuesReqConverter::toJson(const MeterValuesReq& data, rapidjson::Document& json)
{
    bool ret = true;

    fill(json, "connectorId", data.connectorId);
    fill(json, "transactionId", data.transactionId);

    rapidjson::Value    meterValue(rapidjson::kArrayType);
    MeterValueConverter metervalue_converter;
    metervalue_converter.setAllocator(allocator);
    for (const MeterValue& meter_value : data.meterValue)
    {
        rapidjson::Document value(rapidjson::kObjectType);
        ret = ret && metervalue_converter.toJson(meter_value, value);
        meterValue.PushBack(value.Move(), *allocator);
    }
    json.AddMember(rapidjson::StringRef("meterValue"), meterValue.Move(), *allocator);

    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool MeterValuesConfConverter::fromJson(const rapidjson::Value& json,
                                        MeterValuesConf&        data,
                                        std::string&            error_code,
                                        std::string&            error_message)
{
    (void)json;
    (void)data;
    (void)error_code;
    (void)error_message;
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool MeterValuesConfConverter::toJson(const MeterValuesConf& data, rapidjson::Document& json)
{
    (void)data;
    (void)json;
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
