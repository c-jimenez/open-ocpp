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

#ifndef OPENOCPP_ILOCALCONTROLLER_H
#define OPENOCPP_ILOCALCONTROLLER_H

#include "AuthorizationData.h"
#include "Certificate.h"
#include "CertificateHashDataType.h"
#include "ChargingProfile.h"
#include "ILocalControllerConfig.h"
#include "KeyValue.h"
#include "SecurityEvent.h"

#include <memory>

namespace ocpp
{
namespace helpers
{
class ITimerPool;
class WorkerThreadPool;
} // namespace helpers
namespace database
{
class Database;
} // namespace database

namespace localcontroller
{

class ILocalControllerEventsHandler;

/** @brief Interface for local controller implementations */
class ILocalController
{
  public:
    /**
     * @brief Instanciate a local controller
     * @param stack_config Stack configuration
     * @param event_handler Stack event handler
     */
    static std::unique_ptr<ILocalController> create(const ocpp::config::ILocalControllerConfig& stack_config,
                                                    ILocalControllerEventsHandler&              events_handler);

    /**
     * @brief Instanciate a local controller with the provided timer and worker pools
     *        To use when you have to instanciate multiple Central System / Charge Point
     *        => Allow to reduce thread and memory usage
     * @param stack_config Stack configuration
     * @param event_handler Stack event handler
     * @param timer_pool Timer pool
     * @param worker_pool Worker thread pool
     */
    static std::unique_ptr<ILocalController> create(const ocpp::config::ILocalControllerConfig&      stack_config,
                                                    ILocalControllerEventsHandler&                   events_handler,
                                                    std::shared_ptr<ocpp::helpers::ITimerPool>       timer_pool,
                                                    std::shared_ptr<ocpp::helpers::WorkerThreadPool> worker_pool);

    /** @brief Destructor */
    virtual ~ILocalController() { }

    /**
     * @brief Get the configuration associated to the local controller
     * @return Configuration associated to the local controller
     */
    virtual const ocpp::config::ILocalControllerConfig& getConfig() = 0;

    /**
     * @brief Get the timer pool associated to the local controller
     * @return Timer pool associated to the local controller
     */
    virtual ocpp::helpers::ITimerPool& getTimerPool() = 0;

    /**
     * @brief Get the worker pool associated to the local controller
     * @return Worker pool associated to the local controller
     */
    virtual ocpp::helpers::WorkerThreadPool& getWorkerPool() = 0;

    /**
     * @brief Reset the local controller's internal data (can be done only when the local controller is stopped)
     * @return true if the data has been reset, false otherwise
     */
    virtual bool resetData() = 0;

    /**
     * @brief Start the local controller
     * @return true if the local controller has been started, false otherwise
     */
    virtual bool start() = 0;

    /**
     * @brief Stop the local controller
     * @return true if the local controller has been stopped, false otherwise
     */
    virtual bool stop() = 0;
};

} // namespace localcontroller
} // namespace ocpp

#endif // OPENOCPP_ILOCALCONTROLLER_H
