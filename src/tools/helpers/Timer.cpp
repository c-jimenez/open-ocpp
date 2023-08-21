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

#include "Timer.h"
#include "ITimerPool.h"

namespace ocpp
{
namespace helpers
{

/** @brief Constructor */
Timer::Timer(ITimerPool& pool, const char* name)
    : m_pool(pool),
      m_name(name),
      m_single_shot(false),
      m_interval(std::chrono::milliseconds(0)),
      m_wake_up_time_point(std::chrono::time_point<std::chrono::steady_clock>::min()),
      m_started(false),
      m_callback()
{
    m_pool.registerTimer(this);
}

/** @brief Destructor */
Timer::~Timer()
{
    stop();
}

/** @brief Start the timer with the specified interval */
bool Timer::start(std::chrono::milliseconds interval, bool single_shot)
{
    bool ret = false;

    // Lock timers
    m_pool.lock();

    // Check if the timer is already started
    if (!m_started)
    {
        // Configure timer
        m_interval           = interval;
        m_single_shot        = single_shot;
        m_wake_up_time_point = std::chrono::steady_clock::now() + m_interval;

        // Add timer to the list
        m_pool.addTimer(this);

        // Timer is now started
        m_started = true;

        ret = true;
    }

    // Unlock timers
    m_pool.unlock();

    return ret;
}

/** @brief Restart the timer with the specified interval */
bool Timer::restart(std::chrono::milliseconds interval, bool single_shot)
{
    bool ret = false;

    // Lock timers
    m_pool.lock();

    // Check if the timer is already started
    if (m_started)
    {
        // Remove timer from the list
        m_pool.removeTimer(this);
    }

    // Configure timer
    m_interval           = interval;
    m_single_shot        = single_shot;
    m_wake_up_time_point = std::chrono::steady_clock::now() + m_interval;

    // Add timer to the list
    m_pool.addTimer(this);

    // Timer is now started
    m_started = true;

    ret = true;

    // Unlock timers
    m_pool.unlock();

    return ret;
}

/** @brief Stop the timer */
bool Timer::stop()
{
    bool ret = false;

    // Lock timers
    m_pool.lock();

    // Check if the timer is started
    if (m_started)
    {
        // Remove timer from the list
        m_pool.removeTimer(this);

        // Timer is now stopped
        m_started = false;

        ret = true;
    }

    // Unlock timers
    m_pool.unlock();

    return ret;
}

/** @brief Set the timer's callback */
void Timer::setCallback(std::function<void()> callback)
{
    // Lock timers
    m_pool.lock();

    // Save callback
    m_callback = callback;

    // Unlock timers
    m_pool.unlock();
}

} // namespace helpers
} // namespace ocpp
