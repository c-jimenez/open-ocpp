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

#ifndef OPENOCPP_DATATRANSFER_H
#define OPENOCPP_DATATRANSFER_H

#include "CiStringType.h"
#include "Enums.h"
#include "IMessageConverter.h"
#include "Optional.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the DataTransfer messages */
static const std::string DATA_TRANSFER_ACTION = "DataTransfer";

/** @brief DataTransfer.req message */
struct DataTransferReq
{
    /** @brief Required. This identifies the Vendor specific implementation */
    ocpp::types::CiStringType<255u> vendorId;
    /** @brief Optional. Additional identification field */
    ocpp::types::Optional<ocpp::types::CiStringType<50u>> messageId;
    /** @brief Optional. Data without specified length or format */
    ocpp::types::Optional<std::string> data;
};

/** @brief DataTransfer.conf message */
struct DataTransferConf
{
    /** @brief Required. This indicates the success or failure of the data transfer */
    ocpp::types::ocpp16::DataTransferStatus status;
    /** @brief Optional. Data in response to request */
    ocpp::types::Optional<std::string> data;
};

// Message converters
MESSAGE_CONVERTERS(DataTransfer)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_DATATRANSFER_H
