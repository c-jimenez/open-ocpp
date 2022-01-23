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

#ifndef ICENTRALSYSTEM_H
#define ICENTRALSYSTEM_H

#include "ICentralSystemConfig.h"
#include "ICentralSystemEventsHandler.h"

#include <memory>

namespace ocpp
{
namespace helpers
{
class TimerPool;
} // namespace helpers

namespace centralsystem
{

/** @brief Interface for central system implementations */
class ICentralSystem
{
  public:
    /**
     * @brief Instanciate a central system
     * @param stack_config Stack configuration
     * @param event_handler Stack event handler
     */
    static std::unique_ptr<ICentralSystem> create(const ocpp::config::ICentralSystemConfig& stack_config,
                                                  ICentralSystemEventsHandler&              events_handler);

    /** @brief Destructor */
    virtual ~ICentralSystem() { }

    /**
     * @brief Get the timer pool associated to the central system
     * @return Timer pool associated to the central system
     */
    virtual ocpp::helpers::TimerPool& getTimerPool() = 0;

    /**
     * @brief Reset the central system's internal data (can be done only when the central system is stopped)
     * @return true if the data has been reset, false otherwise
     */
    virtual bool resetData() = 0;

    /**
     * @brief Start the central system
     * @return true if the central system has been started, false otherwise
     */
    virtual bool start() = 0;

    /**
     * @brief Stop the central system
     * @return true if the central system has been stopped, false otherwise
     */
    virtual bool stop() = 0;
};

} // namespace centralsystem
} // namespace ocpp

#endif // ICENTRALSYSTEM_H
