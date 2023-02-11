/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License version 2.1
as published by the Free Software Foundation.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPENOCPP_CERTIFICATEHASHDATACHAINTYPECONVERTER_H
#define OPENOCPP_CERTIFICATEHASHDATACHAINTYPECONVERTER_H

#include "CertificateHashDataChainType.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{

/** @brief Converter class for CertificateHashDataChainType type */
class CertificateHashDataChainTypeConverter : public IMessageConverter<ocpp::types::CertificateHashDataChainType>
{
  public:
    /** @copydoc IMessageConverter<ocpp::types::CertificateHashDataChainType>* IMessageConverter<ocpp::types::CertificateHashDataChainType>::clone() const */
    IMessageConverter<ocpp::types::CertificateHashDataChainType>* clone() const override
    {
        return new CertificateHashDataChainTypeConverter();
    }

    /** @copydoc bool IMessageConverter<ocpp::types::CertificateHashDataChainType>::fromJson(const rapidjson::Value&,
    *                                                                                        ocpp::types::CertificateHashDataChainType&,
    *                                                                                        std::string&,
    *                                                                                        std::string&) */
    bool fromJson(const rapidjson::Value&                    json,
                  ocpp::types::CertificateHashDataChainType& data,
                  std::string&                               error_code,
                  std::string&                               error_message) override;

    /** @copydoc bool IMessageConverter<ocpp::types::CertificateHashDataChainType>::toJson(const ocpp::types::CertificateHashDataChainType&,
     *                                                                                     rapidjson::Document&) */
    bool toJson(const ocpp::types::CertificateHashDataChainType& data, rapidjson::Document& json) override;
};

} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_CERTIFICATEHASHDATACHAINTYPECONVERTER_H
