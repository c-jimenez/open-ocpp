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

#ifndef REQUESTFIFOSTUB_H
#define REQUESTFIFOSTUB_H

#include "IRequestFifo.h"

#include <queue>

namespace ocpp
{
namespace messages
{

/** @brief Request FIFO stub for unit tests */
class RequestFifoStub : public IRequestFifo
{
  public:
    /** @brief Constructor */
    RequestFifoStub();
    /** @brief Destructor */
    virtual ~RequestFifoStub();

    /** @copydoc void IRequestFifo::push(unsigned int, const std::string&, const rapidjson::Document&) const */
    void push(unsigned int connector_id, const std::string& action, const rapidjson::Document& payload) override;

    /** @copydoc bool IRequestFifo::front(unsigned int&, std::string&, const rapidjson::Document&) const */
    bool front(unsigned int& connector_id, std::string& action, rapidjson::Document& payload) override;

    /** @@copydoc void IRequestFifo::pop() */
    void pop() override;

    /** @copydoc size_t IRequestFifo::size() const */
    size_t size() const override { return m_fifo.size(); }

    /** @copydoc bool IRequestFifo::empty() const */
    bool empty() const override { return (size() == 0); }

    /** @copydoc void IRequestFifo::registerListener(IListener*) const */
    void registerListener(IListener* listener) override { m_listener = listener; }

    // API

    /** @brief Clear the fifo */
    void clear();

    /** @brief Get the listener */
    IListener* getListener() { return m_listener; }

  private:
    /** @brief FIFO entry */
    struct Entry
    {
        /** @brief Default constructor */
        Entry() : connector_id(0), action(), request() { }
        /** @brief Constructor */
        Entry(unsigned int _connector_id, std::string _action, const rapidjson::Document& _request)
            : connector_id(_connector_id), action(_action), request()
        {
            request.CopyFrom(_request, request.GetAllocator());
        }

        /** @brief Id of the connector related to the request */
        unsigned int connector_id;
        /** @brief Action */
        std::string action;
        /** @brief Request */
        rapidjson::Document request;
    };

    /** @brief FIFO */
    std::queue<Entry> m_fifo;
    /** @brief Listener */
    IListener* m_listener;
};

} // namespace messages
} // namespace ocpp

#endif // REQUESTFIFOSTUB_H
