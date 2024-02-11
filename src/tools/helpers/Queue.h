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

#ifndef OPENOCPP_QUEUE_H
#define OPENOCPP_QUEUE_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <limits>
#include <mutex>
#include <queue>

namespace ocpp
{
namespace helpers
{

/** @brief Message queue for inter-thread communication */
template <typename ItemType, size_t MAX_SIZE = std::numeric_limits<size_t>::max()>
class Queue
{
  public:
    /** @brief Constructor */
    Queue() : m_mutex(), m_cond_var(), m_queue(), m_enabled(true) { }
    /** @brief Destructor */
    virtual ~Queue() { }

    /**
     * @brief Get the size of the queue
     * @return Size of the queue in number of items
     */
    size_t size() const { return MAX_SIZE; }

    /**
     * @brief Indicate if the queue is empty
     * @return true if the queue is empty, false otherwise
     */
    bool empty() const
    {
        // Lock queue
        std::unique_lock<std::mutex> lock(m_mutex);

        // Check count
        bool ret = m_queue.empty();
        return ret;
    }

    /**
     * @brief Indicate if the queue is full
     * @return true if the queue is full, false otherwise
     */
    bool full() const
    {
        // Lock queue
        std::unique_lock<std::mutex> lock(m_mutex);

        // Check count
        bool ret = (m_queue.size() == MAX_SIZE);
        return ret;
    }

    /**
     * @brief Get the number of items in the queue
     * @return Number of items in the queue
     */
    size_t count()
    {
        // Lock queue
        std::unique_lock<std::mutex> lock(m_mutex);

        // Get item count
        size_t ret = m_queue.size();
        return ret;
    }

    /**
     * @brief Adds an item to the queue
     * @param item Item to add
     * @return true if the item has been added, fale if the maximum capacity has been reached
     */
    bool push(const ItemType& item)
    {
        bool ret = false;

        // Lock queue
        std::unique_lock<std::mutex> lock(m_mutex);

        // Check size
        if (m_queue.size() < MAX_SIZE)
        {
            // Add item
            m_queue.push(item);

            // Wakeup waiting thread
            m_cond_var.notify_one();
            ret = true;
        }

        return ret;
    }

    /**
     * @brief Adds an item to the queue
     * @param item Item to add
     * @return true if the item has been added, fale if the maximum capacity has been reached
     */
    bool push(ItemType&& item)
    {
        bool ret = false;

        // Lock queue
        std::unique_lock<std::mutex> lock(m_mutex);

        // Check size
        if (m_queue.size() < MAX_SIZE)
        {
            // Add item
            m_queue.push(std::move(item));

            // Wakeup waiting thread
            m_cond_var.notify_one();
            ret = true;
        }

        return ret;
    }

    /**
     * @brief Get an item from the queue
     * @param item Item retrieved from the queue
     * @param ms_timeout Max wait time in milliseconds
     * @return true if the item has been retrieved, false if the timeout has been reached
     */
    bool pop(ItemType& item, unsigned int ms_timeout = std::numeric_limits<unsigned int>::max())
    {
        bool ret = false;

        // Lock queue
        std::unique_lock<std::mutex> lock(m_mutex);

        // Wait for an item
        if (m_cond_var.wait_for(lock, std::chrono::milliseconds(ms_timeout), [this] { return (!m_enabled || !m_queue.empty()); }))
        {
            // Retrieve item
            if (m_enabled)
            {
                item = std::move(m_queue.front());
                m_queue.pop();
                ret = true;
            }
        }

        return ret;
    }

    /** @brief Clear the contents of the queue */
    void clear()
    {
        // Lock queue
        std::unique_lock<std::mutex> lock(m_mutex);

        // Clear queue
        while (!m_queue.empty())
        {
            m_queue.pop();
        }
    }

    /**
     * @brief Update the state of the queue
     * @param enabled If true messages can be received,
     *                if false abort current waiting operation
     *                and disable further message reception
     */
    void setEnable(bool enabled)
    {
        // Lock queue
        std::unique_lock<std::mutex> lock(m_mutex);

        // Update state
        m_enabled = enabled;

        // Wakeup waiting threads
        m_cond_var.notify_all();
    }

  private:
    /** @brief Mutex for concurrent access */
    mutable std::mutex m_mutex;
    /** @brief Condition variable for synchronization */
    std::condition_variable m_cond_var;
    /** @brief Queue to store data */
    std::queue<ItemType> m_queue;
    /** @brief Indicate that the queue is enabled */
    std::atomic<bool> m_enabled;
};

} // namespace helpers
} // namespace ocpp

#endif // OPENOCPP_QUEUE_H
