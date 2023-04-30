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

#include "WebsocketFactory.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest_wrapper.h"

#include <thread>
#include <vector>

using namespace ocpp::websockets;
using namespace std::chrono_literals;
using namespace std::string_literals;

class WsListener : public IWebsocketClient::IListener
{
  public:
    /** @brief Constructor */
    WsListener(const std::string& name, IWebsocketClient& client) : m_name(name), m_client(client) { }

    /** @brief Called when connection is successfull */
    void wsClientConnected() override { std::cout << "[" << m_name << "] - Connected!" << std::endl; }

    /** @brief Called when connection failed */
    void wsClientFailed() override { std::cout << "[" << m_name << "] - Connection failed!" << std::endl; }

    /** @brief Called when connection is lost */
    void wsClientDisconnected() override { std::cout << "[" << m_name << "] - Disconnected!" << std::endl; }

    /** @brief Called when a critical error occured */
    void wsClientError() override { std::cout << "[" << m_name << "] - Error!" << std::endl; }

    /** @brief Call when data has been received */
    void wsClientDataReceived(const void* data, size_t size) override
    {
        (void)data;
        std::cout << "[" << m_name << "] - Data received (" << size << "bytes)!" << std::endl;
        m_client.send(data, size);
        m_client.disconnect();
    }

  private:
    const std::string m_name;
    IWebsocketClient& m_client;
};

TEST_SUITE("Nominal")
{
    TEST_CASE("Pool")
    {
        std::vector<std::thread*> client_threads;

        WebsocketFactory::setClientPoolCount(4u);
        WebsocketFactory::startClientPools();

        for (size_t i = 0; i < 200u; i++)
        {
            client_threads.push_back(new std::thread(
                [i]
                {
                    std::string       name   = "client_" + std::to_string(i);
                    IWebsocketClient* client = WebsocketFactory::newClientFromPool();
                    WsListener        client_listener(name, *client);
                    client->registerListener(client_listener);

                    IWebsocketClient::Credentials credentials;
                    credentials.allow_selfsigned_certificates = true;
                    credentials.allow_expired_certificates    = true;
                    credentials.accept_untrusted_certificates = true;
                    credentials.skip_server_name_check        = true;
                    client->connect("wss://127.0.0.1:8080/openocpp/"s + name, "ocpp1.6"s, credentials);

                    std::this_thread::sleep_for(30s);

                    delete client;
                }));
        }

        for (auto& thread : client_threads)
        {
            thread->join();
            delete thread;
        }

        WebsocketFactory::stopClientPools();
    }
}
