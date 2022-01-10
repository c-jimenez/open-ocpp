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

#ifndef TIMERPOOL_H
#define TIMERPOOL_H

#include <chrono>
#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <thread>

namespace ocpp
{
namespace helpers
{

class Timer;

/** @brief Handle a pool of timers */
class TimerPool
{
    friend class Timer;

  public:
    /** @brief Constructor */
    TimerPool();
    /** @brief Destructor */
    virtual ~TimerPool();

    /** @brief Create a timer */
    Timer* createTimer();

  private:
    /** @brief Indicate that the timers must stop */
    bool m_stop;
    /** @brief Indicate that the next wakeup time has changed */
    bool m_update_wakeup_time;
    /** @brief Mutex for wakeup condition */
    std::mutex m_wakeup_mutex;
    /** @brief Wakeup condition */
    std::condition_variable m_wakeup_cond;
    /** @brief Next wakeup time point */
    std::chrono::time_point<std::chrono::system_clock> m_wake_up_time_point;
    /** @brief Timers thread */
    std::thread m_thread;
    /** @brief List of active timers */
    std::list<Timer*> m_timers;

    /** @brief Timers thread loop */
    void threadLoop();
    /** @brief Compute next wakeup time point */
    void computeNextWakeupTimepoint();
    /** @brief Lock access to the timers */
    void lock();
    /** @brief Unlock access to the timers */
    void unlock();
    /** @brief Add timer to the list of active timers */
    void addTimer(Timer* timer);
    /** @brief Remove timer from the list of active timers */
    void removeTimer(Timer* timer);
};

} // namespace helpers
} // namespace ocpp

#endif // TIMERPOOL_H
