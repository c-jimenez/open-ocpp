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

#ifndef TESTABLEWORKERTHREADPOOL_H
#define TESTABLEWORKERTHREADPOOL_H

#include "WorkerThreadPool.h"

namespace ocpp
{
namespace helpers
{

/** @brief Testable pool of worker threads which can be used for unit tests */
class TestableWorkerThreadPool : public WorkerThreadPool
{
  public:
    /** @brief Constructor */
    TestableWorkerThreadPool() : WorkerThreadPool(0u) { }

  protected:
    /** @brief Add a job to the queue */
    virtual void queue(const std::shared_ptr<IJob>& job) { job->run(); }
};

} // namespace helpers
} // namespace ocpp

#endif // TESTABLEWORKERTHREADPOOL_H
