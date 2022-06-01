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

#include "RequestFifoStub.h"

namespace ocpp
{
namespace messages
{

/** @brief Constructor */
RequestFifoStub::RequestFifoStub() : m_fifo(), m_listener(nullptr) { }

/** @brief Destructor */
RequestFifoStub::~RequestFifoStub() { }

/** @copydoc void IRequestFifo::push(unsigned int, const std::string&, const rapidjson::Document&) const */
void RequestFifoStub::push(unsigned int connector_id, const std::string& action, const rapidjson::Document& payload)
{
    m_fifo.emplace(connector_id, action, payload);
}

/** @copydoc bool IRequestFifo::front(unsigned int&, std::string&, const rapidjson::Document&) const */
bool RequestFifoStub::front(unsigned int& connector_id, std::string& action, rapidjson::Document& payload)
{
    bool ret = false;

    if (!m_fifo.empty())
    {
        const Entry& entry = m_fifo.front();
        connector_id       = entry.connector_id;
        action             = entry.action;
        payload.CopyFrom(entry.request, payload.GetAllocator());

        ret = true;
    }

    return ret;
}

/** @@copydoc void IRequestFifo::pop() */
void RequestFifoStub::pop()
{
    m_fifo.pop();
}

/** @brief Clear the fifo */
void RequestFifoStub::clear()
{
    while (!m_fifo.empty())
    {
        m_fifo.pop();
    }
}

} // namespace messages
} // namespace ocpp
