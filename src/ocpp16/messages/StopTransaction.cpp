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

#include "StopTransaction.h"
#include "IRpc.h"
#include "IdTagInfoConverter.h"
#include "MeterValueConverter.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief Helper to convert a Reason enum to string */
const EnumToStringFromString<Reason> ReasonHelper = {{Reason::DeAuthorized, "DeAuthorized"},
                                                     {Reason::EmergencyStop, "EmergencyStop"},
                                                     {Reason::EVDisconnected, "EVDisconnected"},
                                                     {Reason::HardReset, "HardReset"},
                                                     {Reason::Local, "Local"},
                                                     {Reason::Other, "Other"},
                                                     {Reason::PowerLoss, "PowerLoss"},
                                                     {Reason::Reboot, "Reboot"},
                                                     {Reason::Remote, "Remote"},
                                                     {Reason::SoftReset, "SoftReset"},
                                                     {Reason::UnlockCommand, "UnlockCommand"}};

} // namespace ocpp16
} // namespace types

namespace messages
{
namespace ocpp16
{
/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool StopTransactionReqConverter::fromJson(const rapidjson::Value& json,
                                           StopTransactionReq&     data,
                                           std::string&            error_code,
                                           std::string&            error_message)
{
    extract(json, "idTag", data.idTag);
    extract(json, "meterStop", data.meterStop);
    bool ret = extract(json, "timestamp", data.timestamp, error_message);
    extract(json, "transactionId", data.transactionId);
    if (json.HasMember("reason"))
    {
        data.reason = ReasonHelper.fromString(json["reason"].GetString());
    }
    if (json.HasMember("transactionData"))
    {
        const rapidjson::Value& transactionData = json["transactionData"];
        MeterValueConverter     metervalue_converter;
        for (auto it_meter = transactionData.Begin(); ret && (it_meter != transactionData.End()); ++it_meter)
        {
            data.transactionData.emplace_back();
            MeterValue& meter_value = data.transactionData.back();
            ret                     = ret && metervalue_converter.fromJson(*it_meter, meter_value, error_code, error_message);
        }
    }
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool StopTransactionReqConverter::toJson(const StopTransactionReq& data, rapidjson::Document& json)
{
    bool ret = true;
    fill(json, "idTag", data.idTag);
    fill(json, "meterStop", data.meterStop);
    fill(json, "timestamp", data.timestamp);
    fill(json, "transactionId", data.transactionId);
    if (data.reason.isSet())
    {
        fill(json, "reason", ReasonHelper.toString(data.reason));
    }
    if (!data.transactionData.empty())
    {
        rapidjson::Value    transactionData(rapidjson::kArrayType);
        MeterValueConverter metervalue_converter;
        metervalue_converter.setAllocator(allocator);
        for (const MeterValue& meter_value : data.transactionData)
        {
            rapidjson::Document value(rapidjson::kObjectType);
            ret = ret && metervalue_converter.toJson(meter_value, value);
            transactionData.PushBack(value.Move(), *allocator);
        }
        json.AddMember(rapidjson::StringRef("transactionData"), transactionData.Move(), *allocator);
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool StopTransactionConfConverter::fromJson(const rapidjson::Value& json,
                                            StopTransactionConf&    data,
                                            std::string&            error_code,
                                            std::string&            error_message)
{
    bool ret = true;
    if (json.HasMember("idTagInfo"))
    {
        IdTagInfoConverter id_tag_info_converter;
        ret = id_tag_info_converter.fromJson(json["idTagInfo"], data.idTagInfo, error_code, error_message);
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool StopTransactionConfConverter::toJson(const StopTransactionConf& data, rapidjson::Document& json)
{
    bool ret = true;
    if (data.idTagInfo.isSet())
    {
        IdTagInfoConverter id_tag_info_converter;
        id_tag_info_converter.setAllocator(allocator);

        rapidjson::Document id_tag_info(rapidjson::kObjectType);
        ret = id_tag_info_converter.toJson(data.idTagInfo, id_tag_info);
        json.AddMember(rapidjson::StringRef("idTagInfo"), id_tag_info.Move(), *allocator);
    }
    return ret;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
