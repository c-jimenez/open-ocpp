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

#include "WorkerThreadPool.h"

namespace ocpp
{
namespace helpers
{

/** @brief Constructor */
WorkerThreadPool::WorkerThreadPool(size_t thread_count) : m_stop(false), m_threads(), m_job_queue()
{
    // Create thread
    for (size_t i = 0; i < thread_count; i++)
    {
        m_threads.push_back(new std::thread(std::bind(&WorkerThreadPool::workerThread, this)));
    }
}

/** @brief Destructor */
WorkerThreadPool::~WorkerThreadPool()
{
    // Stop threads
    m_stop = true;
    m_job_queue.setEnable(false);

    // Wait end of thread an release resources
    for (std::thread* thread : m_threads)
    {
        thread->join();
        delete thread;
    }
}

/** @brief Worker thread */
void WorkerThreadPool::workerThread()
{
    std::shared_ptr<IJob> job;

    // Thread loop
    while (!m_stop)
    {
        // Wait for a job
        if (m_job_queue.pop(job))
        {
            // Execute job
            job->run();

            // Release resources
            job.reset();
        }
    }
}

} // namespace helpers
} // namespace ocpp
