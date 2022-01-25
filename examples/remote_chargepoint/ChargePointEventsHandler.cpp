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

#include "ChargePointEventsHandler.h"
#include "ChargePointDemoConfig.h"
#include "IMeter.h"
#include "ISetpointManager.h"
#include "String.h"

#include <iostream>

using namespace std;
using namespace ocpp::types;

/** @brief Constructor */
ChargePointEventsHandler::ChargePointEventsHandler(ChargePointDemoConfig& config)
    : DefaultChargePointEventsHandler(config), m_meter_simulators(nullptr), m_setpoint_manager(nullptr)
{
}

/** @brief Destructor */
ChargePointEventsHandler::~ChargePointEventsHandler() { }

/** @copydoc unsigned int IChargePointEventsHandler::getTxStartStopMeterValue(unsigned int) */
unsigned int ChargePointEventsHandler::getTxStartStopMeterValue(unsigned int connector_id)
{
    unsigned int ret = 0;
    cout << "Get start/stop meter value for connector " << connector_id << endl;
    if (m_meter_simulators)
    {
        IMeter* meter_simulator = (*m_meter_simulators)[connector_id].get();
        ret                     = static_cast<unsigned int>(meter_simulator->getEnergy());
    }
    return ret;
}

/** @copydoc bool getMeterValue(unsigned int, const std::pair<ocpp::types::Measurand, ocpp::types::Optional<ocpp::types::Phase>>&, ocpp::types::MeterValue&) */
bool ChargePointEventsHandler::getMeterValue(unsigned int connector_id,
                                             const std::pair<ocpp::types::Measurand, ocpp::types::Optional<ocpp::types::Phase>>& measurand,
                                             ocpp::types::MeterValue& meter_value)
{
    bool ret = false;

    cout << "getMeterValue : " << connector_id << " - " << MeasurandHelper.toString(measurand.first) << endl;

    if (m_meter_simulators)
    {
        SampledValue value;
        IMeter*      meter_simulator = (*m_meter_simulators)[connector_id].get();
        ret                          = true;
        switch (measurand.first)
        {
            case Measurand::CurrentImport:
            {
                auto currents = meter_simulator->getCurrents();
                if (measurand.second.isSet())
                {
                    unsigned int phase = static_cast<unsigned int>(measurand.second.value());
                    if (phase <= meter_simulator->getNumberOfPhases())
                    {
                        value.value = std::to_string(currents[phase]);
                        value.phase = static_cast<Phase>(phase);
                        meter_value.sampledValue.push_back(value);
                    }
                    else
                    {
                        ret = false;
                    }
                }
                else
                {
                    for (size_t i = 0; i < currents.size(); i++)
                    {
                        value.value = std::to_string(currents[i]);
                        value.phase = static_cast<Phase>(i);
                        meter_value.sampledValue.push_back(value);
                    }
                }
            }
            break;

            case Measurand::CurrentOffered:
            {
                auto setpoints = m_setpoint_manager->getSetpoints();
                value.value    = std::to_string(static_cast<unsigned int>(setpoints[connector_id]));
                meter_value.sampledValue.push_back(value);
            }
            break;

            case Measurand::EnergyActiveImportRegister:
            {
                value.value = std::to_string(meter_simulator->getEnergy());
                value.phase = ocpp::types::Optional<Phase>();
                meter_value.sampledValue.push_back(value);
            }
            break;

            case Measurand::PowerActiveImport:
            {
                auto powers = meter_simulator->getInstantPowers();
                if (measurand.second.isSet())
                {
                    unsigned int phase = static_cast<unsigned int>(measurand.second.value());
                    if (phase <= meter_simulator->getNumberOfPhases())
                    {
                        value.value = std::to_string(powers[phase]);
                        value.phase = static_cast<Phase>(phase);
                        meter_value.sampledValue.push_back(value);
                    }
                    else
                    {
                        ret = false;
                    }
                }
                else
                {
                    for (size_t i = 0; i < powers.size(); i++)
                    {
                        value.value = std::to_string(powers[i]);
                        value.phase = static_cast<Phase>(i);
                        meter_value.sampledValue.push_back(value);
                    }
                }
            }
            break;

            case Measurand::Voltage:
            {
                auto voltages = meter_simulator->getVoltages();
                if (measurand.second.isSet())
                {
                    unsigned int phase = static_cast<unsigned int>(measurand.second.value());
                    if (phase <= meter_simulator->getNumberOfPhases())
                    {
                        value.value = std::to_string(voltages[phase]);
                        value.phase = static_cast<Phase>(phase);
                        meter_value.sampledValue.push_back(value);
                    }
                    else
                    {
                        ret = false;
                    }
                }
                else
                {
                    for (size_t i = 0; i < voltages.size(); i++)
                    {
                        value.value = std::to_string(voltages[i]);
                        value.phase = static_cast<Phase>(i);
                        meter_value.sampledValue.push_back(value);
                    }
                }
            }
            break;

            default:
            {
                ret = false;
            }
            break;
        }
    }

    return ret;
}
