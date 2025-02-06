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

#include "DefaultBasicChargePointEventsHandler.h"
#include "ChargePointDemoConfig.h"

#include <iostream>

using namespace std;
using namespace ocpp::types;
using namespace ocpp::types::ocpp20;

/** @brief Constructor */
DefaultBasicChargePointEventsHandler::DefaultBasicChargePointEventsHandler(ChargePointDemoConfig&       config,
                                                                           const std::filesystem::path& working_dir)
    : m_config(config), m_working_dir(working_dir), m_is_connected(false)
{
}

/** @brief Destructor */
DefaultBasicChargePointEventsHandler::~DefaultBasicChargePointEventsHandler() { }

/** @copydoc void IChargePointEventsHandler20::connectionFailed(ocpp::types::ocpp20::RegistrationStatusEnumType) */
void DefaultBasicChargePointEventsHandler::connectionFailed(ocpp::types::ocpp20::RegistrationStatusEnumType status)
{
    cout << "Connection failed, previous registration status : " << RegistrationStatusEnumTypeHelper.toString(status) << endl;
}

/** @copydoc void IChargePointEventsHandler20::connectionStateChanged(bool) */
void DefaultBasicChargePointEventsHandler::connectionStateChanged(bool isConnected)
{
    cout << "Connection state changed : " << isConnected << endl;
    m_is_connected = isConnected;
}

/** @copydoc void IChargePointEventsHandler20::bootNotification(ocpp::types::ocpp20::RegistrationStatusEnumType, const ocpp::types::DateTime&) */
void DefaultBasicChargePointEventsHandler::bootNotification(ocpp::types::ocpp20::RegistrationStatusEnumType status,
                                                            const ocpp::types::DateTime&                    datetime)
{
    cout << "Bootnotification : " << RegistrationStatusEnumTypeHelper.toString(status) << " - " << datetime.str() << endl;
}

/** @copydoc void IChargePointEventsHandler20::datetimeReceived(ocpp::types::DateTime) */
void DefaultBasicChargePointEventsHandler::datetimeReceived(const ocpp::types::DateTime& datetime)
{
    cout << "Date time received : " << datetime.str() << endl;
}

// IDeviceModel20 interface

/** @brief Called to retrieve the value of a variable */
void DefaultBasicChargePointEventsHandler::getVariable(ocpp::types::ocpp20::GetVariableResultType& var)
{
    std::string value;
    m_config.getDeviceModelValue(var.component, var.variable, value);
    var.attributeValue.value().assign(std::move(value));
    var.attributeStatus = GetVariableStatusEnumType::Accepted;
}

/** @brief Called to set the value of a variable */
ocpp::types::ocpp20::SetVariableStatusEnumType DefaultBasicChargePointEventsHandler::setVariable(
    const ocpp::types::ocpp20::SetVariableDataType& var)
{
    m_config.setDeviceModelValue(var.component, var.variable, var.attributeValue.str());
    return SetVariableStatusEnumType::Accepted;
}
