/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License version 2.1
as published by the Free Software Foundation.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#include "WebsocketClientStub.h"

#include <cstring>

namespace ocpp
{
namespace websockets
{

/** @brief Constructor */
WebsocketClientStub::WebsocketClientStub()
    : m_connect_called(false),
      m_url(),
      m_protocol(),
      m_credentials(),
      m_connect_timeout(0),
      m_retry_interval(0),
      m_ping_interval(0),
      m_disconnect_called(false),
      m_is_connected(false),
      m_send_called(false),
      m_sent_data(nullptr),
      m_sent_size(0),
      m_listener(nullptr),
      m_next_call_will_fail(false)
{
    reset();
}

/** @brief Destructor */
WebsocketClientStub::~WebsocketClientStub()
{
    reset();
}

/// IWebsocketClient interface

/** @copydoc bool IWebsocketClient::connect(const std::string&, const std::string&, const Credentials&,
 *                                          std::chrono::milliseconds, std::chrono::milliseconds, std::chrono::milliseconds) */
bool WebsocketClientStub::connect(const std::string&        url,
                                  const std::string&        protocol,
                                  const Credentials&        credentials,
                                  std::chrono::milliseconds connect_timeout,
                                  std::chrono::milliseconds retry_interval,
                                  std::chrono::milliseconds ping_interval)
{
    m_connect_called  = true;
    m_url             = url;
    m_protocol        = protocol;
    m_credentials     = credentials;
    m_connect_timeout = static_cast<unsigned int>(connect_timeout.count());
    m_retry_interval  = static_cast<unsigned int>(retry_interval.count());
    m_ping_interval   = static_cast<unsigned int>(ping_interval.count());

    return returnValue();
}

/** @copydoc bool IWebsocketClient::disconnect() */
bool WebsocketClientStub::disconnect()
{
    m_disconnect_called = true;
    return returnValue();
}

/** @copydoc bool IWebsocketClient::isConnected() */
bool WebsocketClientStub::isConnected()
{
    return m_is_connected;
}

/** @copydoc bool IWebsocketClient::send(const void*, size_t) */
bool WebsocketClientStub::send(const void* data, size_t size)
{
    m_send_called = true;
    if (m_sent_data)
    {
        delete[] m_sent_data;
        m_sent_data = nullptr;
    }
    if (data)
    {
        m_sent_data = new uint8_t[size + 1];
        memcpy(m_sent_data, data, size);
        m_sent_data[size] = 0;
    }
    m_sent_size = size;

    return returnValue();
}

/** @copydoc void IWebsocketClient::registerListener(IListener&) */
void WebsocketClientStub::registerListener(IListener& listener)
{
    m_listener = &listener;
}

/** @brief Reset stub's data */
void WebsocketClientStub::reset()
{
    m_connect_called    = false;
    m_url               = "";
    m_protocol          = "";
    m_connect_timeout   = 0;
    m_retry_interval    = 0;
    m_disconnect_called = false;
    m_is_connected      = false;
    m_send_called       = false;
    if (m_sent_data)
    {
        delete[] m_sent_data;
        m_sent_data = nullptr;
    }
    m_sent_size           = 0;
    m_listener            = nullptr;
    m_next_call_will_fail = false;
}

/** @brief Specify that the newxt call must fail */
void WebsocketClientStub::nextCallWillFail()
{
    m_next_call_will_fail = true;
}

/** @brief Indicate that the websocket is connected */
void WebsocketClientStub::setConnected()
{
    m_is_connected = true;
}

/** @brief Notify a connected event to the listener */
void WebsocketClientStub::notifyConnected()
{
    m_listener->wsClientConnected();
}

/** @brief Notify a failed event to the listener */
void WebsocketClientStub::notifyFailed()
{
    m_listener->wsClientFailed();
}

/** @brief Notify a disconnected event to the listener */
void WebsocketClientStub::notifyDisconnected()
{
    m_listener->wsClientDisconnected();
}

/** @brief Notify an error event to the listener */
void WebsocketClientStub::notifyError()
{
    m_listener->wsClientError();
}

/** @brief Notify a data received event to the listener */
void WebsocketClientStub::notifyDataReceived(const void* data, size_t size)
{
    m_listener->wsClientDataReceived(data, size);
}

/** @brief Compute the return value of a call */
bool WebsocketClientStub::returnValue()
{
    const bool ret        = !m_next_call_will_fail;
    m_next_call_will_fail = false;
    return ret;
}

} // namespace websockets
} // namespace ocpp
