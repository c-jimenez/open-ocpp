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

#ifndef METERSIMULATOR_H
#define METERSIMULATOR_H

#include "IMeter.h"
#include "Timer.h"

#include <mutex>

/** @brief Simulate a meter and its current/voltage consumption */
class MeterSimulator : public IMeter
{
  public:
    /** @brief Constructor */
    MeterSimulator(ocpp::helpers::ITimerPool& timer_pool, unsigned int phases_count);

    /** @brief Destructor */
    virtual ~MeterSimulator();

    /** @brief Start the meter */
    void start() override;

    /** @brief Stop the meter */
    void stop() override;

    /** @brief Set the voltages in V */
    void setVoltages(const std::vector<unsigned int> voltages) override;

    /** @brief Set the currents in A */
    void setCurrents(const std::vector<unsigned int> currents) override;

    /** @brief Get the number of phases */
    unsigned int getNumberOfPhases() override { return m_phases_count; }

    /** @brief Get the voltages in V */
    std::vector<unsigned int> getVoltages() override;

    /** @brief Get the currents in A */
    std::vector<unsigned int> getCurrents() override;

    /** @brief Get the instant powers in W */
    std::vector<unsigned int> getInstantPowers() override;

    /** @brief Get the total energy in Wh */
    int64_t getEnergy() override;

  private:
    /** @brief Timer to update meter values */
    ocpp::helpers::Timer m_update_timer;
    /** @brief Number of phases */
    const unsigned int m_phases_count;

    /** @brief Voltages in V */
    std::vector<unsigned int> m_voltages;
    /** @brief Currents in A */
    std::vector<unsigned int> m_currents;
    /** @brief Instant powers in W */
    std::vector<unsigned int> m_powers;
    /** @brief Total energy in mWh */
    int64_t m_energy;

    /** @brief Lock to protect meter values */
    std::mutex m_mutex;

    /** @brief Update period */
    static constexpr std::chrono::milliseconds UPDATE_PERIOD = std::chrono::milliseconds(500);

    /** @brief Periodically update the meter values */
    void update();
};

#endif // METERSIMULATOR_H
