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

#ifndef OPENOCPP_LIBWEBSOCKETSERVER_H
#define OPENOCPP_LIBWEBSOCKETSERVER_H

#include "IWebsocketServer.h"
#include "Queue.h"
#include "Url.h"
#include "websockets.h"

#include <array>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace ocpp
{
namespace websockets
{

/** @brief Websocket server implementation using libwebsockets */
class LibWebsocketServer : public IWebsocketServer
{
  public:
    /** @brief Constructor */
    LibWebsocketServer();
    /** @brief Destructor */
    virtual ~LibWebsocketServer();

    /** @copydoc bool IWebsocketServer::start(const std::string&, const std::string&, const Credentials&,
     *                                        std::chrono::milliseconds) */
    bool start(const std::string&        url,
               const std::string&        protocol,
               const Credentials&        credentials,
               std::chrono::milliseconds ping_interval = std::chrono::seconds(5)) override;

    /** @copydoc bool IWebsocketServer::stop() */
    bool stop() override;

    /** @copydoc void IWebsocketServer::registerListener(IListener&) */
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

    /** @brief Websocket client connection */
    class Client : public IClient
    {
        friend class LibWebsocketServer;

      public:
        /**
         * @brief Constructor
         * @param wsi Client socket
         * @param ip_address IP address
        */
        Client(struct lws* wsi, const char* ip_address);
        /** @brief Destructor */
        virtual ~Client();

        /** @copydoc const std::string& IClient::ipAddress(bool) const */
        const std::string& ipAddress() const override;

        /** @copydoc bool IClient::disconnect(bool) */
        bool disconnect(bool notify_disconnected) override;

        /** @copydoc bool IClient::isConnected() */
        bool isConnected() override;

        /** @copydoc bool IClient::send(const void*, size_t) */
        bool send(const void* data, size_t size) override;

        /** @copydoc bool IClient::registerListener(IListener&) */
        void registerListener(IClient::IListener& listener) override;

        /**
         * @brief Get the size in bytes of the fragmented frame
         * @return Size in bytes of the fragmented frame
         */
        size_t getFragmentedFrameSize() const { return m_fragmented_frame_size; }

        /**
         * @brief Get the fragmented frame
         * @return Fragmented frame
         */
        const void* getFragmentedFrame() const { return m_fragmented_frame; }

        /**
         * @brief Prepare the buffer to store a new fragmented frame
         * @param frame_size Size of the fragmented frame in bytes
         */
        void beginFragmentedFrame(size_t frame_size);

        /**
         * @brief Append data to the fragmented frame
         * @param data Data to append
         * @param size Size of the data in bytes
         */
        void appendFragmentedData(const void* data, size_t size);

        /** @brief Release the memory associated with the fragmented frame */
        void releaseFragmentedFrame();

      private:
        /** @brief Client socket */
        struct lws* m_wsi;
        /** @brief IP address */
        const std::string m_ip_address;
        /** @brief Connection status */
        bool m_connected;
        /** @brief Listener */
        IClient::IListener* m_listener;
        /** @brief Queue of messages to send */
        ocpp::helpers::Queue<SendMsg*> m_send_msgs;
        /** @brief Buffer to store fragmented frames */
        uint8_t* m_fragmented_frame;
        /** @brief Size of the fragmented frame */
        size_t m_fragmented_frame_size;
        /** @brief Current index in the fragmented frame */
        size_t m_fragmented_frame_index;
    };

    /** @brief Listener */
    IListener* m_listener;
    /** @brief Internal thread */
    std::thread* m_thread;
    /** @brief Indicate the end of processing to the thread */
    bool m_end;
    /** @brief Connection URL */
    Url m_url;
    /** @brief Name of the protocol to use */
    std::string m_protocol;
    /** @brief Credentials */
    Credentials m_credentials;

    /** @brief Websocket context */
    struct lws_context* m_context;
    /** @brief Websocket log context */
    lws_log_cx_t m_logs_context;
    /** @brief Related wsi */
    struct lws* m_wsi;
    /** @brief Retry policy */
    lws_retry_bo_t m_retry_policy;
    /** @brief Protocols */
    std::array<struct lws_protocols, 2u> m_protocols;
    /** @brief IP address of the currently connecting client */
    char* m_connecting_ip_address;

    /** @brief Connected clients */
    std::unordered_map<struct lws*, std::shared_ptr<IClient>> m_clients;

    /** @brief Internal thread */
    void process();

    /** @brief libwebsockets event callback */
    static int eventCallback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len);
};

} // namespace websockets
} // namespace ocpp

#endif // OPENOCPP_LIBWEBSOCKETSERVER_H
