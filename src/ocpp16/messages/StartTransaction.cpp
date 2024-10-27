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

#include "StartTransaction.h"
#include "IRpc.h"
#include "IdTagInfoConverter.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace messages
{
namespace ocpp16
{
/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool StartTransactionReqConverter::fromJson(const rapidjson::Value& json,
                                            StartTransactionReq&    data,
                                            std::string&            error_code,
                                            std::string&            error_message)
{
    bool ret = extract(json, "connectorId", data.connectorId, error_message);
    if (ret && (data.connectorId == 0))
    {
        error_message = "connectorId field must be > 0";
    }
    extract(json, "idTag", data.idTag);
    extract(json, "meterStart", data.meterStart);
    extract(json, "reservationId", data.reservationId);
    ret = ret && extract(json, "timestamp", data.timestamp, error_message);
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool StartTransactionReqConverter::toJson(const StartTransactionReq& data, rapidjson::Document& json)
{
    fill(json, "connectorId", data.connectorId);
    fill(json, "idTag", data.idTag);
    fill(json, "meterStart", data.meterStart);
    fill(json, "reservationId", data.reservationId);
    fill(json, "timestamp", data.timestamp);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool StartTransactionConfConverter::fromJson(const rapidjson::Value& json,
                                             StartTransactionConf&   data,
                                             std::string&            error_code,
                                             std::string&            error_message)
{
    IdTagInfoConverter id_tag_info_converter;
    bool               ret = id_tag_info_converter.fromJson(json["idTagInfo"], data.idTagInfo, error_code, error_message);
    extract(json, "transactionId", data.transactionId);
    if (!ret)
    {
        error_code = ocpp::rpc::IRpc::RPC_ERROR_TYPE_CONSTRAINT_VIOLATION;
    }
    return ret;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool StartTransactionConfConverter::toJson(const StartTransactionConf& data, rapidjson::Document& json)
{
    IdTagInfoConverter id_tag_info_converter;
    id_tag_info_converter.setAllocator(allocator);

    rapidjson::Document id_tag_info(rapidjson::kObjectType);
    bool                ret = id_tag_info_converter.toJson(data.idTagInfo, id_tag_info);
    json.AddMember(rapidjson::StringRef("idTagInfo"), id_tag_info.Move(), *allocator);
    fill(json, "transactionId", data.transactionId);
    return ret;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
