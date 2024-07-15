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

#ifndef OPENOCPP_DELETECERTIFICATE_H
#define OPENOCPP_DELETECERTIFICATE_H

#include "CertificateHashDataType.h"
#include "Enums.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Action corresponding to the DeleteCertificate messages */
static const std::string DELETE_CERTIFICATE_ACTION = "DeleteCertificate";

/** @brief DeleteCertificate.req message */
struct DeleteCertificateReq
{
    /** @brief Required. Indicates the certificate of which deletion is requested */
    ocpp::types::ocpp16::CertificateHashDataType certificateHashData;
};

/** @brief DeleteCertificate.conf message */
struct DeleteCertificateConf
{
    /** @brief Required. Charge Point indicates if it can process the request */
    ocpp::types::ocpp16::DeleteCertificateStatusEnumType status;
};

// Message converters
MESSAGE_CONVERTERS(DeleteCertificate)

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_DELETECERTIFICATE_H
