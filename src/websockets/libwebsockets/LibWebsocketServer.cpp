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

#include "LibWebsocketServer.h"

#include <csignal>
#include <cstdint>
#include <functional>
#include <iostream>

namespace ocpp
{
namespace websockets
{

/** @brief Thread local server instance used when callbacks doesn't provide user data */
thread_local LibWebsocketServer* server;

/** @brief Constructor */
LibWebsocketServer::LibWebsocketServer()
    : IWebsocketServer(),
      m_listener(nullptr),
      m_thread(nullptr),
      m_end(false),
      m_url(),
      m_protocol(""),
      m_credentials(),
      m_context(nullptr),
      m_wsi(nullptr),
      m_retry_policy(),
      m_protocols(),
      m_clients()
{
}
/** @brief Destructor */
LibWebsocketServer::~LibWebsocketServer()
{
    // To prevent keeping a started server in background
    stop();
}

/** @copydoc bool IWebsocketServer::start(const std::string&, const std::string&, const Credentials&,
 *                                        std::chrono::milliseconds) */
bool LibWebsocketServer::start(const std::string&        url,
                               const std::string&        protocol,
                               const Credentials&        credentials,
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
            m_protocol     = protocol;
            m_protocols[0] = {m_protocol.c_str(), &LibWebsocketServer::eventCallback, 0, 0, 0, nullptr, 0};
            m_protocols[1] = {nullptr, nullptr, 0, 0, 0, nullptr, 0};

            // Retry policy
            uint16_t ping  = static_cast<uint16_t>(std::chrono::duration_cast<std::chrono::seconds>(ping_interval).count());
            m_retry_policy = {.retry_ms_table       = nullptr,
                              .retry_ms_table_count = 0,
                              .conceal_count        = 0,

                              .secs_since_valid_ping   = ping,                             /* force PINGs after secs idle */
                              .secs_since_valid_hangup = static_cast<uint16_t>(2u * ping), /* hangup after secs idle */

                              .jitter_percent = 0};

            // Fill context information
            struct lws_context_creation_info info;
            memset(&info, 0, sizeof info);
            info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT | LWS_SERVER_OPTION_SKIP_SERVER_CANONICAL_NAME |
                           LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;
            if (m_url.port() != 0)
            {
                info.port = m_url.port();
            }
            else
            {
                // Default to port 80 for ws:// and 443 for wss://
                if (m_url.protocol() == "ws")
                {
                    info.port = 80;
                }
                else
                {
                    info.port = 443;
                }
            }
            info.protocols             = &m_protocols[0];
            info.retry_and_idle_policy = &m_retry_policy;
            m_credentials              = credentials;
            if (m_url.protocol() == "wss")
            {
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
                if (m_credentials.encoded_pem_certificates)
                {
                    // Use PEM encoded data
                    if (!m_credentials.server_certificate.empty())
                    {
                        info.server_ssl_cert_mem     = m_credentials.server_certificate.c_str();
                        info.server_ssl_cert_mem_len = m_credentials.server_certificate.size();
                    }
                    if (!m_credentials.server_certificate_private_key.empty())
                    {
                        info.server_ssl_private_key_mem     = m_credentials.server_certificate_private_key.c_str();
                        info.server_ssl_private_key_mem_len = m_credentials.server_certificate_private_key.size();
                    }
                    if (!m_credentials.server_certificate_ca.empty())
                    {
                        info.server_ssl_ca_mem     = m_credentials.server_certificate_ca.c_str();
                        info.server_ssl_ca_mem_len = m_credentials.server_certificate_ca.size();
                    }
                }
                else
                {
                    // Load PEM files from filesystem
                    if (!m_credentials.server_certificate.empty())
                    {
                        info.ssl_cert_filepath = m_credentials.server_certificate.c_str();
                    }
                    if (!m_credentials.server_certificate_private_key.empty())
                    {
                        info.ssl_private_key_filepath = m_credentials.server_certificate_private_key.c_str();
                    }
                    if (!m_credentials.server_certificate_ca.empty())
                    {
                        info.ssl_ca_filepath = m_credentials.server_certificate_ca.c_str();
                    }
                }
                if (!m_credentials.server_certificate_private_key_passphrase.empty())
                {
                    info.ssl_private_key_password = m_credentials.server_certificate_private_key_passphrase.c_str();
                }
                if (m_credentials.client_certificate_authent)
                {
                    info.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
                }
            }

            // Create context
            m_context = lws_create_context(&info);
            if (m_context)
            {
                // Start server
                m_end    = false;
                m_thread = new std::thread(std::bind(&LibWebsocketServer::process, this));
                ret      = true;
            }
        }
    }

    return ret;
} // namespace websockets

/** @copydoc bool IWebsocketServer::stop() */
bool LibWebsocketServer::stop()
{
    bool ret = false;

    // Check if thread is alive
    if (m_thread)
    {
        // Stop thread
        m_end = true;
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
        m_thread = nullptr;
        ret      = true;
    }

    return ret;
}

/** @copydoc void IWebsocketServer::registerListener(IListener&) */
void LibWebsocketServer::registerListener(IListener& listener)
{
    m_listener = &listener;
}

/** @brief Internal thread */
void LibWebsocketServer::process()
{
    // Save this pointer for further callbacks
    server = this;

    // Mask SIG_PIPE signal
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    // Event loop
    int ret = 0;
    while (!m_end && (ret >= 0))
    {
        ret = lws_service(m_context, 0);
    }
    if (!m_end)
    {
        stop();
        m_listener->wsServerError();
    }

    // Destroy context
    lws_context_destroy(m_context);
}

/** @brief libwebsockets event callback */
int LibWebsocketServer::eventCallback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len)
{
    int ret = 0;
    (void)user;

    switch (reason)
    {
        case LWS_CALLBACK_PROTOCOL_INIT:
            server->m_wsi = wsi;
            break;

        case LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
        {
            // Check selected protocol
            if (strcmp("websocket", static_cast<char*>(in)) == 0)
            {
                // Check URI
                char uri[lws_hdr_total_length(wsi, WSI_TOKEN_GET_URI) + 1];
                int  len = lws_hdr_copy(wsi, uri, sizeof(uri), WSI_TOKEN_GET_URI);
                if ((len >= static_cast<int>(server->m_url.path().size())) &&
                    (strncmp(uri, server->m_url.path().c_str(), server->m_url.path().size()) == 0))
                {
                    // Check basic authent
                    if (server->m_credentials.http_basic_authent)
                    {
                        // The following code snippet is from the lws_check_basic_auth() function in server.c file of libwebsockets
                        // BEGIN SNIPPET

                        // Did he send auth?
                        int ml = lws_hdr_total_length(wsi, WSI_TOKEN_HTTP_AUTHORIZATION);
                        if (!ml)
                        {
                            lwsl_err("missing basic authent header\n");
                            ret = -1;
                        }
                        else
                        {
                            char b64[160];
                            int  m = lws_hdr_copy(wsi, b64, sizeof(b64), WSI_TOKEN_HTTP_AUTHORIZATION);
                            if (m < 7)
                            {
                                lwsl_err("b64 auth too long\n");
                                ret = -1;
                            }
                            else
                            {
                                b64[5] = '\0';
                                if (strcasecmp(b64, "Basic"))
                                {
                                    lwsl_err("auth missing basic: %s\n", b64);
                                    ret = -1;
                                }
                                else
                                {
                                    // It'll be like Authorization: Basic QWxhZGRpbjpPcGVuU2VzYW1l
                                    char plain[(sizeof(b64) * 3) / 4];
                                    m = lws_b64_decode_string(b64 + 6, plain, sizeof(plain) - 1);
                                    if (m < 0)
                                    {
                                        lwsl_err("plain auth too long\n");
                                        ret = -1;
                                    }
                                    else
                                    {
                                        plain[m]     = '\0';
                                        char* pcolon = strchr(plain, ':');
                                        if (!pcolon)
                                        {
                                            lwsl_err("basic auth format broken\n");
                                            ret = -1;
                                        }
                                        else
                                        {
                                            // Check credentials
                                            std::string user(plain, static_cast<size_t>(pcolon - plain));
                                            std::string password(pcolon + 1u);
                                            if (!server->m_listener->wsCheckCredentials(uri, user, password))
                                            {
                                                // The following code snippet is from the lws_unauthorised_basic_auth() function in server.c file of libwebsockets
                                                // BEGIN SNIPPET
                                                unsigned char  frame_buffer[LWS_PRE + 1024u];
                                                unsigned char* start = &frame_buffer[LWS_PRE];
                                                unsigned char* p     = start;
                                                unsigned char* end   = &frame_buffer[sizeof(frame_buffer) - 1u];
                                                char           buf[64];
                                                if (!lws_add_http_header_status(wsi, HTTP_STATUS_UNAUTHORIZED, &p, end))
                                                {
                                                    int n = lws_snprintf(buf, sizeof(buf), "Basic realm=\"Open OCPP\"");
                                                    n     = lws_add_http_header_by_token(
                                                        wsi, WSI_TOKEN_HTTP_WWW_AUTHENTICATE, (unsigned char*)buf, n, &p, end);
                                                    n += lws_add_http_header_content_length(wsi, 0, &p, end);
                                                    n += lws_finalize_http_header(wsi, &p, end);
                                                    n += lws_write(
                                                        wsi,
                                                        start,
                                                        lws_ptr_diff_size_t(p, start),
                                                        static_cast<lws_write_protocol>(LWS_WRITE_HTTP_HEADERS | LWS_WRITE_H2_STREAM_END));
                                                    n += lws_http_transaction_completed(wsi);
                                                }
                                                // END SNIPPET
                                                ret = -1;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        // END SNIPPET
                    }
                }
                else
                {
                    lwsl_err("invalid URI\n");
                    ret = -1;
                }
            }
            else
            {
                lwsl_err("invalid protocol\n");
                ret = -1;
            }
        }
        break;

        case LWS_CALLBACK_ESTABLISHED:
        {
            // Get client IP address
            char ip_address[64];
            lws_get_peer_simple(wsi, ip_address, sizeof(ip_address));

            // Instanciate a new client
            std::shared_ptr<IClient> client(new Client(wsi, ip_address));
            server->m_clients[wsi] = client;

            // Notify connection
            char uri[lws_hdr_total_length(wsi, WSI_TOKEN_GET_URI) + 1];
            if (lws_hdr_copy(wsi, uri, sizeof(uri), WSI_TOKEN_GET_URI) <= 0)
            {
                uri[0] = 0;
            }
            server->m_listener->wsClientConnected(uri, client);
        }
        break;

        case LWS_CALLBACK_CLOSED:
        {
            // Get corresponding client
            auto iter_client = server->m_clients.find(wsi);
            if (iter_client != server->m_clients.end())
            {
                Client* client = dynamic_cast<Client*>(iter_client->second.get());

                // Disconnect client
                client->m_connected = false;

                // Notify client
                if (client->m_listener)
                {
                    client->m_listener->wsClientDisconnected();
                }

                // Remove client
                server->m_clients.erase(iter_client);
            }
        }
        break;

        case LWS_CALLBACK_SERVER_WRITEABLE:
        {
            // Get corresponding client
            auto iter_client = server->m_clients.find(wsi);
            if (iter_client != server->m_clients.end())
            {
                Client* client = dynamic_cast<Client*>(iter_client->second.get());
                if (client->m_connected)
                {

                    // Send data if any ready
                    SendMsg* msg = nullptr;
                    if (client->m_send_msgs.pop(msg, 0))
                    {
                        if (lws_write(client->m_wsi, msg->payload, msg->size, LWS_WRITE_TEXT) < static_cast<int>(msg->size))
                        {
                            // Error
                            client->disconnect(true);
                            if (client->m_listener)
                            {
                                client->m_listener->wsClientError();
                            }
                        }

                        // Free message memory
                        delete msg;
                    }
                }
                else
                {
                    // Close connection
                    lws_close_reason(client->m_wsi, LWS_CLOSE_STATUS_NORMAL, nullptr, 0);
                    ret = -1;
                }
            }
        }
        break;

        case LWS_CALLBACK_RECEIVE:
        {
            // Get corresponding client
            auto iter_client = server->m_clients.find(wsi);
            if (iter_client != server->m_clients.end())
            {
                Client* client = dynamic_cast<Client*>(iter_client->second.get());

                // Notify client
                if (client->m_listener)
                {
                    client->m_listener->wsClientDataReceived(in, len);
                }
            }
        }
        break;

        default:
            break;
    }

    return ret;
}

/** @brief Constructor */
LibWebsocketServer::Client::Client(struct lws* wsi, const char* ip_address)
    : m_wsi(wsi), m_ip_address(ip_address), m_connected(true), m_listener(nullptr), m_send_msgs()
{
}
/** @brief Destructor */
LibWebsocketServer::Client::~Client()
{
    disconnect(true);
}

/** @copydoc const std::string& IClient::ipAddress(bool) const */
const std::string& LibWebsocketServer::Client::ipAddress() const
{
    return m_ip_address;
}

/** @copydoc bool IClient::disconnect(bool) */
bool LibWebsocketServer::Client::disconnect(bool notify_disconnected)
{
    bool ret = false;

    // Check if connected
    if (m_connected)
    {
        // Disconnect
        m_connected = false;
        ret         = true;
        if (!notify_disconnected)
        {
            m_listener = nullptr;
        }

        // Schedule a close
        lws_callback_on_writable(m_wsi);
    }

    // Empty message queue
    SendMsg* msg = nullptr;
    while (m_send_msgs.pop(msg, 0))
    {
        delete msg;
    }

    return ret;
}

/** @copydoc bool IClient::isConnected() */
bool LibWebsocketServer::Client::isConnected()
{
    return m_connected;
}

/** @copydoc bool IClient::send(const void*, size_t) */
bool LibWebsocketServer::Client::send(const void* data, size_t size)
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

/** @copydoc bool IClient::registerListener(IListener&) */
void LibWebsocketServer::Client::registerListener(IClient::IListener& listener)
{
    m_listener = &listener;
}

} // namespace websockets
} // namespace ocpp
