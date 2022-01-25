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

#ifndef CHARGEPOINTEVENTSHANDLER_H
#define CHARGEPOINTEVENTSHANDLER_H

#include "DefaultChargePointEventsHandler.h"

#include <memory>

class IMeter;
class ISetpointManager;

/** @brief Charge point event handlers implementation */
class ChargePointEventsHandler : public DefaultChargePointEventsHandler
{
  public:
    /** @brief Constructor */
    ChargePointEventsHandler(ChargePointDemoConfig& config);

    /** @brief Destructor */
    virtual ~ChargePointEventsHandler();

    // IChargePointEventsHandler interface

    /** @copydoc unsigned int IChargePointEventsHandler::getTxStartStopMeterValue(unsigned int) */
    unsigned int getTxStartStopMeterValue(unsigned int connector_id) override;

    /** @copydoc bool IChargePointEventsHandler::getMeterValue(unsigned int, const std::pair<ocpp::types::Measurand, ocpp::types::Optional<ocpp::types::Phase>>&, ocpp::types::MeterValue&) */
    bool getMeterValue(unsigned int                                                                        connector_id,
                       const std::pair<ocpp::types::Measurand, ocpp::types::Optional<ocpp::types::Phase>>& measurand,
                       ocpp::types::MeterValue&                                                            meter_value) override;

    // API

    /** @brief Set the meter simulators */
    void setMeterSimulators(std::vector<std::unique_ptr<IMeter>>& meter_simulators) { m_meter_simulators = &meter_simulators; }

    /** @brief Set the setpoint manager */
    void setSetpointManager(ISetpointManager& setpoint_manager) { m_setpoint_manager = &setpoint_manager; }

  private:
    /** @brief Meter simulators */
    std::vector<std::unique_ptr<IMeter>>* m_meter_simulators;
    /** @brief Setpoint manager */
    ISetpointManager* m_setpoint_manager;
};

#endif // CHARGEPOINTEVENTSHANDLER_H
