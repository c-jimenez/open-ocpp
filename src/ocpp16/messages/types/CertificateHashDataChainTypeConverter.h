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

#ifndef OPENOCPP_CERTIFICATEHASHDATACHAINTYPECONVERTER_H
#define OPENOCPP_CERTIFICATEHASHDATACHAINTYPECONVERTER_H

#include "CertificateHashDataChainType.h"
#include "IMessageConverter.h"

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Converter class for CertificateHashDataChainType type */
class CertificateHashDataChainTypeConverter : public IMessageConverter<ocpp::types::ocpp16::CertificateHashDataChainType>
{
  public:
    /** @copydoc IMessageConverter<ocpp::types::ocpp16::CertificateHashDataChainType>* IMessageConverter<ocpp::types::ocpp16::CertificateHashDataChainType>::clone() const */
    IMessageConverter<ocpp::types::ocpp16::CertificateHashDataChainType>* clone() const override
    {
        return new CertificateHashDataChainTypeConverter();
    }

    /** @copydoc bool IMessageConverter<ocpp::types::ocpp16::CertificateHashDataChainType>::fromJson(const rapidjson::Value&,
    *                                                                                        ocpp::types::ocpp16::CertificateHashDataChainType&,
    *                                                                                        std::string&,
    *                                                                                        std::string&) */
    bool fromJson(const rapidjson::Value&                            json,
                  ocpp::types::ocpp16::CertificateHashDataChainType& data,
                  std::string&                                       error_code,
                  std::string&                                       error_message) override;

    /** @copydoc bool IMessageConverter<ocpp::types::ocpp16::CertificateHashDataChainType>::toJson(const ocpp::types::ocpp16::CertificateHashDataChainType&,
     *                                                                                     rapidjson::Document&) */
    bool toJson(const ocpp::types::ocpp16::CertificateHashDataChainType& data, rapidjson::Document& json) override;
};

} // namespace ocpp16
} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_CERTIFICATEHASHDATACHAINTYPECONVERTER_H
