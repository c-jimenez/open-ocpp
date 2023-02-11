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
        // Read the whole file
        std::string     json;
        char            buffer[1024u];
        std::streamsize size;
        do
        {
            file.read(buffer, sizeof(buffer) - 1u);
            size         = file.gcount();
            buffer[size] = 0;
            json.append(buffer);
        } while (size != 0);

        // Parse JSON schema
        rapidjson::Document schema_doc;
        schema_doc.Parse(json.c_str());
        rapidjson::ParseErrorCode error = schema_doc.GetParseError();
        if (error == rapidjson::ParseErrorCode ::kParseErrorNone)
        {
            // Instanciate validator
            m_schema     = std::make_unique<rapidjson::SchemaDocument>(schema_doc);
            m_last_error = "";
            ret          = true;
        }
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
        const char* invalid_keyword = validator.GetInvalidSchemaKeyword();
        if (invalid_keyword)
        {
            m_last_error = "Error on keyword : " + std::string(invalid_keyword);
        }
        else
        {
            m_last_error = "Unknown error";
        }
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
