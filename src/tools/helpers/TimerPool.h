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

#ifndef OPENOCPP_TIMERPOOL_H
#define OPENOCPP_TIMERPOOL_H

#include "ITimerPool.h"

#include <atomic>
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
class TimerPool : public ITimerPool
{
    friend class Timer;

  public:
    /** @brief Constructor */
    TimerPool();
    /** @brief Destructor */
    virtual ~TimerPool();

    /** @copydoc Timer* ITimerPool::createTimer(const char*) */
    Timer* createTimer(const char* name = "") override;

    /** @copydoc Timer* ITimerPool::getTimer(const std::string&) */
    Timer* getTimer(const std::string& timer_name) override;

  private:
    /** @brief Indicate that the timers must stop */
    std::atomic<bool> m_stop;
    /** @brief Indicate that the next wakeup time has changed */
    std::atomic<bool> m_update_wakeup_time;
    /** @brief Mutex for wakeup condition */
    std::mutex m_wakeup_mutex;
    /** @brief Wakeup condition */
    std::condition_variable m_wakeup_cond;
    /** @brief Next wakeup time point */
    std::chrono::time_point<std::chrono::steady_clock> m_wake_up_time_point;
    /** @brief Timers thread */
    std::thread m_thread;
    /** @brief List of registered timers */
    std::list<Timer*> m_timers;
    /** @brief List of active timers */
    std::list<Timer*> m_active_timers;

    /** @brief Timers thread loop */
    void threadLoop();
    /** @brief Compute next wakeup time point */
    void computeNextWakeupTimepoint();

    /** @copydoc void ITimerPool::addTimer(Timer*) */
    void registerTimer(Timer* timer) override;
    /** @copydoc void ITimerPool::lock() */
    void lock() override;
    /** @copydoc void ITimerPool::unlock() */
    void unlock() override;
    /** @copydoc void ITimerPool::addTimer(Timer*) */
    void addTimer(Timer* timer) override;
    /** @copydoc void ITimerPool::removeTimer(Timer*) */
    void removeTimer(Timer* timer) override;
};

} // namespace helpers
} // namespace ocpp

#endif // OPENOCPP_TIMERPOOL_H
