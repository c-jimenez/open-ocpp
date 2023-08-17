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
      m_wake_up_time_point(std::chrono::steady_clock::now() + std::chrono::hours(2400u)),
      m_thread(std::bind(&TimerPool::threadLoop, this)),
      m_timers(),
      m_active_timers()
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

/** @copydoc Timer* ITimerPool::createTimer(const char*) */
Timer* TimerPool::createTimer(const char* name)
{
    return new Timer(*this, name);
}

/** @copydoc Timer* ITimerPool::getTimer(const std::string&) */
Timer* TimerPool::getTimer(const std::string& timer_name)
{
    Timer* timer = nullptr;

    std::lock_guard<std::mutex> lock(m_wakeup_mutex);
    for (Timer* t : m_timers)
    {
        if (t->m_name == timer_name)
        {
            timer = t;
            break;
        }
    }
    return timer;
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
            Timer* timer = m_active_timers.front();
            if (timer->m_single_shot)
            {
                // Single shot : remove timer from the list
                m_active_timers.pop_front();

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
    if (m_active_timers.empty())
    {
        // Next wakeup in 100days
        m_wake_up_time_point = std::chrono::steady_clock::now() + std::chrono::hours(2400u);
    }
    else
    {
        // Re-order timer list
        m_active_timers.sort([](const Timer* a, const Timer* b) { return (a->m_wake_up_time_point < b->m_wake_up_time_point); });
        m_wake_up_time_point = m_active_timers.front()->m_wake_up_time_point;
    }
}

/** @copydoc void ITimerPool::addTimer(Timer*) */
void TimerPool::registerTimer(Timer* timer)
{
    std::lock_guard<std::mutex> lock(m_wakeup_mutex);
    m_timers.push_back(timer);
}

/** @copydoc void ITimerPool::lock() */
void TimerPool::lock()
{
    if (std::this_thread::get_id() != m_thread.get_id())
    {
        m_wakeup_mutex.lock();
    }
}

/** @copydoc void ITimerPool::unlock() */
void TimerPool::unlock()
{
    if (std::this_thread::get_id() != m_thread.get_id())
    {
        m_wakeup_mutex.unlock();
    }
}

/** @copydoc void ITimerPool::addTimer(Timer*) */
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
    m_active_timers.push_back(timer);
}

/** @copydoc void ITimerPool::removeTimer(Timer*) */
void TimerPool::removeTimer(Timer* timer)
{
    // Check if the timer is the next timer to wakeup
    if (timer == m_active_timers.front())
    {
        // Trigger update of wakeup timepoint
        m_update_wakeup_time = true;
        m_wakeup_cond.notify_one();
    }

    // Remove timer from the list
    m_active_timers.remove(timer);
}

} // namespace helpers
} // namespace ocpp
