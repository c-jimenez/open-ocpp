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

#ifndef OPENOCPP_OCPP20_DEVICEMODELMANAGER20_H
#define OPENOCPP_OCPP20_DEVICEMODELMANAGER20_H

#include "IChargePointConfig20.h"
#include "IDeviceModel20.h"
#include "JsonValidator.h"

namespace ocpp
{
namespace chargepoint
{
namespace ocpp20
{

/** @brief Handle of the device model operations */
class DeviceModelManager : public IDeviceModel
{
  public:
    /** @brief Constructor */
    DeviceModelManager(const ocpp::config::IChargePointConfig20& stack_config);

    /** @brief Destructor */
    virtual ~DeviceModelManager();

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

    // IDeviceModel interface

    /** @brief Register a listener to device model events */
    void registerListener(IListener& listener) override { m_listener = &listener; }

    /** @brief Get the full device model */
    const DeviceModel& getModel() const override { return m_device_model; }

    /** @brief Get a variable value in the device model */
    ocpp::types::ocpp20::GetVariableResultType getVariable(const ocpp::types::ocpp20::GetVariableDataType& requested_var) override;

    /** @brief Set a variable value in the device model */
    ocpp::types::ocpp20::SetVariableResultType setVariable(const ocpp::types::ocpp20::SetVariableDataType& requested_var) override;

    /** @brief Update a variable value in the device model without value or mutability check */
    ocpp::types::ocpp20::SetVariableResultType updateVariable(const ocpp::types::ocpp20::SetVariableDataType& requested_var) override;

  private:
    /** @brief Stack configuration */
    const ocpp::config::IChargePointConfig20& m_stack_config;
    /** @brief JSON schema validator */
    ocpp::json::JsonValidator m_validator;
    /** @brief Last error message */
    std::string m_last_error;
    /** @brief Device model */
    DeviceModel m_device_model;
    /** @brief Listener to device model events */
    IListener* m_listener;

    /** @brief Load the device model from its JSON representation */
    void loadDeviceModel(const rapidjson::Document& device_model_doc);
    /** @brief Save the device model to a JSON representation */
    void saveDeviceModel(rapidjson::Document& device_model_doc);

    /** @brief Look for a component in the device model */
    const Component* getComponent(const ocpp::types::ocpp20::ComponentType& requested_component);
    /** @brief Look for a variable in the device model */
    const Variable* getVariable(const Component&                                                     component,
                                const ocpp::types::Optional<ocpp::types::ocpp20::AttributeEnumType>& attribute,
                                const ocpp::types::ocpp20::VariableType&                             requested_var,
                                bool&                                                                not_supported_attribute_type);
    /** @brief Check the validity of the value to set to a variable */
    bool isValidValue(const Variable& var, const std::string& value);
    /** @brief Set a variable value in the device model */
    ocpp::types::ocpp20::SetVariableResultType setVariable(const ocpp::types::ocpp20::SetVariableDataType& requested_var, bool check_value);
};

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_OCPP20_DEVICEMODELMANAGER20_H
