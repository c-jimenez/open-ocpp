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

#include "SetpointManager.h"

using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

/** @brief Constructor */
SetpointManager::SetpointManager(ocpp::chargepoint::IChargePoint& charge_point,
                                 unsigned int                     connector_count,
                                 unsigned int                     max_charge_point_current,
                                 unsigned int                     max_connector_current)
    : m_charge_point(charge_point),
      m_update_timer(charge_point.getTimerPool(), "Setpoint"),
      m_connector_count(connector_count),
      m_max_charge_point_current(max_charge_point_current),
      m_max_connector_current(max_connector_current),
      m_mutex(),
      m_setpoints(connector_count + 1u)
{
    // Start update timer
    m_update_timer.setCallback(std::bind(&SetpointManager::update, this));
    m_update_timer.start(UPDATE_PERIOD);
}

/** @brief Destructor */
SetpointManager::~SetpointManager() { }

/** @brief Get the setpoints of each connectors */
std::vector<float> SetpointManager::getSetpoints()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_setpoints;
}
/** @brief Get the setpoints of a connectors */
float SetpointManager::getSetpoint(unsigned int connector_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_setpoints[connector_id];
}

/** @brief Periodically update the setpoints */
void SetpointManager::update()
{
    Optional<SmartChargingSetpoint> charge_point_setpoint;
    Optional<SmartChargingSetpoint> connector_setpoint;

    std::lock_guard<std::mutex> lock(m_mutex);

    // Default setpoint is max current
    m_setpoints[0] = static_cast<float>(m_max_charge_point_current);

    // Get the smart charging setpoint for each connectors
    unsigned int charging_connectors = 0;
    for (unsigned int id = 1u; id <= m_connector_count; id++)
    {
        // Check if a charge is in progress on the connector to determine
        // default setpoint
        ChargePointStatus status = m_charge_point.getConnectorStatus(id);
        if ((status == ChargePointStatus::Charging) || (status == ChargePointStatus::SuspendedEVSE) ||
            (status == ChargePointStatus::SuspendedEV))
        {
            // Default setpoint is max current
            m_setpoints[id] = static_cast<float>(m_max_connector_current);
        }
        else
        {
            // Setpoint can be set to 0 when no charge is in progress
            m_setpoints[id] = 0.;
        }

        // Get the smart charging setpoint
        if (m_charge_point.getSetpoint(id, charge_point_setpoint, connector_setpoint))
        {
            // Apply setpoints
            if (charge_point_setpoint.isSet())
            {
                if (charge_point_setpoint.value().value < static_cast<float>(m_max_charge_point_current))
                {
                    m_setpoints[0] = charge_point_setpoint.value().value;
                }
                else
                {
                    m_setpoints[0] = static_cast<float>(m_max_charge_point_current);
                }
            }
            if (connector_setpoint.isSet())
            {
                if (connector_setpoint.value().value < static_cast<float>(m_max_connector_current))
                {
                    m_setpoints[id] = connector_setpoint.value().value;
                }
                else
                {
                    m_setpoints[id] = static_cast<float>(m_max_connector_current);
                }
            }
        }

        // Count charging connectors
        if (m_setpoints[id] > 0.)
        {
            charging_connectors++;
        }
    }

    // Check that the sum of all connectors setpoints doesn't exceed
    // the charge point setpoint
    float total_connectors = 0.f;
    for (unsigned int id = 1u; id <= m_connector_count; id++)
    {
        total_connectors += m_setpoints[id];
    }
    if (total_connectors > m_setpoints[0])
    {
        // Remove the same amount of current on each charging connector to not exceed
        // the charge point capacity
        float per_connector_exceed_current = (total_connectors - m_setpoints[0]) / static_cast<float>(charging_connectors);
        for (unsigned int id = 1u; id <= m_connector_count; id++)
        {
            if (m_setpoints[id] > 0.)
            {
                m_setpoints[id] -= per_connector_exceed_current;
            }
        }
    }
}
