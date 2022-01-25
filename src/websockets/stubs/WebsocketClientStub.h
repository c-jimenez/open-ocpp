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

#ifndef WEBSOCKETCLIENTSTUB_H
#define WEBSOCKETCLIENTSTUB_H

#include "IWebsocketClient.h"

#include <cstdint>

namespace ocpp
{
namespace websockets
{

/** @brief Websocket client stub for unit tests */
class WebsocketClientStub : public IWebsocketClient
{
  public:
    /** @brief Constructor */
    WebsocketClientStub();

    /** @brief Destructor */
    virtual ~WebsocketClientStub();

    /// IWebsocketClient interface

    /** @copydoc bool IWebsocketClient::connect(const std::string&, const std::string&, const Credentials&,
     *                                          std::chrono::milliseconds, std::chrono::milliseconds, std::chrono::milliseconds) */
    bool connect(const std::string&        url,
                 const std::string&        protocol,
                 const Credentials&        credentials,
                 std::chrono::milliseconds connect_timeout,
                 std::chrono::milliseconds retry_interval,
                 std::chrono::milliseconds ping_interval) override;

    /** @copydoc bool IWebsocketClient::disconnect() */
    bool disconnect() override;

    /** @copydoc bool IWebsocketClient::isConnected() */
    bool isConnected() override;

    /** @copydoc bool IWebsocketClient::send(const void*, size_t) */
    bool send(const void* data, size_t size) override;

    /** @copydoc void IWebsocketClient::registerListener(IListener&) */
    void registerListener(IListener& listener) override;

    /// Stub interface

    /** @brief Reset stub's data */
    void reset();

    /** @brief Specify that the next call must fail */
    void nextCallWillFail();

    /** @brief Indicate that the websocket is connected */
    void setConnected();

    /** @brief Notify a connected event to the listener */
    void notifyConnected();

    /** @brief Notify a failed event to the listener */
    void notifyFailed();

    /** @brief Notify a disconnected event to the listener */
    void notifyDisconnected();

    /** @brief Notify an error event to the listener */
    void notifyError();

    /** @brief Notify a data received event to the listener */
    void notifyDataReceived(const void* data, size_t size);

    /// Getters

    bool               connectCalled() const { return m_connect_called; }
    std::string        url() const { return m_url; }
    std::string        protocol() const { return m_protocol; }
    const Credentials& credentials() const { return m_credentials; }
    unsigned int       connectTimeout() const { return m_connect_timeout; }
    unsigned int       retryInterval() const { return m_retry_interval; }
    unsigned int       pingInterval() const { return m_ping_interval; }
    bool               disconnectCalled() const { return m_disconnect_called; }
    bool               sendCalled() const { return m_send_called; }
    const uint8_t*     sentData() const { return m_sent_data; }
    size_t             sentSize() const { return m_sent_size; }

  private:
    /** @brief Indicate if the connect() function has been called */
    bool m_connect_called;
    /** @brief URL */
    std::string m_url;
    /** @brief Protocol */
    std::string m_protocol;
    /** @brief Credentials */
    Credentials m_credentials;
    /** @brief Connect timeout */
    unsigned int m_connect_timeout;
    /** @brief Retry interval */
    unsigned int m_retry_interval;
    /** @brief PING interval */
    unsigned int m_ping_interval;
    /** @brief Indicate if the disconnect() function has been called */
    bool m_disconnect_called;
    /** @brief Connection state */
    bool m_is_connected;
    /** @brief Indicate if the send() function has been called */
    bool m_send_called;
    /** @brief Sent data */
    uint8_t* m_sent_data;
    /** @brief Sent data size */
    size_t m_sent_size;
    /** @brief Listener */
    IListener* m_listener;
    /** @brief Indicate that the next call will fail */
    bool m_next_call_will_fail;

    /** @brief Compute the return value of a call */
    bool returnValue();
};

} // namespace websockets
} // namespace ocpp

#endif // WEBSOCKETCLIENTSTUB_H
