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

#ifndef OPENOCPP_ITIMERPOOL_H
#define OPENOCPP_ITIMERPOOL_H

#include <string>

namespace ocpp
{
namespace helpers
{

class Timer;

/** @brief Interface for timer pools implementations */
class ITimerPool
{
    friend class Timer;

  public:
    /** @brief Destructor */
    virtual ~ITimerPool() { }

    /**
     * @brief Create a timer
     * @param name Name of the timer
     * @return Created timer
     */
    virtual Timer* createTimer(const char* name = "") = 0;

    /**
     * @brief Get a timer from its name
     * @param timer_name Name of the timer
     * @return Request timer is found, nullptr otherwise
     */
    virtual Timer* getTimer(const std::string& timer_name) = 0;

  protected:
    /**
     * @brief Register a timer in the timer pool
     * @param timer Timer to register
     */
    virtual void registerTimer(Timer* timer) = 0;
    /** @brief Lock access to the timers */
    virtual void lock() = 0;
    /** @brief Unlock access to the timers */
    virtual void unlock() = 0;
    /**
     * @brief Add timer to the list of active timers
     * @param timer Timer to add
     */
    virtual void addTimer(Timer* timer) = 0;
    /**
     * @brief Remove timer from the list of active timers
     * @param timer Timer to remove
     */
    virtual void removeTimer(Timer* timer) = 0;
};

} // namespace helpers
} // namespace ocpp

#endif // OPENOCPP_ITIMERPOOL_H
