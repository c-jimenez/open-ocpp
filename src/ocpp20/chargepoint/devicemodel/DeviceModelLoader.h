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

#ifndef OPENOCPP_OCPP20_DEVICEMODELLOADER_H
#define OPENOCPP_OCPP20_DEVICEMODELLOADER_H

#include "IChargePointConfig20.h"
#include "JsonValidator.h"
#include "Optional.h"
#include "VariableAttributeType20.h"
#include "VariableCharacteristicsType20.h"

#include <map>

namespace ocpp
{
namespace chargepoint
{
namespace ocpp20
{

/** @brief Variable */
struct Variable
{
    /** @brief Name */
    std::string name;
    /** @brief EVSE id */
    ocpp::types::Optional<unsigned int> evse;
    /** @brief Connector id */
    ocpp::types::Optional<unsigned int> connector;
    /** @brief Instance */
    ocpp::types::Optional<std::string> instance;
    /** @brief Attributes */
    ocpp::types::ocpp20::VariableAttributeType attributes;
    /** @brief Characteristics */
    ocpp::types::ocpp20::VariableCharacteristicsType characteristics;
};

/** @brief Component */
struct Component
{
    /** @brief Name */
    std::string name;
    /** @brief Variables */
    std::map<std::string, std::map<std::string, Variable>> variables;
};

/** @brief Helper class to load/store the device model */
class DeviceModelLoader
{
  public:
    /** @brief Constructor */
    DeviceModelLoader(const ocpp::config::IChargePointConfig20& stack_config);

    /** @brief Destructor */
    virtual ~DeviceModelLoader();

    /** 
     * @brief Initialize the device model loader
     * @return true if the initialization was succesful, fals otherwise
     */
    bool init();

    /**
     * @brief Load the device model from a file
     * @param device_model_file_path Path to the device model file
     * @return true if the device model has been loaded, false otherwise
     */
    bool load(const std::string& device_model_file_path);

    /**
     * @brief Save the device model to a file
     * @param device_model_file_path Path to the device model file
     * @return true if the device model has been saved, false otherwise
     */
    bool save(const std::string& device_model_file_path);

    /** @brief Get the last error message */
    const std::string& lastError() const { return m_last_error; }

    /** @brief Get the device model */
    const std::map<std::string, Component>& deviceModel() const { return m_components; }

  private:
    /** @brief Stack configuration */
    const ocpp::config::IChargePointConfig20& m_stack_config;
    /** @brief JSON schema validator */
    ocpp::json::JsonValidator m_validator;
    /** @brief Last error message */
    std::string m_last_error;
    /** @brief Components in the device model */
    std::map<std::string, Component> m_components;

    /** @brief Load the device model from its JSON representation */
    void loadDeviceModel(const rapidjson::Document& device_model_doc);
    /** @brief Save the device model to a JSON representation */
    void saveDeviceModel(rapidjson::Document& device_model_doc);
};

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_OCPP20_CHARGEPOINT20_H
