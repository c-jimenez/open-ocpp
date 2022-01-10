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

#include "TimerPool.h"
#include "Timer.h"

namespace ocpp
{
namespace helpers
{

/** @brief Constructor */
TimerPool::TimerPool()
    : m_stop(false),
      m_update_wakeup_time(false),
      m_wakeup_mutex(),
      m_wakeup_cond(),
      m_wake_up_time_point(std::chrono::system_clock::now() + std::chrono::hours(2400u)),
      m_thread(std::bind(&TimerPool::threadLoop, this)),
      m_timers()
{
}

/** @brief Destructor */
TimerPool::~TimerPool()
{
    // Stop thread
    m_stop = true;
    m_wakeup_cond.notify_one();
    m_thread.join();
}

/** @brief Create a timer */
Timer* TimerPool::createTimer()
{
    return new Timer(*this);
}

/** @brief TimerPool thread loop */
void TimerPool::threadLoop()
{
    // Thread loop
    while (!m_stop)
    {
        // Wait next timer
        std::unique_lock<std::mutex> lock(m_wakeup_mutex);
        if (m_wakeup_cond.wait_until(lock, m_wake_up_time_point, [this] { return (m_stop || m_update_wakeup_time); }))
        {
            // Update wake up time to new timer wakeup timer
            if (m_update_wakeup_time)
            {
                // New wakeup time point
                computeNextWakeupTimepoint();

                m_update_wakeup_time = false;
            }
        }
        else
        {
            // Timer has elapsed
            Timer* timer = m_timers.front();
            if (timer->m_single_shot)
            {
                // Single shot : remove timer from the list
                m_timers.pop_front();

                // Timer is now stopped
                timer->m_started = false;
            }
            else
            {
                // Periodic : compute next wakeup time point
                timer->m_wake_up_time_point += timer->m_interval;
            }

            // New wakeup time point
            computeNextWakeupTimepoint();

            // Notify user
            timer->m_callback();
        }
    }
}

/** @brief Compute next wakeup time point */
void TimerPool::computeNextWakeupTimepoint()
{
    // New wakeup time point
    if (m_timers.empty())
    {
        // Next wakeup in 100days
        m_wake_up_time_point = std::chrono::system_clock::now() + std::chrono::hours(2400u);
    }
    else
    {
        // Re-order timer list
        m_timers.sort([](const Timer* a, const Timer* b) { return (a->m_wake_up_time_point < b->m_wake_up_time_point); });
        m_wake_up_time_point = m_timers.front()->m_wake_up_time_point;
    }
}

/** @brief Lock access to the timers */
void TimerPool::lock()
{
    if (std::this_thread::get_id() != m_thread.get_id())
    {
        m_wakeup_mutex.lock();
    }
}

/** @brief Unlock access to the timers */
void TimerPool::unlock()
{
    if (std::this_thread::get_id() != m_thread.get_id())
    {
        m_wakeup_mutex.unlock();
    }
}

/** @brief Add timer to the list of active timers */
void TimerPool::addTimer(Timer* timer)
{
    // Check if the timer shall wakeup before
    // next programmed wakeup timepoint
    if (timer->m_wake_up_time_point < m_wake_up_time_point)
    {
        // Trigger update of wakeup timepoint
        m_update_wakeup_time = true;
        m_wakeup_cond.notify_one();
    }

    // Add timer to the list
    m_timers.push_back(timer);

    // Timer is now started
    timer->m_started = true;
}

/** @brief Remove timer from the list of active timers */
void TimerPool::removeTimer(Timer* timer)
{
    // Check if the timer is the next timer to wakeup
    if (timer == m_timers.front())
    {
        // Trigger update of wakeup timepoint
        m_update_wakeup_time = true;
        m_wakeup_cond.notify_one();
    }

    // Remove timer from the list
    m_timers.remove(timer);

    // Timer is now stopped
    timer->m_started = false;
}

} // namespace helpers
} // namespace ocpp
