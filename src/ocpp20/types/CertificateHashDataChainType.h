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

/*
    Generated by json2cpp from : urn:OCPP:Cp:2:2020:3:GetInstalledCertificateIdsRequest
                                 OCPP 2.0.1 FINAL
*/

#ifndef OPENOCPP_CERTIFICATEHASHDATACHAINTYPE_H
#define OPENOCPP_CERTIFICATEHASHDATACHAINTYPE_H

#include "CustomDataType.h"
#include "CertificateHashDataType.h"
#include "GetCertificateIdUseEnumType.h"
#include "CertificateHashDataType.h"

#include "Optional.h"
#include <vector>
#include "IMessageConverter.h"

namespace ocpp
{
namespace types
{
namespace ocpp20
{

/** @brief  */
struct CertificateHashDataChainType
{
    /** @brief  */
    ocpp::types::Optional<CustomDataType> customData;
    /** @brief  */
    CertificateHashDataType certificateHashData;
    /** @brief  */
    GetCertificateIdUseEnumType certificateType;
    /** @brief  */
    std::vector<CertificateHashDataType> childCertificateHashData;
};

/** @brief Converter class for CertificateHashDataChainType type */
class CertificateHashDataChainTypeConverter : public ocpp::messages::IMessageConverter<CertificateHashDataChainType>
{
  public:
    /** @brief Clone the converter */
    ocpp::messages::IMessageConverter<CertificateHashDataChainType>* clone() const override { return new CertificateHashDataChainTypeConverter(); }

    /** @brief Convert a CertificateHashDataChainType from a JSON representation */
    bool fromJson(const rapidjson::Value&       json,
                  CertificateHashDataChainType&                data,
                  std::string&                  error_code,
                  [[maybe_unused]] std::string& error_message) override;

    /** @brief Convert a CertificateHashDataChainType to a JSON representation */
    bool toJson(const CertificateHashDataChainType& data, rapidjson::Document& json) override;
};

} // namespace ocpp20
} // namespace types
} // namespace ocpp

#endif // OPENOCPP_CERTIFICATEHASHDATACHAINTYPE_H