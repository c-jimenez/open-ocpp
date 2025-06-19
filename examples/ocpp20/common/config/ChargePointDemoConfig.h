/*
MIT License

Copyright (c) 2020 Cedric Jimenez

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef CHARGEPOINTDEMOCONFIG_H
#define CHARGEPOINTDEMOCONFIG_H

#include "ChargePointConfig.h"
#include "IDeviceModel20.h"
#include "IniFile.h"

/** @brief Configuration of the Charge Point demo */
class ChargePointDemoConfig
{
  public:
    /** @brief Constructor */
    ChargePointDemoConfig(const std::string& config_file) : m_config(config_file), m_stack_config(m_config) { }

    /** @brief Stack internal configuration */
    ocpp::config::IChargePointConfig20& stackConfig() { return m_stack_config; }

    /** @brief Set the value of a stack internal configuration key */
    void setStackConfigValue(const std::string& key, const std::string& value) { m_stack_config.setConfigValue(key, value); }

    /** @brief Get the value of a variable of the device model */
    bool getDeviceModelValue(const ocpp::types::ocpp20::ComponentType& component,
                             const ocpp::types::ocpp20::VariableType&  variable,
                             std::string&                              value)
    {
        auto ini_value = m_config.get(buildComponentName(component), buildVariableName(variable));
        value          = ini_value.toString();
        return true;
    }

    /** @brief Set the value of a variable of the device model */
    bool setDeviceModelValue(const ocpp::types::ocpp20::ComponentType& component,
                             const ocpp::types::ocpp20::VariableType&  variable,
                             const std::string&                        value)
    {
        m_config.set(buildComponentName(component), buildVariableName(variable), value);
        m_config.store();
        return true;
    }

    /** @brief Save the configuration */
    bool save() { return m_config.store(); }

  private:
    /** @brief Configuration file */
    ocpp::helpers::IniFile m_config;

    /** @brief Stack internal configuration */
    ChargePointConfig m_stack_config;

    /** @brief Build the device model component unique name */
    std::string buildComponentName(const ocpp::types::ocpp20::ComponentType& component)
    {
        std::string name = component.name;
        if (component.instance.isSet())
        {
            name += "." + component.instance.value().str();
        }
        if (component.evse.isSet())
        {
            name += "." + std::to_string(component.evse.value().id);
            if (component.evse.value().connectorId.isSet())
            {
                name += "." + std::to_string(component.evse.value().connectorId.value());
            }
        }
        return name;
    }

    /** @brief Build the device model variable unique name */
    std::string buildVariableName(const ocpp::types::ocpp20::VariableType& variable)
    {
        std::string name = variable.name;
        if (variable.instance.isSet())
        {
            name += "." + variable.instance.value().str();
        }
        return name;
    }
};

#endif // CHARGEPOINTDEMOCONFIG_H
