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

#ifndef OPENOCPP_LIBWEBSOCKETCLIENT_H
#define OPENOCPP_LIBWEBSOCKETCLIENT_H

#include "IWebsocketClient.h"
#include "Queue.h"
#include "Url.h"
#include "websockets.h"

#include <condition_variable>
#include <mutex>
#include <thread>

namespace ocpp
{
namespace websockets
{

/** @brief Websocket client implementation using libwebsockets */
class LibWebsocketClient : public IWebsocketClient
{
  public:
    /** @brief Constructor */
    LibWebsocketClient();
    /** @brief Destructor */
    virtual ~LibWebsocketClient();

    /** @copydoc bool IWebsocketClient::connect(const std::string&, const std::string&, const Credentials&,
     *                                          std::chrono::milliseconds, std::chrono::milliseconds, std::chrono::milliseconds) */
    bool connect(const std::string&        url,
                 const std::string&        protocol,
                 const Credentials&        credentials,
                 std::chrono::milliseconds connect_timeout = std::chrono::seconds(5),
                 std::chrono::milliseconds retry_interval  = std::chrono::seconds(5),
                 std::chrono::milliseconds ping_interval   = std::chrono::seconds(5)) override;

    /** @copydoc bool IWebsocketClient::disconnect() */
    bool disconnect() override;

    /** @copydoc bool IWebsocketClient::isConnected() */
    bool isConnected() override;

    /** @copydoc bool IWebsocketClient::send(const void*, size_t) */
    bool send(const void* data, size_t size) override;

    /** @copydoc void IWebsocketClient::registerListener(IListener&) */
    void registerListener(IListener& listener) override;

  private:
    /** @brief Message to send */
    struct SendMsg
    {
        /** @brief Constructor */
        SendMsg(const void* _data, size_t _size)
        {
            data    = new unsigned char[LWS_PRE + _size];
            size    = _size;
            payload = &data[LWS_PRE];
            memcpy(payload, _data, size);
        }
        /** @brief Destructor */
        virtual ~SendMsg() { delete[] data; }

        /** @brief Data buffer */
        unsigned char* data;
        /** @brief Payload start */
        unsigned char* payload;
        /** @brief Size in bytes */
        size_t size;
    };

    /** @brief Listener */
    IListener* m_listener;
    /** @brief Internal thread */
    std::thread* m_thread;
    /** @brief Indicate the end of processing to the thread */
    bool m_end;
    /** @brief Retry interval in ms */
    uint32_t m_retry_interval;
    /** @brief PING interval in s */
    uint16_t m_ping_interval;
    /** @brief Indicate if the connection error has been notified at least once */
    bool m_connection_error_notified;
    /** @brief Connection URL */
    Url m_url;
    /** @brief Name of the protocol to use */
    std::string m_protocol;
    /** @brief Credentials */
    Credentials m_credentials;
    /** @brief Indicate the connection state */
    bool m_connected;

    /** @brief Websocket context */
    struct lws_context* m_context;
    /** @brief Websocket log context */
    lws_log_cx_t m_logs_context;
    /** @brief Schedule list */
    lws_sorted_usec_list_t m_sched_list;
    /** @brief Related wsi */
    struct lws* m_wsi;
    /** @brief Retry policy */
    lws_retry_bo_t m_retry_policy;
    /** @brief Consecutive retries */
    uint16_t m_retry_count;

    /** @brief Queue of messages to send */
    ocpp::helpers::Queue<SendMsg*> m_send_msgs;

    /** @brief Buffer to store fragmented frames */
    uint8_t* m_fragmented_frame;
    /** @brief Size of the fragmented frame */
    size_t m_fragmented_frame_size;
    /** @brief Current index in the fragmented frame */
    size_t m_fragmented_frame_index;

    /** @brief Internal thread */
    void process();

    /** @brief Prepare the buffer to store a new fragmented frame */
    void beginFragmentedFrame(size_t frame_size);
    /** @brief Append data to the fragmented frame */
    void appendFragmentedData(const void* data, size_t size);
    /** @brief Release the memory associated with the fragmented frame */
    void releaseFragmentedFrame();

    /** @brief libwebsockets connection callback */
    static void connectCallback(struct lws_sorted_usec_list* sul) noexcept;
    /** @brief libwebsockets event callback */
    static int eventCallback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len) noexcept;
};

} // namespace websockets
} // namespace ocpp

#endif // OPENOCPP_LIBWEBSOCKETCLIENT_H
