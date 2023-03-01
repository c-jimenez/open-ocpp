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

#ifndef OPENOCPP_IREQUESTFIFO_H
#define OPENOCPP_IREQUESTFIFO_H

#include "json.h"

#include <string>

namespace ocpp
{
namespace messages
{

/** @brief Interface for request FIFO implementations */
class IRequestFifo
{
  public:
    class IListener;

    /** @brief Destructor */
    virtual ~IRequestFifo() { }

    /**
     * @brief Queue a request inside the FIFO
     * @param connector_id Id of the connector related to the request
     * @param action RPC action for the request
     * @param payload JSON payload of the request
     */
    virtual void push(unsigned int connector_id, const std::string& action, const rapidjson::Document& payload) = 0;

    /**
     * @brief Get the first request from the FIFO
     * @param connector_id Id of the connector related to the request
     * @param action RPC action for the request
     * @param payload JSON payload of the request
     * @return true if a request has been retrived, false if the FIFO is empty
     */
    virtual bool front(unsigned int& connector_id, std::string& action, rapidjson::Document& payload) = 0;

    /** @brief Delete the first request from the FIFO */
    virtual void pop() = 0;

    /**
     * @brief Get the number of requests inside the FIFO
     * @return Number of requests inside the FIFO
     */
    virtual size_t size() const = 0;

    /**
     * @brief Indicate if the FIFO is empty
     * @return true if the FIFO is empty, false otherwise
     */
    virtual bool empty() const = 0;

    /**
     * @brief Register a listener to FIFO events
     * @param listener Listener to register
     */
    virtual void registerListener(IListener* listener) = 0;

    /** @brief Interface to listener to FIFO events implementations */
    class IListener
    {
      public:
        /** @brief Destructor */
        virtual ~IListener() { }

        /** @brief Called when a request has been queued */
        virtual void requestQueued() = 0;
    };
};

} // namespace messages
} // namespace ocpp

#endif // OPENOCPP_IREQUESTFIFO_H
