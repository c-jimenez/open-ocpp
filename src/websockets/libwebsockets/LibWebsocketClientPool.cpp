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

#include "LibWebsocketClientPool.h"

#include <csignal>
#include <cstdint>
#include <functional>

/** @brief Generate basic authent header with bytes password (may contain \0 char) => implemented in LibWebsocketClient.cpp*/
extern int lws_http_basic_auth_gen2(const char* user, const void* pw, size_t pwd_len, char* buf, size_t len);

namespace ocpp
{
namespace websockets
{

/** @brief Thread local client instance used when callbacks doesn't provide user data */
thread_local LibWebsocketClientPool* pool;

/** @brief Constructor */
LibWebsocketClientPool::LibWebsocketClientPool()
    : m_context(nullptr),
      m_logs_context(),
      m_thread(nullptr),
      m_end(false),
      m_clients_count(0),
      m_clients_count_mutex(),
      m_waiting_connect_queue(),
      m_waiting_disconnect_queue(),
      m_waiting_send_queue()
{
}

/** @brief Destructor */
LibWebsocketClientPool::~LibWebsocketClientPool()
{
    stop();
}

/** @brief Start the pool */
bool LibWebsocketClientPool::start()
{
    bool ret = false;

    // Check if thread is alive and if a listener has been registered
    if (!m_thread)
    {
        // Fill context information
        struct lws_context_creation_info info;
        memset(&info, 0, sizeof(info));
        info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT | LWS_SERVER_OPTION_EXPLICIT_VHOSTS;
        info.user    = this;
        info.log_cx  = &m_logs_context;

        memset(&m_logs_context, 0, sizeof(m_logs_context));
        m_logs_context.u.emit    = LIBWEBSOCKET_LOG_OUTPUT_FN;
        m_logs_context.lll_flags = LIBWEBSOCKET_LOG_FLAGS;

        // Create context
        m_context = lws_create_context(&info);
        if (m_context)
        {
            // Start process thread
            m_end    = false;
            m_thread = new std::thread(std::bind(&LibWebsocketClientPool::process, this));
            ret      = true;
        }
    }

    return ret;
}

/** @brief Stop the pool */
bool LibWebsocketClientPool::stop()
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

/** @brief Get the number of clients */
size_t LibWebsocketClientPool::getClientsCount() const
{
    std::lock_guard<std::mutex> lock(m_clients_count_mutex);
    return m_clients_count;
}

/** @brief Instanciate a client websocket */
IWebsocketClient* LibWebsocketClientPool::newClient()
{
    return new Client(*this);
}

/** @brief Internal thread */
void LibWebsocketClientPool::process()
{
    // Save this pointer for further callbacks
    pool = this;

    // Mask SIG_PIPE signal
#ifndef _MSC_VER
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
#endif // _MSC_VER

    // Need to ensure that the context is still valid when a user callback
    // has called stop() function
    lws_context* context = m_context;

    // Dummy vhost to handle context related events
    struct lws_protocols             protocols[] = {{"LibWebsocketClientPool", &LibWebsocketClientPool::eventCallback, 0, 0, 0, this, 0},
                                        LWS_PROTOCOL_LIST_TERM};
    struct lws_context_creation_info vhost_info;
    memset(&vhost_info, 0, sizeof(vhost_info));
    vhost_info.protocols = protocols;
    vhost_info.log_cx    = &m_logs_context;
    lws_vhost* vhost     = lws_create_vhost(m_context, &vhost_info);

    // Event loop
    int ret = 0;
    while (!m_end && (ret >= 0))
    {
        ret = lws_service(context, 0);
    }
    if (!m_end)
    {
        stop();
    }

    // Destroy context
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Ensure stop caller is joining
    lws_vhost_destroy(vhost);
    lws_context_destroy(context);
}

/** @brief libwebsockets event callback */
int LibWebsocketClientPool::eventCallback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len) noexcept
{
    int ret = 0;

    (void)wsi;
    (void)in;
    (void)len;
    (void)user;

    // Handle event
    switch (reason)
    {
        case LWS_CALLBACK_EVENT_WAIT_CANCELLED:
        {
            // Schedule first connections
            Client* waiting_client = nullptr;
            while (pool->m_waiting_connect_queue.pop(waiting_client, 0))
            {
                lws_sul_schedule(
                    pool->m_context, 0, &waiting_client->m_schedule_data.sched_list, LibWebsocketClientPool::Client::connectCallback, 1);
            }

            // Handle requested sends
            while (pool->m_waiting_send_queue.pop(waiting_client, 0))
            {
                if (waiting_client->m_connected)
                {
                    lws_callback_on_writable(waiting_client->m_wsi);
                }
            }

            // Handle requested disconnections
            while (pool->m_waiting_disconnect_queue.pop(waiting_client, 0))
            {
                lws_sul_cancel(&waiting_client->m_schedule_data.sched_list);
                if (waiting_client->m_connected)
                {
                    lws_set_timeout(waiting_client->m_wsi, static_cast<pending_timeout>(1), LWS_TO_KILL_SYNC);
                }
                if (waiting_client->m_vhost)
                {
                    lws_vhost_destroy(waiting_client->m_vhost);
                }

                std::lock_guard<std::mutex> lock(waiting_client->m_disconnect_mutex);
                waiting_client->m_protocol.clear();
                waiting_client->m_vhost                          = nullptr;
                waiting_client->m_connected                      = false;
                waiting_client->m_disconnect_process_in_progress = false;
                waiting_client->m_disconnect_process_done        = true;
                waiting_client->m_disconnect_cond_var.notify_all();
            }
        }
        break;

        default:
            break;
    }

    return ret;
}

/** @brief Constructor */
LibWebsocketClientPool::Client::Client(LibWebsocketClientPool& pool)
    : IWebsocketClient(),
      m_pool(pool),
      m_listener(nullptr),
      m_connect_timeout(0),
      m_retry_interval(0),
      m_ping_interval(0),
      m_connection_error_notified(false),
      m_url(),
      m_protocol(""),
      m_credentials(),
      m_connected(false),
      m_disconnect_cond_var(),
      m_disconnect_mutex(),
      m_disconnect_process_in_progress(false),
      m_disconnect_process_done(false),
      m_context(m_pool.m_context),
      m_vhost(nullptr),
      m_wsi(nullptr),
      m_retry_policy(),
      m_retry_count(0),
      m_schedule_data(),
      m_send_msgs(),
      m_fragmented_frame(nullptr),
      m_fragmented_frame_size(0),
      m_fragmented_frame_index(0)
{
    // Increase client count in the associated pool
    std::lock_guard<std::mutex> lock(m_pool.m_clients_count_mutex);
    m_pool.m_clients_count++;
}

/** @brief Destructor */
LibWebsocketClientPool::Client::~Client()
{
    // To prevent keeping an open connection in background
    disconnect();
    releaseFragmentedFrame();

    // Decrease client count in the associated pool
    std::lock_guard<std::mutex> lock(m_pool.m_clients_count_mutex);
    m_pool.m_clients_count--;
}

/** @copydoc bool IWebsocketClient::connect(const std::string&, const std::string&, const Credentials&,
 *                                          std::chrono::milliseconds, std::chrono::milliseconds, std::chrono::milliseconds) */
bool LibWebsocketClientPool::Client::connect(const std::string&        url,
                                             const std::string&        protocol,
                                             const Credentials&        credentials,
                                             std::chrono::milliseconds connect_timeout,
                                             std::chrono::milliseconds retry_interval,
                                             std::chrono::milliseconds ping_interval)
{
    bool ret = false;

    std::lock_guard<std::mutex> lock(m_disconnect_mutex);

    // Check if thread is alive and if a listener has been registered
    if (!m_vhost && m_listener)
    {
        // Check URL
        m_url = url;
        if (m_url.isValid() && ((m_url.protocol() == "ws") || (m_url.protocol() == "wss")))
        {
            // Save connection parameters
            m_protocol        = protocol;
            m_credentials     = credentials;
            m_connect_timeout = static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::seconds>(connect_timeout).count());
            m_retry_interval  = static_cast<uint32_t>(retry_interval.count());
            m_ping_interval   = static_cast<uint16_t>(std::chrono::duration_cast<std::chrono::seconds>(ping_interval).count());

            // Configure retry policy
#ifdef _MSC_VER
            m_retry_policy = {&m_retry_interval, 1, 1, m_ping_interval, static_cast<uint16_t>(2u * m_ping_interval), 20};
#else
            m_retry_policy = {
                .retry_ms_table       = &m_retry_interval,
                .retry_ms_table_count = 1,
                .conceal_count        = 1,

                .secs_since_valid_ping   = m_ping_interval,                             /* force PINGs after secs idle */
                .secs_since_valid_hangup = static_cast<uint16_t>(2u * m_ping_interval), /* hangup after secs idle */

                .jitter_percent = 20,
            };
#endif // _MSC_VER

            // Initialize schedule data
            memset(&m_schedule_data, 0, sizeof(m_schedule_data));
            m_schedule_data.client = this;

            // Start connection process
            m_connection_error_notified = false;
            m_connected                 = false;
            m_pool.m_waiting_connect_queue.push(this);
            lws_cancel_service(m_context);

            ret = true;
        }
    }

    return ret;
}

/** @copydoc bool IWebsocketClient::disconnect() */
bool LibWebsocketClientPool::Client::disconnect()
{
    bool ret = false;

    std::unique_lock<std::mutex> lock(m_disconnect_mutex);

    // Check if connected
    if (!m_disconnect_process_in_progress && !m_protocol.empty())
    {
        // Schedule disconnection
        m_retry_interval                 = 0;
        m_disconnect_process_in_progress = true;
        m_disconnect_process_done        = false;
        m_pool.m_waiting_disconnect_queue.push(this);
        lws_cancel_service(m_context);

        // Wait actual disconnection
        if (std::this_thread::get_id() != m_pool.m_thread->get_id())
        {
            m_disconnect_cond_var.wait(lock, [&] { return m_disconnect_process_done; });
        }
    }

    // Clear message queue
    SendMsg* msg;
    while (m_send_msgs.pop(msg, 0))
    {
        delete msg;
    }

    ret = true;

    return ret;
}

/** @copydoc bool IWebsocketClient::isConnected() */
bool LibWebsocketClientPool::Client::isConnected()
{
    return m_connected;
}

/** @copydoc bool IWebsocketClient::send(const void*, size_t) */
bool LibWebsocketClientPool::Client::send(const void* data, size_t size)
{
    bool ret = false;

    std::lock_guard<std::mutex> lock(m_disconnect_mutex);

    // Check if connected
    if (m_connected)
    {
        // Prepare data to send
        SendMsg* msg = new SendMsg(data, size);
        ret          = m_send_msgs.push(msg);

        // Schedule a send
        m_pool.m_waiting_send_queue.push(this);
        lws_cancel_service(m_context);
    }

    return ret;
}

/** @copydoc void IWebsocketClient::registerListener(IListener&) */
void LibWebsocketClientPool::Client::registerListener(IListener& listener)
{
    m_listener = &listener;
}

/** @brief Prepare the buffer to store a new fragmented frame */
void LibWebsocketClientPool::Client::beginFragmentedFrame(size_t frame_size)
{
    // Release previously allocated data
    releaseFragmentedFrame();

    // Allocate new buffer
    m_fragmented_frame      = new uint8_t[frame_size];
    m_fragmented_frame_size = frame_size;
}

/** @brief Append data to the fragmented frame */
void LibWebsocketClientPool::Client::appendFragmentedData(const void* data, size_t size)
{
    size_t copy_len = size;
    if ((m_fragmented_frame_index + size) >= m_fragmented_frame_size)
    {
        copy_len = m_fragmented_frame_size - m_fragmented_frame_index;
    }
    memcpy(&m_fragmented_frame[m_fragmented_frame_index], data, copy_len);
    m_fragmented_frame_index += copy_len;
}

/** @brief Release the memory associated with the fragmented frame */
void LibWebsocketClientPool::Client::releaseFragmentedFrame()
{
    delete[] m_fragmented_frame;
    m_fragmented_frame       = nullptr;
    m_fragmented_frame_size  = 0;
    m_fragmented_frame_index = 0;
}

/** @brief libwebsockets connection callback */
void LibWebsocketClientPool::Client::connectCallback(struct lws_sorted_usec_list* sul) noexcept
{
    // Get next client to connect
    ScheduleData* schedule_data = lws_container_of(sul, ScheduleData, sched_list);
    if (schedule_data)
    {
        Client*                     client = schedule_data->client;
        std::lock_guard<std::mutex> lock(client->m_disconnect_mutex);

        // Check if a disconnect process is in progress
        if (!client->m_disconnect_process_in_progress)
        {
            // Check if vhost has been created
            if (!client->m_vhost)
            {
                // Define callback
                struct lws_protocols protocols[] = {
                    {"LibWebsocketClientPoolClient", &LibWebsocketClientPool::Client::eventCallback, 0, 0, 0, client, 0},
                    LWS_PROTOCOL_LIST_TERM};

                // Fill vhost information
                struct lws_context_creation_info vhost_info;
                memset(&vhost_info, 0, sizeof(vhost_info));
                vhost_info.options              = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
                vhost_info.port                 = CONTEXT_PORT_NO_LISTEN;
                vhost_info.timeout_secs         = client->m_connect_timeout;
                vhost_info.connect_timeout_secs = client->m_connect_timeout;
                vhost_info.protocols            = protocols;
                vhost_info.log_cx               = &pool->m_logs_context;
                if (client->m_url.protocol() == "wss")
                {
                    if (!client->m_credentials.tls12_cipher_list.empty())
                    {
                        vhost_info.client_ssl_cipher_list = client->m_credentials.tls12_cipher_list.c_str();
                    }
                    if (!client->m_credentials.tls13_cipher_list.empty())
                    {
                        vhost_info.client_tls_1_3_plus_cipher_list = client->m_credentials.tls13_cipher_list.c_str();
                    }
                    if (client->m_credentials.encoded_pem_certificates)
                    {
                        // Use PEM encoded data
                        if (!client->m_credentials.server_certificate_ca.empty())
                        {
                            vhost_info.client_ssl_ca_mem = client->m_credentials.server_certificate_ca.c_str();
                            vhost_info.client_ssl_ca_mem_len =
                                static_cast<unsigned int>(client->m_credentials.server_certificate_ca.size());
                        }
                        if (!client->m_credentials.client_certificate.empty())
                        {
                            vhost_info.client_ssl_cert_mem     = client->m_credentials.client_certificate.c_str();
                            vhost_info.client_ssl_cert_mem_len = static_cast<unsigned int>(client->m_credentials.client_certificate.size());
                        }
                        if (!client->m_credentials.client_certificate_private_key.empty())
                        {
                            vhost_info.client_ssl_key_mem = client->m_credentials.client_certificate_private_key.c_str();
                            vhost_info.client_ssl_key_mem_len =
                                static_cast<unsigned int>(client->m_credentials.client_certificate_private_key.size());
                        }
                    }
                    else
                    {
                        // Load PEM files from filesystem
                        if (!client->m_credentials.server_certificate_ca.empty())
                        {
                            vhost_info.client_ssl_ca_filepath = client->m_credentials.server_certificate_ca.c_str();
                        }
                        if (!client->m_credentials.client_certificate.empty())
                        {
                            vhost_info.client_ssl_cert_filepath = client->m_credentials.client_certificate.c_str();
                        }
                        if (!client->m_credentials.client_certificate_private_key.empty())
                        {
                            vhost_info.client_ssl_private_key_filepath = client->m_credentials.client_certificate_private_key.c_str();
                        }
                    }
                    if (!client->m_credentials.client_certificate_private_key_passphrase.empty())
                    {
                        vhost_info.client_ssl_private_key_password =
                            client->m_credentials.client_certificate_private_key_passphrase.c_str();
                    }
                }

                // Create vhost
                client->m_vhost = lws_create_vhost(client->m_context, &vhost_info);
            }
            if (client->m_vhost)
            {
                // Connexion parameters
                struct lws_client_connect_info connect_info;
                memset(&connect_info, 0, sizeof(connect_info));
                connect_info.context = client->m_context;
                connect_info.vhost   = client->m_vhost;
                connect_info.address = client->m_url.address().c_str();
                connect_info.path    = client->m_url.path().c_str();
                if (client->m_credentials.server_name.empty())
                {
                    connect_info.host = connect_info.address;
                }
                else
                {
                    connect_info.host = client->m_credentials.server_name.c_str();
                }
                connect_info.origin = connect_info.address;
                if (client->m_url.protocol() == "wss")
                {
                    connect_info.ssl_connection = LCCSCF_USE_SSL;
                    if (client->m_credentials.allow_selfsigned_certificates)
                    {
                        connect_info.ssl_connection |= LCCSCF_ALLOW_SELFSIGNED;
                    }
                    if (client->m_credentials.allow_expired_certificates)
                    {
                        connect_info.ssl_connection |= LCCSCF_ALLOW_EXPIRED;
                    }
                    if (client->m_credentials.accept_untrusted_certificates)
                    {
                        connect_info.ssl_connection |= LCCSCF_ALLOW_INSECURE;
                    }
                    if (client->m_credentials.skip_server_name_check)
                    {
                        connect_info.ssl_connection |= LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;
                    }
                    connect_info.port = 443;
                }
                else
                {
                    connect_info.port = 80;
                }
                if (client->m_url.port())
                {
                    connect_info.port = static_cast<int>(client->m_url.port());
                }
                connect_info.protocol              = client->m_protocol.c_str();
                connect_info.local_protocol_name   = "LibWebsocketClientPoolClient";
                connect_info.pwsi                  = &client->m_wsi;
                connect_info.retry_and_idle_policy = &client->m_retry_policy;
                connect_info.userdata              = client;

                // Start connection
                if (!lws_client_connect_via_info(&connect_info))
                {
                    // Schedule a retry
                    client->m_retry_count = 0;
                    lws_retry_sul_schedule(pool->m_context,
                                           0,
                                           sul,
                                           &client->m_retry_policy,
                                           &LibWebsocketClientPool::Client::connectCallback,
                                           &client->m_retry_count);
                }
            }
        }
    }
}

/** @brief libwebsockets event callback */
int LibWebsocketClientPool::Client::eventCallback(
    struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len) noexcept
{
    int  ret   = 0;
    bool retry = false;

    // Get corresponding client
    Client* client = reinterpret_cast<Client*>(user);

    // Handle event
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

            if (lws_http_basic_auth_gen2(client->m_credentials.user.c_str(),
                                         client->m_credentials.password.data(),
                                         client->m_credentials.password.size(),
                                         b,
                                         sizeof(b)))
                break;
            if (lws_add_http_header_by_token(wsi, WSI_TOKEN_HTTP_AUTHORIZATION, (unsigned char*)b, (int)strlen(b), p, end))
                return -1;

            break;
        }

        case LWS_CALLBACK_CLIENT_ESTABLISHED:
        {
            client->m_connected = true;
            client->m_listener->wsClientConnected();
            break;
        }

        case LWS_CALLBACK_CLIENT_RECEIVE:
        {
            if (client->m_listener)
            {
                // Get frame info
                bool   is_first         = (lws_is_first_fragment(wsi) == 1);
                bool   is_last          = (lws_is_final_fragment(wsi) == 1);
                size_t remaining_length = lws_remaining_packet_payload(wsi);
                if (is_first && is_last)
                {
                    // Notify client
                    client->m_listener->wsClientDataReceived(in, len);
                }
                else if (is_first)
                {
                    // Prepare frame bufferization
                    client->beginFragmentedFrame(len + remaining_length);
                    client->appendFragmentedData(in, len);
                }
                else
                {
                    // Bufferize data
                    client->appendFragmentedData(in, len);
                    if (is_last)
                    {
                        // Notify client
                        client->m_listener->wsClientDataReceived(client->m_fragmented_frame, client->m_fragmented_frame_size);

                        // Release resources
                        client->releaseFragmentedFrame();
                    }
                }
            }
        }
        break;

        case LWS_CALLBACK_CLIENT_WRITEABLE:
        {
            // Send data if any ready
            bool     error = false;
            SendMsg* msg   = nullptr;
            while (client->m_send_msgs.pop(msg, 0) && !error)
            {
                if (lws_write(wsi, msg->payload, msg->size, LWS_WRITE_TEXT) < static_cast<int>(msg->size))
                {
                    // Error, close the socket
                    error = true;
                }

                // Free message memory
                delete msg;
            }
            if (error)
            {
                return -1;
            }
        }
        break;

        case LWS_CALLBACK_CLOSED_CLIENT_HTTP:
            if (client->m_retry_interval != 0)
            {
                retry = true;
            }
            break;

        case LWS_CALLBACK_CLIENT_CLOSED:
            client->m_connected = false;
            client->m_listener->wsClientDisconnected();
            if (client->m_retry_interval != 0)
            {
                retry = true;
            }

            SendMsg* msg;
            while (client->m_send_msgs.pop(msg, 0))
            {
                delete msg;
            }
            break;

        default:
            break;
    }
    if (retry)
    {
        // Schedule a retry
        client->m_retry_count = 0;
        lws_retry_sul_schedule_retry_wsi(
            wsi, &client->m_schedule_data.sched_list, &LibWebsocketClientPool::Client::connectCallback, &client->m_retry_count);
    }
    else
    {
        ret = lws_callback_http_dummy(wsi, reason, user, in, len);
    }

    return ret;
}

} // namespace websockets
} // namespace ocpp
