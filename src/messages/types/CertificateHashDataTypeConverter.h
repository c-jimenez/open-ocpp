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

#ifndef OPENOCPP_CERTIFICATEHASHDATATYPECONVERTER_H
#define OPENOCPP_CERTIFICATEHASHDATATYPECONVERTER_H

#include "CertificateHashDataType.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{

/** @brief Converter class for CertificateHashDataType type */
class CertificateHashDataTypeConverter : public IMessageConverter<ocpp::types::CertificateHashDataType>
{
  public:
    /** @copydoc IMessageConverter<ocpp::types::CertificateHashDataType>* IMessageConverter<ocpp::types::CertificateHashDataType>::clone() const */
    IMessageConverter<ocpp::types::CertificateHashDataType>* clone() const override { return new CertificateHashDataTypeConverter(); }

    /** @copydoc bool IMessageConverter<ocpp::types::CertificateHashDataType>::fromJson(const rapidjson::Value&,
    *                                                                                   ocpp::types::CertificateHashDataType&,
    *                                                                                   std::string&,
    *                                                                                   std::string&) */
    bool fromJson(const rapidjson::Value&               json,
                  ocpp::types::CertificateHashDataType& data,
                  std::string&                          error_code,
                  std::string&                          error_message) override;

    /** @copydoc bool IMessageConverter<ocpp::types::CertificateHashDataType>::toJson(const ocpp::types::CertificateHashDataType&,
     *                                                                                rapidjson::Document&) */
    bool toJson(const ocpp::types::CertificateHashDataType& data, rapidjson::Document& json) override;
};

} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_CERTIFICATEHASHDATATYPECONVERTER_H
