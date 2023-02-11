/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License version 2.1
as published by the Free Software Foundation.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#include "TestableTimerPool.h"
#include "Timer.h"

namespace ocpp
{
namespace helpers
{

/** @brief Constructor */
TestableTimerPool::TestableTimerPool() : m_timers() { }

/** @brief Destructor */
TestableTimerPool::~TestableTimerPool() { }

/** @copydoc Timer* ITimerPool::createTimer(const char*) */
Timer* TestableTimerPool::createTimer(const char* name)
{
    Timer* timer = new Timer(*this, name);
    m_timers.push_back(timer);
    return timer;
}

/** @copydoc Timer* ITimerPool::getTimer(const std::string&) */
Timer* TestableTimerPool::getTimer(const std::string& timer_name)
{
    Timer* ret = nullptr;
    for (Timer* timer : m_timers)
    {
        if (timer->getName() == timer_name)
        {
            ret = timer;
            break;
        }
    }
    return ret;
}

/** @copydoc void ITimerPool::addTimer(Timer*) */
void TestableTimerPool::registerTimer(Timer* timer)
{
    if (timer)
    {
        m_timers.push_back(timer);
    }
}

/** @copydoc void ITimerPool::lock() */
void TestableTimerPool::lock() { }

/** @copydoc void ITimerPool::unlock() */
void TestableTimerPool::unlock() { }

/** @copydoc void ITimerPool::addTimer(Timer*) */
void TestableTimerPool::addTimer(Timer* timer)
{
    (void)timer;
}

/** @copydoc void ITimerPool::removeTimer(Timer*) */
void TestableTimerPool::removeTimer(Timer* timer)
{
    (void)timer;
}

} // namespace helpers
} // namespace ocpp
