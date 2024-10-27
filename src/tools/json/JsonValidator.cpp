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

#include "JsonValidator.h"

#include <fstream>

namespace ocpp
{
namespace json
{

/** @brief Constructor */
JsonValidator::JsonValidator() : m_schema(nullptr), m_last_error() { }

/** @brief Destructor */
JsonValidator::~JsonValidator() { }

/** @brief Initialize the validator with a specific JSON schema file */
bool JsonValidator::init(const std::string& schema_file)
{
    bool ret = false;

    // Open schema file
    std::ifstream file;
    file.open(schema_file);
    if (file.is_open())
    {
        // Parse JSON schema
        rapidjson::IStreamWrapper file_wrapper(file);
        rapidjson::Document       schema_doc;
        schema_doc.ParseStream(file_wrapper);
        rapidjson::ParseErrorCode error = schema_doc.GetParseError();
        if (error == rapidjson::ParseErrorCode ::kParseErrorNone)
        {
            // Instanciate validator
            m_schema     = std::make_unique<rapidjson::SchemaDocument>(schema_doc);
            m_last_error = "";
            ret          = true;
        }
        else
        {
            m_last_error = GetParseError_En(error);
        }
    }
    else
    {
        m_last_error = "Unable to open schema file : " + schema_file;
    }

    return ret;
}

/** @brief Validate a JSON document according to the schema file */
bool JsonValidator::isValid(const rapidjson::Value& json_document)
{
    bool ret = false;

    // Instanciate validator
    rapidjson::SchemaValidator validator(*(m_schema.get()));

    // Validate document
    ret = json_document.Accept(validator);
    if (!ret)
    {
        m_last_error = GetValidateError_En(validator.GetInvalidSchemaCode());
    }

    return ret;
}

/** @brief Get the last error message */
const std::string& JsonValidator::lastError() const
{
    return m_last_error;
}

} // namespace json
} // namespace ocpp
