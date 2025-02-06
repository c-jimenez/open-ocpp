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

#ifndef OPENOCPP_DEFAULTBASICCHARGEPOINTEVENTSHANDLERCHARGEPOINT_H
#define OPENOCPP_DEFAULTBASICCHARGEPOINTEVENTSHANDLERCHARGEPOINT_H

#include "IBasicChargePointEventsHandler20.h"
#include "IDeviceModel20.h"

#include <filesystem>

class ChargePointDemoConfig;

/** @brief Interface for charge point event handlers implementations */
class DefaultBasicChargePointEventsHandler : public ocpp::chargepoint::ocpp20::IBasicChargePointEventsHandler,
                                             public ocpp::chargepoint::ocpp20::IDeviceModel::IListener
{
  public:
    /** @brief Constructor */
    DefaultBasicChargePointEventsHandler(ChargePointDemoConfig& config, const std::filesystem::path& working_dir);

    /** @brief Destructor */
    virtual ~DefaultBasicChargePointEventsHandler();

    /** @copydoc void IChargePointEventsHandler20::connectionFailed(ocpp::types::ocpp20::RegistrationStatusEnumType) */
    void connectionFailed(ocpp::types::ocpp20::RegistrationStatusEnumType status) override;

    /** @copydoc void IChargePointEventsHandler20::connectionStateChanged(bool) */
    void connectionStateChanged(bool isConnected) override;

    /** @copydoc void IChargePointEventsHandler20::bootNotification(ocpp::types::ocpp20::RegistrationStatusEnumType, const ocpp::types::DateTime&) */
    void bootNotification(ocpp::types::ocpp20::RegistrationStatusEnumType status, const ocpp::types::DateTime& datetime) override;

    /** @copydoc void IChargePointEventsHandler20::datetimeReceived(ocpp::types::DateTime) */
    void datetimeReceived(const ocpp::types::DateTime& datetime) override;

    // IDeviceModel interface

    /** @brief Called to retrieve the value of a variable */
    void getVariable(ocpp::types::ocpp20::GetVariableResultType& var) override;

    /** @brief Called to set the value of a variable */
    ocpp::types::ocpp20::SetVariableStatusEnumType setVariable(const ocpp::types::ocpp20::SetVariableDataType& var) override;

  private:
    /** @brief Configuration */
    ChargePointDemoConfig& m_config;
    /** @brief Working directory */
    std::filesystem::path m_working_dir;
    /** @brief Indicate if the Charge Point is connected */
    bool m_is_connected;
};

#endif // OPENOCPP_DEFAULTBASICCHARGEPOINTEVENTSHANDLERCHARGEPOINT_H
