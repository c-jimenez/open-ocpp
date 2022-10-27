/*
MIT License

Copyright (c) 2020 Cedric Jimenez

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "DefaultLocalControllerEventsHandler.h"

#include <iostream>
#include <thread>

using namespace std;
using namespace ocpp::localcontroller;

/** @brief Constructor */
DefaultLocalControllerEventsHandler::DefaultLocalControllerEventsHandler() : m_mutex(), m_chargepoints() { }

/** @brief Destructor */
DefaultLocalControllerEventsHandler::~DefaultLocalControllerEventsHandler() { }

// ILocalControllerEventsHandler interface

/** @copydoc bool ILocalControllerEventsHandler::checkCredentials(const std::string&, const std::string&) */
bool DefaultLocalControllerEventsHandler::checkCredentials(const std::string& chargepoint_id, const std::string& password)
{
    cout << "Check credentials for [" << chargepoint_id << "] : " << password << endl;
    return true;
}

/** @copydoc bool ILocalControllerEventsHandler::chargePointConnected(std::shared_ptr<IChargePointProxy> */
void DefaultLocalControllerEventsHandler::chargePointConnected(std::shared_ptr<ocpp::localcontroller::IChargePointProxy> chargepoint)
{
    cout << "Charge point [" << chargepoint->identifier() << "] connected" << endl;
    m_mutex.lock();
    auto iter_chargepoint = m_chargepoints.find(chargepoint->identifier());
    if (iter_chargepoint == m_chargepoints.end())
    {
        m_chargepoints[chargepoint->identifier()] =
            std::make_shared<DefaultLocalControllerEventsHandler::LocalControllerProxyEventsHandler>(*this, chargepoint);
        m_mutex.unlock();
        ocpp::websockets::IWebsocketClient::Credentials credentials;
        credentials.accept_untrusted_certificates = true;
        credentials.allow_expired_certificates    = true;
        credentials.allow_selfsigned_certificates = true;
        credentials.skip_server_name_check        = true;
        chargepoint->centralSystemProxy()->connect("wss://127.0.0.1:8081/ocpp/", credentials);
    }
    else
    {
        m_mutex.unlock();
        cout << "Charge point [" << chargepoint->identifier() << "] already connected" << endl;
        chargepoint.reset();
    }
}

/** @brief Remove a charge point from the connected charge points */
void DefaultLocalControllerEventsHandler::removeChargePoint(const std::string& identifier)
{
    std::thread t(
        [this, &identifier]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            std::lock_guard<std::mutex> lock(m_mutex);
            auto                        iter_chargepoint = m_chargepoints.find(identifier);
            if (iter_chargepoint != m_chargepoints.end())
            {
                m_chargepoints.erase(iter_chargepoint);
            }
        });
    t.detach();
}

/** @brief Constructor */
DefaultLocalControllerEventsHandler::LocalControllerProxyEventsHandler::LocalControllerProxyEventsHandler(
    DefaultLocalControllerEventsHandler& event_handler, std::shared_ptr<ocpp::localcontroller::IChargePointProxy>& chargepoint)
    : m_event_handler(event_handler), m_chargepoint(chargepoint)
{
    m_chargepoint->registerListener(*this);
}

/** @brief Destructor */
DefaultLocalControllerEventsHandler::LocalControllerProxyEventsHandler::~LocalControllerProxyEventsHandler() { }

/** @brief Called to notify the disconnection of the charge point */
void DefaultLocalControllerEventsHandler::LocalControllerProxyEventsHandler::disconnectedFromChargePoint()
{
    cout << "Charge Point [" << m_chargepoint->identifier() << "] disconnected!" << endl;
    m_event_handler.removeChargePoint(m_chargepoint->identifier());
}

/** @brief Called to notify the connection to the central system */
void DefaultLocalControllerEventsHandler::LocalControllerProxyEventsHandler::connectedToCentralSystem()
{
    cout << "Charge Point [" << m_chargepoint->identifier() << "] connected to Central System!" << endl;
}

/** @brief Called to notify the failure of the connection to the central system */
void DefaultLocalControllerEventsHandler::LocalControllerProxyEventsHandler::failedToConnectToCentralSystem()

{
    cout << "Charge Point [" << m_chargepoint->identifier() << "] failed to connect to Central System!" << endl;
}

/** @brief Called to notify the disconnection from the central system */
void DefaultLocalControllerEventsHandler::LocalControllerProxyEventsHandler::disconnectedFromCentralSystem()
{
    cout << "Charge Point [" << m_chargepoint->identifier() << "] disconnected from Central System!" << endl;
}