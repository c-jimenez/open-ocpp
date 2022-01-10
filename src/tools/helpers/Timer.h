/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <functional>
#include <string>
namespace ocpp
{
namespace helpers
{

class TimerPool;

/** @brief Timer */
class Timer
{
    friend class TimerPool;

  public:
    /**
     * @brief Constructor
     * @param pool Pool which will handle the timer
     * @param name Name of the timer
     */
    Timer(TimerPool& pool, const char* name = "");

    /** @brief Copy constructor */
    Timer(const Timer& timer);

    /** @brief Destructor */
    virtual ~Timer();

    /**
     * @brief Start the timer with the specified interval
     * @param interval Timer interval in milliseconds
     * @param single_shot Indicate if its a single shot timer
     * @return true if the timer has been started, false otherwise
     */
    bool start(std::chrono::milliseconds interval, bool single_shot = false);

    /**
     * @brief Restart the timer with the specified interval
     * @param interval Timer interval in milliseconds
     * @param single_shot Indicate if its a single shot timer
     * @return true if the timer has been started, false otherwise
     */
    bool restart(std::chrono::milliseconds interval, bool single_shot = false);

    /**
     * @brief Stop the timer
     * @return true if the timer has been stopped, false otherwise
     */
    bool stop();

    /**
     * @brief Indicate if the timer is started
     * @return true if the timer is started, false otherwise
     */
    bool isStarted() const;

    /**
     * @brief Set the timer's callback
     * @param callback Function to call when the timer elapse
     */
    void setCallback(std::function<void()> callback);

    /**
     * @brief Get the timer's interval
     * @return Timer's interval
     */
    std::chrono::milliseconds getInterval() const;

  private:
    /** @brief Timer pool */
    TimerPool& m_pool;
    /** @brief Name */
    const std::string m_name;
    /** @brief Indicate if the timer is a single shot timer */
    bool m_single_shot;
    /** @brief Wake uo interval */
    std::chrono::milliseconds m_interval;
    /** @brief Next wakeup time point */
    std::chrono::time_point<std::chrono::system_clock> m_wake_up_time_point;
    /** @brief Indicate if the timer is started */
    bool m_started;
    /** @brief Callback */
    std::function<void()> m_callback;
};

} // namespace helpers
} // namespace ocpp

#endif // TIMER_H
