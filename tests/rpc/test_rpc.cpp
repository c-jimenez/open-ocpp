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

#include "RpcClient.h"
#include "WebsocketClientStub.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <cstring>
#include <thread>

using namespace ocpp::websockets;
using namespace ocpp::rpc;

class RpcClientListener : public IRpc::IListener, public RpcClient::IListener
{
  public:
    RpcClientListener()
        : connected(false),
          failed(false),
          error(false),
          action(),
          payload(),
          response(nullptr),
          error_code(nullptr),
          error_message(nullptr),
          received_error(false)
    {
    }
    virtual ~RpcClientListener() { }

    /** @copydoc void RpcClient::IListener::rpcClientConnected() */
    void rpcClientConnected() override { connected = true; }

    /** @copydoc void RpcClient::IListener::rpcClientFailed() */
    void rpcClientFailed() override { failed = true; }

    /** @copydoc void IRpc::IListener::rpcDisconnected() */
    void rpcDisconnected() override { connected = false; }

    /** @copydoc void IRpc::IListener::rpcError() */
    void rpcError() override { error = true; }

    /** @copydoc void IRpc::IListener::rpcCallReceived(const std::string&,
                                                       const rapidjson::Value&,
                                                       rapidjson::Document&,
                                                       const char*&,
                                                       std::string&) */
    bool rpcCallReceived(const std::string&      action,
                         const rapidjson::Value& payload,
                         rapidjson::Document&    response,
                         const char*&            error_code,
                         std::string&            error_message) override
    {
        this->action = action;
        rapidjson::StringBuffer                    buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        payload.Accept(writer);
        this->payload = buffer.GetString();
        if (this->response)
        {
            response.Parse(this->response);
        }
        error_code = this->error_code;
        if (this->error_message)
        {
            error_message = this->error_message;
        }
        return !received_error;
    }

    bool        connected;
    bool        failed;
    bool        error;
    bool        received;
    std::string action;
    std::string payload;
    const char* response;
    const char* error_code;
    const char* error_message;
    bool        received_error;
};

static constexpr const char* WS_PROTOCOL = "ocpp1.6";
static constexpr const char* WS_URL      = "ws://localhost:8080/ocpp/";

TEST_SUITE("Initialization, connection/disconnection")
{
    TEST_CASE("Nominal")
    {
        RpcClientListener             listener;
        WebsocketClientStub           websocket;
        IWebsocketClient::Credentials credentials;
        RpcClient                     client(websocket, WS_PROTOCOL);
        client.registerListener(listener);
        client.registerClientListener(listener);

        CHECK(client.start(
            WS_URL, credentials, std::chrono::milliseconds(1500u), std::chrono::milliseconds(2500u), std::chrono::milliseconds(3500u)));
        CHECK(websocket.connectCalled());
        CHECK_EQ(websocket.protocol(), WS_PROTOCOL);
        CHECK_EQ(websocket.url(), WS_URL);
        CHECK_EQ(websocket.connectTimeout(), 1500u);
        CHECK_EQ(websocket.retryInterval(), 2500u);
        CHECK_EQ(websocket.pingInterval(), 3500u);

        websocket.notifyFailed();
        CHECK(listener.failed);

        websocket.notifyConnected();
        CHECK(listener.connected);

        CHECK(client.stop());
        CHECK(websocket.disconnectCalled());

        websocket.notifyDisconnected();
        CHECK_FALSE(listener.connected);
    }

    TEST_CASE("Error")
    {
        RpcClientListener             listener;
        WebsocketClientStub           websocket;
        IWebsocketClient::Credentials credentials;
        RpcClient                     client(websocket, WS_PROTOCOL);
        client.registerListener(listener);
        client.registerClientListener(listener);

        websocket.nextCallWillFail();
        CHECK_FALSE(client.start(
            WS_URL, credentials, std::chrono::milliseconds(1500u), std::chrono::milliseconds(2500u), std::chrono::milliseconds(3500u)));
        CHECK(websocket.connectCalled());

        websocket.reset();
        CHECK(client.start(
            WS_URL, credentials, std::chrono::milliseconds(1500u), std::chrono::milliseconds(2500u), std::chrono::milliseconds(3500u)));
        CHECK(websocket.connectCalled());

        websocket.nextCallWillFail();
        CHECK_FALSE(client.stop());
        CHECK(websocket.disconnectCalled());
    }
}

static constexpr const char* ACTION                        = "Heartbeat";
static constexpr const char* CALL_PAYLOAD                  = "{\"id\":4}";
static constexpr const char* CALLRESULT_PAYLOAD            = "{\"name\":\"bob\"}";
static constexpr const char* CALLERROR_PAYLOAD             = "This is an error!";
static constexpr const char* EXPECTED_CALL_MESSAGE_0       = "[2, \"0\", \"Heartbeat\", {\"id\":4}]";
static constexpr const char* EXPECTED_CALL_MESSAGE_1       = "[2, \"1\", \"Heartbeat\", {\"id\":4}]";
static constexpr const char* EXPECTED_CALL_MESSAGE_2       = "[2, \"2\", \"Heartbeat\", {\"id\":4}]";
static constexpr const char* EXPECTED_CALLRESULT_MESSAGE_1 = "[3, \"1\", {\"name\":\"bob\"}]";
static constexpr const char* EXPECTED_CALLRESULT_MESSAGE_2 = "[3, \"2\", {\"name\":\"bob\"}]";
static constexpr const char* EXPECTED_CALLERROR_MESSAGE_1  = "[4, \"1\", \"NotImplemented\", \"This is an error!\", {}]";

TEST_SUITE("CALL messages")
{
    TEST_CASE("Nominal")
    {
        RpcClientListener   listener;
        WebsocketClientStub websocket;
        RpcClient           client(websocket, WS_PROTOCOL);
        client.registerListener(listener);
        client.registerClientListener(listener);
        websocket.setConnected();

        rapidjson::Document payload;
        payload.Parse(CALL_PAYLOAD);

        rapidjson::Document                        rpc_frame;
        rapidjson::Value                           response;
        rapidjson::StringBuffer                    buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        CHECK_FALSE(client.call(ACTION, payload, rpc_frame, response, std::chrono::milliseconds(0)));
        CHECK(websocket.sendCalled());
        CHECK_EQ(strcmp(reinterpret_cast<const char*>(websocket.sentData()), EXPECTED_CALL_MESSAGE_0), 0);

        CHECK_FALSE(client.call(ACTION, payload, rpc_frame, response, std::chrono::milliseconds(0)));
        CHECK_EQ(strcmp(reinterpret_cast<const char*>(websocket.sentData()), EXPECTED_CALL_MESSAGE_1), 0);

        std::thread response_thread(
            [&websocket]
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(25u));
                websocket.notifyDataReceived(EXPECTED_CALLRESULT_MESSAGE_2, strlen(EXPECTED_CALLRESULT_MESSAGE_2));
            });
        CHECK(client.call(ACTION, payload, rpc_frame, response, std::chrono::milliseconds(50)));
        CHECK_EQ(strcmp(reinterpret_cast<const char*>(websocket.sentData()), EXPECTED_CALL_MESSAGE_2), 0);
        response.Accept(writer);
        CHECK_EQ(strcmp(buffer.GetString(), CALLRESULT_PAYLOAD), 0);
        response_thread.join();
    }

    TEST_CASE("Timeout")
    {
        RpcClientListener   listener;
        WebsocketClientStub websocket;
        RpcClient           client(websocket, WS_PROTOCOL);
        client.registerListener(listener);
        client.registerClientListener(listener);
        websocket.setConnected();

        rapidjson::Document payload;
        rapidjson::Document rpc_frame;
        rapidjson::Value    response;

        auto start = std::chrono::steady_clock::now();
        CHECK_FALSE(client.call(ACTION, payload, rpc_frame, response, std::chrono::milliseconds(0)));
        auto                          end  = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = end - start;
        CHECK_LT(diff, std::chrono::milliseconds(5u));
        CHECK(websocket.sendCalled());

        start = std::chrono::steady_clock::now();
        CHECK_FALSE(client.call(ACTION, payload, rpc_frame, response, std::chrono::milliseconds(100)));
        end  = std::chrono::steady_clock::now();
        diff = end - start;
        CHECK_GT(diff, std::chrono::milliseconds(99u));
        CHECK(websocket.sendCalled());

        std::thread response_thread(
            [&websocket]
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100u));
                websocket.notifyDataReceived(EXPECTED_CALLRESULT_MESSAGE_2, strlen(EXPECTED_CALLRESULT_MESSAGE_2));
            });
        CHECK_FALSE(client.call(ACTION, payload, rpc_frame, response, std::chrono::milliseconds(50)));
        response_thread.join();
    }

    TEST_CASE("Reception of call request")
    {
        RpcClientListener             listener;
        WebsocketClientStub           websocket;
        IWebsocketClient::Credentials credentials;
        RpcClient                     client(websocket, WS_PROTOCOL);
        client.registerListener(listener);
        client.registerClientListener(listener);
        client.start("", credentials);

        listener.response = CALLRESULT_PAYLOAD;
        websocket.notifyDataReceived(EXPECTED_CALL_MESSAGE_1, strlen(EXPECTED_CALL_MESSAGE_1));
        std::this_thread::sleep_for(std::chrono::milliseconds(50u));
        CHECK_EQ(listener.action, ACTION);
        CHECK_EQ(listener.payload, CALL_PAYLOAD);
        CHECK(websocket.sendCalled());
        CHECK_EQ(strcmp(reinterpret_cast<const char*>(websocket.sentData()), EXPECTED_CALLRESULT_MESSAGE_1), 0);
    }

    TEST_CASE("Error generation on reception of a call request")
    {
        RpcClientListener             listener;
        WebsocketClientStub           websocket;
        IWebsocketClient::Credentials credentials;
        RpcClient                     client(websocket, WS_PROTOCOL);
        client.registerListener(listener);
        client.registerClientListener(listener);
        client.start("", credentials);

        listener.response       = CALLRESULT_PAYLOAD;
        listener.received_error = true;
        listener.error_code     = IRpc::RPC_ERROR_NOT_IMPLEMENTED;
        listener.error_message  = CALLERROR_PAYLOAD;
        websocket.notifyDataReceived(EXPECTED_CALL_MESSAGE_1, strlen(EXPECTED_CALL_MESSAGE_1));
        std::this_thread::sleep_for(std::chrono::milliseconds(50u));
        CHECK_EQ(listener.action, ACTION);
        CHECK_EQ(listener.payload, CALL_PAYLOAD);
        CHECK(websocket.sendCalled());
        CHECK_EQ(strcmp(reinterpret_cast<const char*>(websocket.sentData()), EXPECTED_CALLERROR_MESSAGE_1), 0);
    }
}
