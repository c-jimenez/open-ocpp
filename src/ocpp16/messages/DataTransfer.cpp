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

#include "DataTransfer.h"
#include "IRpc.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace types
{
namespace ocpp16
{
/** @brief Helper to convert a enum class DataTransferStatus enum to string */
const EnumToStringFromString<DataTransferStatus> DataTransferStatusHelper = {{DataTransferStatus::Accepted, "Accepted"},
                                                                             {DataTransferStatus::Rejected, "Rejected"},
                                                                             {DataTransferStatus::UnknownMessageId, "UnknownMessageId"},
                                                                             {DataTransferStatus::UnknownVendorId, "UnknownVendorId"}};

} // namespace ocpp16
} // namespace types
namespace messages
{
namespace ocpp16
{

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool DataTransferReqConverter::fromJson(const rapidjson::Value& json,
                                        DataTransferReq&        data,
                                        std::string&            error_code,
                                        std::string&            error_message)
{
    (void)error_code;
    (void)error_message;
    extract(json, "vendorId", data.vendorId);
    extract(json, "messageId", data.messageId);
    extract(json, "data", data.data);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool DataTransferReqConverter::toJson(const DataTransferReq& data, rapidjson::Document& json)
{
    fill(json, "vendorId", data.vendorId);
    fill(json, "messageId", data.messageId);
    fill(json, "data", data.data);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::fromJson(const rapidjson::Value&, DataType&, std::string&, std::string&) */
bool DataTransferConfConverter::fromJson(const rapidjson::Value& json,
                                         DataTransferConf&       data,
                                         std::string&            error_code,
                                         std::string&            error_message)
{
    (void)error_code;
    (void)error_message;
    data.status = DataTransferStatusHelper.fromString(json["status"].GetString());
    extract(json, "data", data.data);
    return true;
}

/** @copydoc bool IMessageConverter<DataType>::toJson(DataType&, rapidjson::Document&, std::string&, std::string&) */
bool DataTransferConfConverter::toJson(const DataTransferConf& data, rapidjson::Document& json)
{
    fill(json, "status", DataTransferStatusHelper.toString(data.status));
    fill(json, "data", data.data);
    return true;
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
