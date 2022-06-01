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

#ifndef TESTABLETIMERPOOL_H
#define TESTABLETIMERPOOL_H

#include "ITimerPool.h"

#include <list>

namespace ocpp
{
namespace helpers
{

/** @brief Testable pool of timers which can be used for unit tests */
class TestableTimerPool : public ITimerPool
{
  public:
    /** @brief Constructor */
    TestableTimerPool();
    /** @brief Destructor */
    virtual ~TestableTimerPool();

    /** @copydoc Timer* ITimerPool::createTimer(const char*) */
    Timer* createTimer(const char* name = "") override;
    /** @copydoc Timer* ITimerPool::getTimer(const std::string&) */
    Timer* getTimer(const std::string& timer_name) override;

  private:
    /** @brief List of registered timers */
    std::list<Timer*> m_timers;

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

#endif // TESTABLETIMERPOOL_H
