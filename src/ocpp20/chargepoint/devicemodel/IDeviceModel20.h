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

#ifndef OPENOCPP_OCPP20_IDEVICEMODEL20_H
#define OPENOCPP_OCPP20_IDEVICEMODEL20_H

#include "GetVariableDataType20.h"
#include "GetVariableResultType20.h"
#include "SetVariableDataType20.h"
#include "SetVariableResultType20.h"
#include "VariableAttributeType20.h"
#include "VariableCharacteristicsType20.h"

#include <map>
#include <vector>

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
    /** @brief Instance */
    ocpp::types::Optional<std::string> instance;
    /** @brief EVSE id */
    ocpp::types::Optional<unsigned int> evse;
    /** @brief Connector id */
    ocpp::types::Optional<unsigned int> connector;
    /** @brief Variables */
    std::map<std::string, std::map<std::string, Variable>> variables;
};

/** @brief Device model */
struct DeviceModel
{
    /** @brief Components in the device model */
    std::map<std::string, std::vector<Component>> components;
};

/** @brief Interface to interact with the device model */
class IDeviceModel
{
  public:
    // Forward declaration
    class IListener;

    /** @brief Destructor */
    virtual ~IDeviceModel() { }

    /** @brief Register a listener to device model events */
    virtual void registerListener(IListener& listener) = 0;

    /** @brief Get the full device model */
    virtual const DeviceModel& getModel() const = 0;

    /** @brief Get a variable value in the device model */
    virtual ocpp::types::ocpp20::GetVariableResultType getVariable(const ocpp::types::ocpp20::GetVariableDataType& requested_var) = 0;

    /** @brief Set a variable value in the device model */
    virtual ocpp::types::ocpp20::SetVariableResultType setVariable(const ocpp::types::ocpp20::SetVariableDataType& requested_var) = 0;

    /** @brief Update a variable value in the device model without value or mutability check */
    virtual ocpp::types::ocpp20::SetVariableResultType updateVariable(const ocpp::types::ocpp20::SetVariableDataType& requested_var) = 0;

    /** @brief Interface to the listeners of the device model events */
    class IListener
    {
      public:
        /** @brief Destructor */
        virtual ~IListener() { }

        /** @brief Called to retrieve the value of a variable */
        virtual void getVariable(ocpp::types::ocpp20::GetVariableResultType& var) = 0;
        /** @brief Called to set the value of a variable */
        virtual ocpp::types::ocpp20::SetVariableStatusEnumType setVariable(const ocpp::types::ocpp20::SetVariableDataType& var) = 0;
    };
};

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_OCPP20_IDEVICEMODEL20_H
