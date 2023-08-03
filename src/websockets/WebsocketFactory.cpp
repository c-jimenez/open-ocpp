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

#include "WebsocketFactory.h"
#include "LibWebsocketClient.h"
#include "LibWebsocketClientPool.h"
#include "LibWebsocketServer.h"

#include <memory>
#include <mutex>
#include <vector>

namespace ocpp
{
namespace websockets
{

/** @brief Mutex to protect the access to the client pools */
static std::mutex s_client_pools_mutex;
/** @brief Client pools */
static std::vector<std::unique_ptr<LibWebsocketClientPool>> s_client_pools;
/** @brief Indicate if the standard clients must be allocated from the pools */
static bool s_force_clients_from_pool = false;

/** @brief Instanciate a client websocket */
IWebsocketClient* WebsocketFactory::newClient()
{
    if (s_force_clients_from_pool)
    {
        return newClientFromPool();
    }
    else
    {
        return new LibWebsocketClient();
    }
}

/** @brief Instanciate a client websocket from the pool (the pool must be started first) */
IWebsocketClient* WebsocketFactory::newClientFromPool()
{
    IWebsocketClient*           ret = nullptr;
    std::lock_guard<std::mutex> lock(s_client_pools_mutex);
    if (!s_client_pools.empty())
    {
        // Look for the pool with the less associated clients
        auto* selected_pool = &s_client_pools[0];
        for (auto& pool : s_client_pools)
        {
            if (pool->getClientsCount() < (*selected_pool)->getClientsCount())
            {
                selected_pool = &pool;
            }
        }
        ret = (*selected_pool)->newClient();
    }
    return ret;
}

/** @brief Instanciate a server websocket */
IWebsocketServer* WebsocketFactory::newServer()
{
    return new LibWebsocketServer();
}

/** @brief Set the number of client pools (can only be done once) */
bool WebsocketFactory::setClientPoolCount(size_t count)
{
    bool ret = false;

    std::lock_guard<std::mutex> lock(s_client_pools_mutex);
    if (s_client_pools.empty() && (count > 0))
    {
        for (size_t i = 0; i < count; i++)
        {
            s_client_pools.emplace_back(std::make_unique<LibWebsocketClientPool>());
        }
        ret = true;
    }

    return ret;
}

/** @brief Start the client pools */
bool WebsocketFactory::startClientPools()
{
    bool ret = true;

    std::lock_guard<std::mutex> lock(s_client_pools_mutex);
    if (!s_client_pools.empty())
    {
        for (auto& pool : s_client_pools)
        {
            ret = pool->start() && ret;
        }
    }

    return ret;
}

/** @brief Stop the client pools */
bool WebsocketFactory::stopClientPools()
{
    bool ret = true;

    std::lock_guard<std::mutex> lock(s_client_pools_mutex);
    if (!s_client_pools.empty())
    {
        for (auto& pool : s_client_pools)
        {
            ret = pool->stop() && ret;
        }
    }

    return ret;
}

/** @brief Indicate to use the client pools even for new clients instanciated with the newClient API */
void WebsocketFactory::forceClientPoolsUsage()
{
    s_force_clients_from_pool = true;
}

} // namespace websockets
} // namespace ocpp
