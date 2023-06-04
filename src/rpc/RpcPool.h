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

#ifndef OPENOCPP_RPCPOOL_H
#define OPENOCPP_RPCPOOL_H

#include "RpcBase.h"
#include "WorkerThreadPool.h"

#include <vector>

namespace ocpp
{
namespace rpc
{

/** @brief Pool of threads to handle RPC communication */
class RpcPool
{
  public:
    /** @brief Constructor */
    RpcPool();

    /** @brief Destructor */
    virtual ~RpcPool();

    /** 
     * @brief Start the pool 
     * @param thread_count Number of worker threads
     * @return true if the pool has been started, false otherwise
     */
    bool start(unsigned int thread_count);

    /** 
     * @brief Stop the pool
     * @return true if the pool has been stopped, false otherwise
     */
    bool stop();

    /** @brief Get the request queue for incoming calls */
    ocpp::helpers::Queue<std::shared_ptr<RpcBase::RpcMessage>>& getRequestQueue() { return m_requests_queue; }

  protected:
    /** @brief Queue for incomming call requests */
    ocpp::helpers::Queue<std::shared_ptr<RpcBase::RpcMessage>> m_requests_queue;
    /** @brief Thread pool to handle requests */
    std::unique_ptr<ocpp::helpers::WorkerThreadPool> m_pool;
    /** @brief Waiters to synchronize with the end of the processsing */
    std::vector<ocpp::helpers::Waiter<void>> m_waiters;
};

} // namespace rpc
} // namespace ocpp

#endif // OPENOCPP_RPCBASE_H
