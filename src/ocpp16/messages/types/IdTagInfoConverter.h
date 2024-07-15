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

#ifndef OPENOCPP_IDTAGINFOCONVERTER_H
#define OPENOCPP_IDTAGINFOCONVERTER_H

#include "IMessageConverter.h"
#include "IdTagInfo.h"

namespace ocpp
{
namespace messages
{

/** @brief Converter class for IdTagInfo type */
class IdTagInfoConverter : public IMessageConverter<ocpp::types::IdTagInfo>
{
  public:
    /** @copydoc IMessageConverter<ocpp::types::IdTagInfo>* IMessageConverter<ocpp::types::IdTagInfo>::clone() const */
    IMessageConverter<ocpp::types::IdTagInfo>* clone() const override { return new IdTagInfoConverter(); }

    /** @copydoc bool IMessageConverter<ocpp::types::IdTagInfo>::fromJson(const rapidjson::Value&,
     *                                                                    ocpp::types::IdTagInfo&,
     *                                                                    std::string&,
     *                                                                    std::string&) */
    bool fromJson(const rapidjson::Value& json, ocpp::types::IdTagInfo& data, std::string& error_code, std::string& error_message) override;

    /** @copydoc bool IMessageConverter<ocpp::types::IdTagInfo>::toJson(const ocpp::types::IdTagInfo&,
     *                                                                  rapidjson::Document&) */
    bool toJson(const ocpp::types::IdTagInfo& data, rapidjson::Document& json) override;
};

} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_IDTAGINFOCONVERTER_H
