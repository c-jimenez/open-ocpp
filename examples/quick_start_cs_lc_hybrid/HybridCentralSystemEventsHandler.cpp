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

#include "HybridCentralSystemEventsHandler.h"
#include "StringHelpers.h"

#include <iostream>
#include <thread>

using namespace std;

/** @brief Constructor */
HybridCentralSystemEventsHandler::HybridCentralSystemEventsHandler(LocalControllerConfig& config,
                                                                   std::filesystem::path  iso_v2g_root_ca,
                                                                   std::filesystem::path  iso_mo_root_ca,
                                                                   bool                   set_pending_status)
    : DefaultCentralSystemEventsHandler(iso_v2g_root_ca, iso_mo_root_ca, set_pending_status), m_config(config)
{
}

/** @brief Destructor */
HybridCentralSystemEventsHandler::~HybridCentralSystemEventsHandler() { }

// ICentralSystemEventsHandler interface

/** @copydoc bool ICentralSystemEventsHandler::chargePointConnected(std::shared_ptr<ICentralSystem::IChargePoint>) */
void HybridCentralSystemEventsHandler::chargePointConnected(std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint> chargepoint)
{
    cout << "Charge point [" << chargepoint->identifier() << "] connected" << endl;
    auto iter_chargepoint = m_chargepoints.find(chargepoint->identifier());
    if (iter_chargepoint == m_chargepoints.end())
    {
        auto iter_forwarded = m_fowarded_chargepoints.find(chargepoint->identifier());
        if (iter_forwarded == m_fowarded_chargepoints.end())
        {
            // Charge point ended with "lc" are forwared to Central System
            if (ocpp::helpers::endsWith(chargepoint->identifier(), "lc"))
            {
                // Create Local Controller proxy
                auto proxy = ocpp::localcontroller::IChargePointProxy::createFrom(chargepoint, m_config);

                // Open connection to the Central System
                ocpp::websockets::IWebsocketClient::Credentials credentials;
                credentials.accept_untrusted_certificates             = false;
                credentials.allow_expired_certificates                = false;
                credentials.allow_selfsigned_certificates             = false;
                credentials.skip_server_name_check                    = false;
                credentials.encoded_pem_certificates                  = false;
                credentials.tls12_cipher_list                         = m_config.tlsv12CipherList();
                credentials.tls13_cipher_list                         = m_config.tlsv13CipherList();
                credentials.server_certificate_ca                     = m_config.tlsServerCertificateCa();
                credentials.client_certificate                        = m_config.tlsClientCertificate();
                credentials.client_certificate_private_key            = m_config.tlsClientCertificatePrivateKey();
                credentials.client_certificate_private_key_passphrase = m_config.tlsClientCertificatePrivateKeyPassphrase();
                if (!proxy->centralSystemProxy()->connect(m_config.connexionUrl(), credentials))
                {
                    cout << "Forwarded  Charge point [" << proxy->identifier() << "] unable to start connection to Central System" << endl;
                    proxy.reset();
                }
                else
                {
                    m_fowarded_chargepoints[proxy->identifier()] =
                        std::make_shared<HybridCentralSystemEventsHandler::LocalControllerProxyEventsHandler>(*this, proxy);
                }
            }
            else
            {
                m_chargepoints[chargepoint->identifier()] =
                    std::shared_ptr<ChargePointRequestHandler>(new ChargePointRequestHandler(*this, chargepoint));
            }
        }
        else
        {
            cout << "Charge point [" << chargepoint->identifier() << "] already forwarded" << endl;
            chargepoint.reset();
        }
    }
    else
    {
        cout << "Charge point [" << chargepoint->identifier() << "] already connected" << endl;
        chargepoint.reset();
    }
}

/** @brief Remove a charge point from the forwarded charge points */
void HybridCentralSystemEventsHandler::removeForwardedChargePoint(const std::string& identifier)
{
    std::thread t(
        [this, identifier = identifier]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            m_fowarded_chargepoints.erase(identifier);
        });
    t.detach();
}

/** @brief Constructor */
HybridCentralSystemEventsHandler::LocalControllerProxyEventsHandler::LocalControllerProxyEventsHandler(
    HybridCentralSystemEventsHandler& event_handler, std::shared_ptr<ocpp::localcontroller::IChargePointProxy>& chargepoint)
    : m_event_handler(event_handler), m_chargepoint(chargepoint)
{
    m_chargepoint->registerListener(*this);
}

/** @brief Destructor */
HybridCentralSystemEventsHandler::LocalControllerProxyEventsHandler::~LocalControllerProxyEventsHandler() { }

/** @brief Called to notify the disconnection of the charge point */
void HybridCentralSystemEventsHandler::LocalControllerProxyEventsHandler::disconnectedFromChargePoint()
{
    cout << "Forwarded Charge Point [" << m_chargepoint->identifier() << "] disconnected!" << endl;
    m_event_handler.removeForwardedChargePoint(m_chargepoint->identifier());
}

/** @brief Called to notify the connection to the central system */
void HybridCentralSystemEventsHandler::LocalControllerProxyEventsHandler::connectedToCentralSystem()
{
    cout << "Forwarded Charge Point [" << m_chargepoint->identifier() << "] connected to Central System!" << endl;
}

/** @brief Called to notify the failure of the connection to the central system */
void HybridCentralSystemEventsHandler::LocalControllerProxyEventsHandler::failedToConnectToCentralSystem()

{
    cout << "Forwarded Charge Point [" << m_chargepoint->identifier() << "] failed to connect to Central System!" << endl;
}

/** @brief Called to notify the disconnection from the central system */
void HybridCentralSystemEventsHandler::LocalControllerProxyEventsHandler::disconnectedFromCentralSystem()
{
    cout << "Forwarded Charge Point [" << m_chargepoint->identifier() << "] disconnected from Central System!" << endl;
}
