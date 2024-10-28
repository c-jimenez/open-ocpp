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

#include "DeviceModelManager20.h"
#include "StringHelpers.h"

#include <filesystem>
#include <fstream>

using namespace ocpp::types;
using namespace ocpp::types::ocpp20;

namespace ocpp
{
namespace chargepoint
{
namespace ocpp20
{

/** @brief Constructor */
DeviceModelManager20::DeviceModelManager20(const ocpp::config::IChargePointConfig20& stack_config)
    : m_stack_config(stack_config), m_validator(), m_last_error(), m_device_model(), m_listener(nullptr)
{
}

/** @brief Destructor */
DeviceModelManager20::~DeviceModelManager20() { }

/** @brief Initialize the device model loader */
bool DeviceModelManager20::init()
{
    bool ret = false;

    // Initialize JSON schema validator
    std::filesystem::path device_model_schema_path = m_stack_config.jsonSchemasPath();
    device_model_schema_path.append("device_model_schema.json");
    ret = m_validator.init(device_model_schema_path);
    if (!ret)
    {
        m_last_error = m_validator.lastError();
    }

    return ret;
}

/** @brief Load the device model from a file */
bool DeviceModelManager20::load(const std::string& device_model_file_path)
{
    bool ret = false;

    // Open the device model file
    std::ifstream device_model_file;
    device_model_file.open(device_model_file_path);
    if (device_model_file.is_open())
    {
        // Parse JSON contents
        rapidjson::IStreamWrapper device_model_file_wrapper(device_model_file);
        rapidjson::Document       device_model_doc;
        device_model_doc.ParseStream(device_model_file_wrapper);
        rapidjson::ParseErrorCode error = device_model_doc.GetParseError();
        if (error == rapidjson::ParseErrorCode ::kParseErrorNone)
        {
            // Validate schema
            ret = m_validator.isValid(device_model_doc);
            if (ret)
            {
                // Load the device model
                loadDeviceModel(device_model_doc);
            }
            else
            {
                m_last_error = m_validator.lastError();
            }
        }
        else
        {
            m_last_error = GetParseError_En(error);
        }
    }
    else
    {
        m_last_error = "Unable to open device model file : " + device_model_file_path;
    }

    return ret;
}

/** @brief Save the device model to a file */
bool DeviceModelManager20::save(const std::string& device_model_file_path)
{
    bool ret = false;

    // Open the device model file
    std::ofstream device_model_file;
    device_model_file.open(device_model_file_path);
    if (device_model_file.is_open())
    {
        // Convert the device model to JSON
        rapidjson::Document device_model_doc(rapidjson::kObjectType);
        saveDeviceModel(device_model_doc);

        // Save JSON contents
        rapidjson::OStreamWrapper                    device_model_file_wrapper(device_model_file);
        rapidjson::Writer<rapidjson::OStreamWrapper> device_model_file_writer(device_model_file_wrapper);
        ret = device_model_doc.Accept(device_model_file_writer);
        if (!ret)
        {
            m_last_error = "Unable to write to device model file : " + device_model_file_path;
        }
    }
    else
    {
        m_last_error = "Unable to open device model file : " + device_model_file_path;
    }

    return ret;
}

// IDeviceModel interface

/** @brief Get a variable value in the device model */
ocpp::types::ocpp20::GetVariableResultType DeviceModelManager20::getVariable(const ocpp::types::ocpp20::GetVariableDataType& requested_var)
{
    GetVariableResultType result;

    // Get requested component
    const Component* component = getComponent(requested_var.component);
    if (component)
    {
        // Get the requested variable
        bool            not_supported_attribute_type = false;
        const Variable* var = getVariable(*component, requested_var.attributeType, requested_var.variable, not_supported_attribute_type);
        if (var)
        {
            // Notify request
            result.component       = requested_var.component;
            result.variable        = requested_var.variable;
            result.attributeType   = requested_var.attributeType;
            result.attributeStatus = GetVariableStatusEnumType::Rejected;
            if (m_listener)
            {
                m_listener->getVariable(result);
            }
        }
        else
        {
            if (not_supported_attribute_type)
            {
                result.attributeStatus = GetVariableStatusEnumType::NotSupportedAttributeType;
            }
            else
            {
                result.attributeStatus = GetVariableStatusEnumType::UnknownVariable;
            }
        }
    }
    else
    {
        result.attributeStatus = GetVariableStatusEnumType::UnknownComponent;
    }

    return result;
}

/** @brief Set a variable value in the device model */
ocpp::types::ocpp20::SetVariableResultType DeviceModelManager20::setVariable(const ocpp::types::ocpp20::SetVariableDataType& requested_var)
{
    SetVariableResultType result;

    // Get requested component
    const Component* component = getComponent(requested_var.component);
    if (component)
    {
        // Get the requested variable
        bool            not_supported_attribute_type = false;
        const Variable* var = getVariable(*component, requested_var.attributeType, requested_var.variable, not_supported_attribute_type);
        if (var)
        {
            // Check value
            result.component       = requested_var.component;
            result.variable        = requested_var.variable;
            result.attributeType   = requested_var.attributeType;
            result.attributeStatus = SetVariableStatusEnumType::Rejected;
            if (isValidValue(*var, requested_var.attributeValue.str()))
            {
                // Notify request
                result.component       = requested_var.component;
                result.variable        = requested_var.variable;
                result.attributeType   = requested_var.attributeType;
                result.attributeStatus = SetVariableStatusEnumType::Rejected;
                if (m_listener)
                {
                    result.attributeStatus = m_listener->setVariable(requested_var);
                }
            }
        }
        else
        {
            if (not_supported_attribute_type)
            {
                result.attributeStatus = SetVariableStatusEnumType::NotSupportedAttributeType;
            }
            else
            {
                result.attributeStatus = SetVariableStatusEnumType::UnknownVariable;
            }
        }
    }
    else
    {
        result.attributeStatus = SetVariableStatusEnumType::UnknownComponent;
    }

    return result;
}

/** @brief Load the device model from its JSON representation */
void DeviceModelManager20::loadDeviceModel(const rapidjson::Document& device_model_doc)
{
    // Clear device model
    m_device_model.components.clear();

    // For each component
    const rapidjson::Value& components_json = device_model_doc["components"];
    for (auto it_comps = components_json.Begin(); it_comps != components_json.End(); ++it_comps)
    {
        Component               component;
        const rapidjson::Value& component_json = *it_comps;

        // Name
        component.name = component_json["name"].GetString();

        // EVSE id
        if (component_json.HasMember("evse"))
        {
            component.evse.value() = component_json["evse"].GetUint();
        }

        // Connector id
        if (component_json.HasMember("connector"))
        {
            component.connector.value() = component_json["connector"].GetUint();
        }

        // Instance
        if (component_json.HasMember("instance"))
        {
            component.instance.value() = component_json["instance"].GetString();
        }

        // For each variable
        const rapidjson::Value& variables_json = component_json["variables"];
        for (auto it_vars = variables_json.Begin(); it_vars != variables_json.End(); ++it_vars)
        {
            Variable                var;
            const rapidjson::Value& var_json = *it_vars;

            // Name
            var.name = var_json["name"].GetString();

            // Instance
            if (var_json.HasMember("instance"))
            {
                var.instance.value() = var_json["instance"].GetString();
            }

            // Attributes
            {
                const rapidjson::Value& var_attributes_json = var_json["attributes"];

                // Type
                if (var_attributes_json.HasMember("type"))
                {
                    var.attributes.type.value() = AttributeEnumTypeHelper.fromString(var_attributes_json["type"].GetString());
                }
                else
                {
                    var.attributes.type.value() = AttributeEnumType::Actual;
                }

                // Mutability
                var.attributes.mutability.value() = MutabilityEnumTypeHelper.fromString(var_attributes_json["mutability"].GetString());

                // Persistent flag
                if (var_attributes_json.HasMember("persistent"))
                {
                    var.attributes.persistent.value() = var_attributes_json["persistent"].GetBool();
                }
                else
                {
                    var.attributes.persistent.value() = true;
                }

                // Constant flag
                if (var_attributes_json.HasMember("constant"))
                {
                    var.attributes.constant.value() = var_attributes_json["constant"].GetBool();
                }
                else
                {
                    var.attributes.constant.value() = false;
                }
            }

            // Characteristics
            {
                const rapidjson::Value& var_chars_json = var_json["characteristics"];

                // Data type
                var.characteristics.dataType = DataEnumTypeHelper.fromString(var_chars_json["data_type"].GetString());

                // Unit
                if (var_chars_json.HasMember("unit"))
                {
                    var.characteristics.unit.value().assign(var_chars_json["unit"].GetString());
                }

                // Min limit
                if (var_chars_json.HasMember("min_limit"))
                {
                    var.characteristics.minLimit.value() = var_chars_json["min_limit"].GetFloat();
                }

                // Max limit
                if (var_chars_json.HasMember("max_limit"))
                {
                    var.characteristics.maxLimit.value() = var_chars_json["max_limit"].GetFloat();
                }

                // Values list
                if (var_chars_json.HasMember("values_list"))
                {
                    var.characteristics.valuesList.value().assign(var_chars_json["values_list"].GetString());
                }

                // Supports monitoring flag
                if (var_chars_json.HasMember("supports_monitoring"))
                {
                    var.characteristics.supportsMonitoring = var_chars_json["supports_monitoring"].GetBool();
                }
                else
                {
                    var.characteristics.supportsMonitoring = true;
                }
            }

            // Add to list of variables
            const Variable& cvar     = var;
            auto            iter_var = component.variables.find(var.name);
            if (iter_var != component.variables.end())
            {

                iter_var->second[cvar.instance.value()] = std::move(var);
            }
            else
            {
                std::map<std::string, Variable> vars;
                vars[cvar.instance.value()]                    = std::move(var);
                component.variables[vars.begin()->second.name] = std::move(vars);
            }
        }

        // Add to list of components
        auto iter_comp = m_device_model.components.find(component.name);
        if (iter_comp != m_device_model.components.end())
        {

            iter_comp->second.push_back(std::move(component));
        }
        else
        {
            std::vector<Component> comps;
            comps.push_back(std::move(component));
            m_device_model.components[comps.front().name] = std::move(comps);
        }
    }
}

/** @brief Save the device model to a JSON representation */
void DeviceModelManager20::saveDeviceModel(rapidjson::Document& device_model_doc)
{
    RAPIDJSON_DEFAULT_ALLOCATOR& allocator = device_model_doc.GetAllocator();

    // Components
    rapidjson::Value components_json(rapidjson::kArrayType);

    // For each component
    for (const auto& [_, components] : m_device_model.components)
    {
        for (const auto& component : components)
        {
            rapidjson::Value component_json(rapidjson::kObjectType);

            // Name
            component_json.AddMember(rapidjson::StringRef("name"), rapidjson::Value(component.name.c_str(), allocator).Move(), allocator);

            // EVSE id
            if (component.evse.isSet())
            {
                component_json.AddMember(rapidjson::StringRef("evse"), rapidjson::Value(component.evse.value()).Move(), allocator);
            }

            // Connector id
            if (component.connector.isSet())
            {
                component_json.AddMember(
                    rapidjson::StringRef("connector"), rapidjson::Value(component.connector.value()).Move(), allocator);
            }

            // Instance
            if (component.instance.isSet())
            {
                component_json.AddMember(
                    rapidjson::StringRef("instance"), rapidjson::Value(component.instance.value().c_str(), allocator).Move(), allocator);
            }

            // For each variable
            rapidjson::Value variables_json(rapidjson::kArrayType);
            for (const auto& [_, vars] : component.variables)
            {
                for (const auto& [_, var] : vars)
                {
                    rapidjson::Value var_json(rapidjson::kObjectType);

                    // Name
                    var_json.AddMember(rapidjson::StringRef("name"), rapidjson::Value(var.name.c_str(), allocator).Move(), allocator);

                    // Instance
                    if (var.instance.isSet())
                    {
                        var_json.AddMember(
                            rapidjson::StringRef("instance"), rapidjson::Value(var.instance.value().c_str(), allocator).Move(), allocator);
                    }

                    // Attributes
                    {
                        rapidjson::Value var_attributes_json(rapidjson::kObjectType);

                        // Type
                        if (var.attributes.type.isSet())
                        {
                            var_attributes_json.AddMember(
                                rapidjson::StringRef("type"),
                                rapidjson::Value(AttributeEnumTypeHelper.toString(var.attributes.type.value()).c_str(), allocator).Move(),
                                allocator);
                        }

                        // Mutability
                        if (var.attributes.mutability.isSet())
                        {
                            var_attributes_json.AddMember(
                                rapidjson::StringRef("mutability"),
                                rapidjson::Value(MutabilityEnumTypeHelper.toString(var.attributes.mutability.value()).c_str(), allocator)
                                    .Move(),
                                allocator);
                        }

                        // Persistent flag
                        if (var.attributes.persistent.isSet())
                        {
                            var_attributes_json.AddMember(
                                rapidjson::StringRef("persistent"), rapidjson::Value(var.attributes.persistent.value()).Move(), allocator);
                        }

                        // Constant flag
                        if (var.attributes.constant.isSet())
                        {
                            var_attributes_json.AddMember(
                                rapidjson::StringRef("constant"), rapidjson::Value(var.attributes.constant.value()).Move(), allocator);
                        }

                        var_json.AddMember(rapidjson::StringRef("attributes"), var_attributes_json.Move(), allocator);
                    }

                    // Characteristics
                    {
                        rapidjson::Value var_chars_json(rapidjson::kObjectType);

                        // Data type
                        var_chars_json.AddMember(
                            rapidjson::StringRef("data_type"),
                            rapidjson::Value(DataEnumTypeHelper.toString(var.characteristics.dataType).c_str(), allocator).Move(),
                            allocator);

                        // Unit
                        if (var.characteristics.unit.isSet())
                        {
                            var_chars_json.AddMember(rapidjson::StringRef("unit"),
                                                     rapidjson::Value(var.characteristics.unit.value().c_str(), allocator).Move(),
                                                     allocator);
                        }

                        // Min limit
                        if (var.characteristics.minLimit.isSet())
                        {
                            var_chars_json.AddMember(rapidjson::StringRef("min_limit"),
                                                     rapidjson::Value(var.characteristics.minLimit.value()).Move(),
                                                     allocator);
                        }

                        // Max limit
                        if (var.characteristics.maxLimit.isSet())
                        {
                            var_chars_json.AddMember(rapidjson::StringRef("max_limit"),
                                                     rapidjson::Value(var.characteristics.maxLimit.value()).Move(),
                                                     allocator);
                        }

                        // Values list
                        if (var.characteristics.valuesList.isSet())
                        {
                            var_chars_json.AddMember(rapidjson::StringRef("values_list"),
                                                     rapidjson::Value(var.characteristics.valuesList.value().c_str(), allocator).Move(),
                                                     allocator);
                        }

                        // Supports monitoring flag
                        var_chars_json.AddMember(rapidjson::StringRef("supports_monitoring"),
                                                 rapidjson::Value(var.characteristics.supportsMonitoring).Move(),
                                                 allocator);

                        var_json.AddMember(rapidjson::StringRef("characteristics"), var_chars_json.Move(), allocator);
                    }

                    variables_json.PushBack(var_json.Move(), allocator);
                }
            }
            component_json.AddMember(rapidjson::StringRef("variables"), variables_json.Move(), allocator);

            components_json.PushBack(component_json.Move(), allocator);
        }
    }

    device_model_doc.AddMember(rapidjson::StringRef("components"), components_json.Move(), allocator);
}

/** @brief Look for a component in the device model */
const Component* DeviceModelManager20::getComponent(const ocpp::types::ocpp20::ComponentType& requested_component)
{
    const Component* component = nullptr;

    auto iter_comp = m_device_model.components.find(requested_component.name);
    if (iter_comp != m_device_model.components.end())
    {
        for (const auto& comp : iter_comp->second)
        {
            if ((!requested_component.instance.isSet() || (requested_component.instance.value() == comp.instance.value())) &&
                (!requested_component.evse.isSet() ||
                 ((requested_component.evse.value().id == static_cast<int>(comp.evse.value())) &&
                  (!requested_component.evse.value().connectorId.isSet() ||
                   (requested_component.evse.value().connectorId.value() == static_cast<int>(comp.connector.value()))))))
            {
                component = &comp;
                break;
            }
        }
    }

    return component;
}

/** @brief Look for a variable in the device model */
const Variable* DeviceModelManager20::getVariable(const Component&                                                     component,
                                                  const ocpp::types::Optional<ocpp::types::ocpp20::AttributeEnumType>& attribute,
                                                  const ocpp::types::ocpp20::VariableType&                             requested_var,
                                                  bool& not_supported_attribute_type)

{
    const Variable* var          = nullptr;
    not_supported_attribute_type = false;

    auto iter_var = component.variables.find(requested_var.name);
    if (iter_var != component.variables.end())
    {
        auto iter_instance = iter_var->second.find(requested_var.instance.value());
        if (iter_instance != iter_var->second.end())
        {
            AttributeEnumType attribute_type = (attribute.isSet() ? attribute.value() : AttributeEnumType::Actual);
            if (iter_instance->second.attributes.type.value() == attribute_type)
            {
                var = &iter_instance->second;
            }
            else
            {
                not_supported_attribute_type = true;
            }
        }
    }

    return var;
}

/** @brief Check the validity of the value to set to a variable */
bool DeviceModelManager20::isValidValue(const Variable& var, const std::string& value)
{
    bool ret = true;

    if (var.characteristics.minLimit.isSet())
    {
        switch (var.characteristics.dataType)
        {
            case DataEnumType::integer:
                [[fallthrough]];
            case DataEnumType::decimal:
            {
                float float_value = static_cast<float>(std::atof(value.c_str()));
                if (float_value < var.characteristics.minLimit)
                {
                    m_last_error = "Value is below the minimal limit : " + var.name;
                    ret          = false;
                }
            }
            break;

            default:
                // Don't care
                break;
        }
    }

    if (var.characteristics.maxLimit.isSet())
    {
        switch (var.characteristics.dataType)
        {
            case DataEnumType::integer:
                [[fallthrough]];
            case DataEnumType::decimal:
            {
                float float_value = static_cast<float>(std::atof(value.c_str()));
                if (float_value > var.characteristics.maxLimit)
                {
                    m_last_error = "Value is above the maximal limit : " + var.name;
                    ret          = false;
                }
            }
            break;

            case DataEnumType::string:
                [[fallthrough]];
            case DataEnumType::OptionList:
                [[fallthrough]];
            case DataEnumType::MemberList:
                [[fallthrough]];
            case DataEnumType::SequenceList:
            {
                size_t max_length = static_cast<size_t>(var.characteristics.maxLimit);
                if (value.size() > max_length)
                {
                    m_last_error = "Value length is above the maximal limit : " + var.name;
                    ret          = false;
                }
            }
            break;

            default:
                // Don't care
                break;
        }
    }

    if (var.characteristics.valuesList.isSet() && !var.characteristics.valuesList.value().empty())
    {
        switch (var.characteristics.dataType)
        {
            case DataEnumType::OptionList:
                [[fallthrough]];
            case DataEnumType::MemberList:
                [[fallthrough]];
            case DataEnumType::SequenceList:
            {
                auto allowed_values = ocpp::helpers::split(var.characteristics.valuesList.value(), ',');
                bool found          = false;
                for (auto& allowed_value : allowed_values)
                {
                    allowed_value = ocpp::helpers::trim(allowed_value);
                    if (value == allowed_value)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    m_last_error = "Value is not in the values list : " + var.name;
                    ret          = false;
                }
            }
            break;

            default:
                // Don't care
                break;
        }
    }

    return ret;
}

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp
