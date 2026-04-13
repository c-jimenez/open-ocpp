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

#ifndef OPENOCPP_JSONVALIDATOR_H
#define OPENOCPP_JSONVALIDATOR_H

#include "json.h"

#include <memory>
#include <string>

namespace ocpp
{
namespace json
{

/** @brief Helper class to validate JSON schemas */
class JsonValidator
{
  public:
    /** @brief Constructor */
    JsonValidator();

    /** @brief Destructor */
    virtual ~JsonValidator();

    /** @brief Initialize the validator with a specific JSON schema file */
    bool init(const std::string& schema_file);

    /** @brief Validate a JSON document according to the schema file */
    bool isValid(const rapidjson::Value& json_document);

    /** @brief Get the last error message */
    const std::string& lastError() const;

  private:
    /** @brief Schema document */
    std::unique_ptr<rapidjson::SchemaDocument> m_schema;
    /** @brief Last error message */
    std::string m_last_error;
};

} // namespace json
} // namespace ocpp

#endif // OPENOCPP_JSONVALIDATOR_H
