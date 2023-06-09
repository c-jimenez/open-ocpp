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

#include "RpcPool.h"

namespace ocpp
{
namespace rpc
{

/** @brief Constructor */
RpcPool::RpcPool() : m_requests_queue(), m_pool(), m_waiters() { }

/** @brief Destructor */
RpcPool::~RpcPool()
{
    stop();
}

/** @brief Start the pool */
bool RpcPool::start(unsigned int thread_count)
{
    bool ret = false;

    // Check if the pool is already started
    if (!m_pool)
    {
        // Check params
        if (thread_count > 0)
        {
            // Instanciate the thread pool
            m_pool = std::make_unique<ocpp::helpers::WorkerThreadPool>(thread_count);

            // Enable message queue
            m_requests_queue.setEnable(true);

            // Start threads
            for (unsigned int i = 0; i < thread_count; i++)
            {
                auto waiter = m_pool->run<void>(
                    [this]
                    {
                        // Wait for a message
                        std::shared_ptr<RpcBase::RpcMessage> rpc_message;
                        while (m_requests_queue.pop(rpc_message))
                        {
                            // Get owner
                            auto owner = rpc_message->owner.lock();
                            if (owner)
                            {
                                // Lock owner
                                std::lock_guard<std::mutex> lock(owner->lock);
                                if (owner->is_operational)
                                {
                                    // Process request
                                    owner->rpc_base.processIncomingRequest(rpc_message);
                                }
                            }
                        }
                    });
                m_waiters.push_back(std::move(waiter));
            }

            ret = true;
        }
    }

    return ret;
}

/** @brief Stop the pool */
bool RpcPool::stop()
{
    bool ret = false;

    // Check if the pool is already started
    if (m_pool)
    {
        // Disable message queue
        m_requests_queue.setEnable(false);

        // Wait for threads termination
        for (auto& waiter : m_waiters)
        {
            waiter.wait();
        }

        // Release pool
        m_pool.reset();

        // Flush queue
        m_requests_queue.clear();

        ret = true;
    }

    return ret;
}

} // namespace rpc
} // namespace ocpp
