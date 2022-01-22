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

#include "LibWebsocketClient.h"

#include <cstdint>
#include <functional>
#include <iostream>

namespace ocpp
{
namespace websockets
{

/** @brief Thread local client instance used when callbacks doesn't provide user data */
thread_local LibWebsocketClient* client;

/** @brief Constructor */
LibWebsocketClient::LibWebsocketClient()
    : IWebsocketClient(),
      m_listener(nullptr),
      m_thread(nullptr),
      m_end(false),
      m_retry_interval(0),
      m_ping_interval(0),
      m_connection_error_notified(false),
      m_url(),
      m_protocol(""),
      m_credentials(),
      m_connected(false),
      m_context(nullptr),
      m_sched_list(),
      m_wsi(nullptr),
      m_retry_policy(),
      m_retry_count(0),
      m_send_msgs()
{
}
/** @brief Destructor */
LibWebsocketClient::~LibWebsocketClient()
{
    // To prevent keeping an open connection in background
    disconnect();
}

/** @copydoc bool IWebsocketClient::connect(const std::string&, const std::string&, const Credentials&,
 *                                          std::chrono::milliseconds, std::chrono::milliseconds, std::chrono::milliseconds) */
bool LibWebsocketClient::connect(const std::string&        url,
                                 const std::string&        protocol,
                                 const Credentials&        credentials,
                                 std::chrono::milliseconds connect_timeout,
                                 std::chrono::milliseconds retry_interval,
                                 std::chrono::milliseconds ping_interval)
{
    bool ret = false;

    // Check if thread is alive and if a listener has been registered
    if (!m_thread && m_listener)
    {
        // Check URL
        m_url = url;
        if (m_url.isValid() && ((m_url.protocol() == "ws") || (m_url.protocol() == "wss")))
        {
            // Define callback
            static const struct lws_protocols protocols[] = {
                {"LibWebsocketClient", &LibWebsocketClient::eventCallback, 0, 0, 0, nullptr, 0}, {nullptr, nullptr, 0, 0, 0, nullptr, 0}};

            // Fill context information
            struct lws_context_creation_info info;
            memset(&info, 0, sizeof info);
            info.options      = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
            info.port         = CONTEXT_PORT_NO_LISTEN;
            info.protocols    = protocols;
            info.timeout_secs = static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::seconds>(connect_timeout).count());
            m_credentials     = credentials;
            if (!m_credentials.tls12_cipher_list.empty())
            {
                info.ssl_cipher_list = m_credentials.tls12_cipher_list.c_str();
            }
            if (!m_credentials.tls13_cipher_list.empty())
            {
                info.tls1_3_plus_cipher_list = m_credentials.tls13_cipher_list.c_str();
            }
            if (!m_credentials.ecdh_curve.empty())
            {
                info.ecdh_curve = m_credentials.ecdh_curve.c_str();
            }

            // Create context
            m_context = lws_create_context(&info);
            if (m_context)
            {
                // Schedule first connection now
                memset(&m_sched_list, 0, sizeof(m_sched_list));
                lws_sul_schedule(m_context, 0, &m_sched_list, LibWebsocketClient::connectCallback, 1);

                // Start connection process
                m_end                       = false;
                m_connection_error_notified = false;
                m_connected                 = false;
                m_retry_interval            = static_cast<uint32_t>(retry_interval.count());
                m_ping_interval = static_cast<uint16_t>(std::chrono::duration_cast<std::chrono::seconds>(ping_interval).count());
                m_retry_count   = 0;
                m_protocol      = protocol;
                m_thread        = new std::thread(std::bind(&LibWebsocketClient::process, this));
                ret             = true;
            }
        }
    }

    return ret;
}

/** @copydoc bool IWebsocketClient::disconnect() */
bool LibWebsocketClient::disconnect()
{
    bool ret = false;

    // Check if thread is alive
    if (m_thread)
    {
        // Stop thread
        m_end = true;
        SendMsg* msg;
        while (m_send_msgs.pop(msg, 0))
        {
            delete msg;
        }
        lws_cancel_service(m_context);
        if (std::this_thread::get_id() != m_thread->get_id())
        {
            m_thread->join();
        }
        else
        {
            m_thread->detach();
        }
        delete m_thread;
        m_thread    = nullptr;
        m_connected = false;
        ret         = true;
    }

    return ret;
}

/** @copydoc bool IWebsocketClient::isConnected() */
bool LibWebsocketClient::isConnected()
{
    return m_connected;
}

/** @copydoc bool IWebsocketClient::send(const void*, size_t) */
bool LibWebsocketClient::send(const void* data, size_t size)
{
    bool ret = false;

    // Check if connected
    if (m_connected)
    {
        // Prepare data to send
        SendMsg* msg = new SendMsg(data, size);
        ret          = m_send_msgs.push(msg);

        // Schedule a send
        lws_callback_on_writable(m_wsi);
    }

    return ret;
}

/** @copydoc void IWebsocketClient::registerListener(IWebsocketClientListener&) */
void LibWebsocketClient::registerListener(IWebsocketClientListener& listener)
{
    m_listener = &listener;
}

/** @brief Internal thread */
void LibWebsocketClient::process()
{
    // Save this pointer for further callbacks
    client = this;

    // Event loop
    int ret = 0;
    while (!m_end && (ret >= 0))
    {
        ret = lws_service(m_context, 0);
    }
    if (!m_end)
    {
        disconnect();
        m_listener->wsClientError();
    }

    // Destroy context
    lws_context_destroy(m_context);
}

/** @brief libwebsockets connection callback */
void LibWebsocketClient::connectCallback(struct lws_sorted_usec_list* sul)
{
    // Configure retry policy
    client->m_retry_policy = {
        .retry_ms_table       = &client->m_retry_interval,
        .retry_ms_table_count = 1,
        .conceal_count        = 1,

        .secs_since_valid_ping   = client->m_ping_interval,                             /* force PINGs after secs idle */
        .secs_since_valid_hangup = static_cast<uint16_t>(2u * client->m_ping_interval), /* hangup after secs idle */

        .jitter_percent = 20,
    };

    // Connexion parameters
    struct lws_client_connect_info i;
    memset(&i, 0, sizeof(i));
    i.context = client->m_context;
    i.address = client->m_url.address().c_str();
    i.path    = client->m_url.path().c_str();
    i.host    = i.address;
    i.origin  = i.address;
    if (client->m_url.protocol() == "wss")
    {
        i.ssl_connection = LCCSCF_USE_SSL;
        if (client->m_credentials.allow_selfsigned_certificates)
        {
            i.ssl_connection |= LCCSCF_ALLOW_SELFSIGNED;
        }
        if (client->m_credentials.allow_expired_certificates)
        {
            i.ssl_connection |= LCCSCF_ALLOW_EXPIRED;
        }
        if (client->m_credentials.accept_untrusted_certificates)
        {
            i.ssl_connection |= LCCSCF_ALLOW_INSECURE;
        }
        if (client->m_credentials.skip_server_name_check)
        {
            i.ssl_connection |= LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;
        }
        i.port = 443;
    }
    else
    {
        i.port = 80;
    }
    if (client->m_url.port())
    {
        i.port = client->m_url.port();
    }
    i.protocol              = client->m_protocol.c_str();
    i.local_protocol_name   = "LibWebsocketClient";
    i.pwsi                  = &client->m_wsi;
    i.retry_and_idle_policy = &client->m_retry_policy;
    i.userdata              = client;

    // Start connection
    if (!lws_client_connect_via_info(&i))
    {
        // Schedule a retry
        client->m_retry_count = 0;
        lws_retry_sul_schedule(
            client->m_context, 0, sul, &client->m_retry_policy, &LibWebsocketClient::connectCallback, &client->m_retry_count);
    }
}

/** @brief libwebsockets event callback */
int LibWebsocketClient::eventCallback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len)
{
    int  ret   = 0;
    bool retry = false;
    (void)user;

    switch (reason)
    {
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            if (!client->m_connection_error_notified)
            {
                client->m_connection_error_notified = true;
                client->m_listener->wsClientFailed();
            }
            if (client->m_retry_interval != 0)
            {
                retry = true;
            }
            break;

        case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
        {
            unsigned char **p = (unsigned char**)in, *end = (*p) + len;
            char            b[128];

            if (client->m_credentials.user.empty())
                break;

            if (lws_http_basic_auth_gen(client->m_credentials.user.c_str(), client->m_credentials.password.c_str(), b, sizeof(b)))
                break;
            if (lws_add_http_header_by_token(wsi, WSI_TOKEN_HTTP_AUTHORIZATION, (unsigned char*)b, (int)strlen(b), p, end))
                return -1;

            break;
        }

        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            client->m_connected = true;
            client->m_listener->wsClientConnected();
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            client->m_listener->wsClientDataReceived(in, len);
            break;

        case LWS_CALLBACK_CLIENT_WRITEABLE:
        {
            // Send data if any ready
            SendMsg* msg = nullptr;
            if (client->m_send_msgs.pop(msg, 0))
            {
                if (lws_write(wsi, msg->payload, msg->size, LWS_WRITE_TEXT) < static_cast<int>(msg->size))
                {
                    // Error
                    client->disconnect();
                    client->m_listener->wsClientError();
                }

                // Free message memory
                delete msg;
            }
        }
        break;

        case LWS_CALLBACK_CLIENT_CLOSED:
            client->m_connected = false;
            client->m_listener->wsClientDisconnected();
            if (client->m_retry_interval != 0)
            {
                retry = true;
            }
            break;

        default:
            break;
    }
    if (retry)
    {
        // Schedule a retry
        client->m_retry_count = 0;
        lws_retry_sul_schedule_retry_wsi(wsi, &client->m_sched_list, &LibWebsocketClient::connectCallback, &client->m_retry_count);
    }
    else
    {
        ret = lws_callback_http_dummy(wsi, reason, user, in, len);
    }

    return ret;
}

} // namespace websockets
} // namespace ocpp
