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

#include "CentralSystemEventsHandler.h"
#include "ChargePointDatabase.h"

#include <array>
#include <iostream>
#include <random>
#include <sstream>
#include <thread>

using namespace std;
using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

/** @brief Constructor */
CentralSystemEventsHandler::CentralSystemEventsHandler(ChargePointDatabase& chargepoint_db)
    : m_chargepoint_db(chargepoint_db), m_mutex(), m_chargepoints(), m_connected_queue()
{
}

/** @brief Destructor */
CentralSystemEventsHandler::~CentralSystemEventsHandler() { }

/** @brief Wait for a charge point to be connected */
std::shared_ptr<CentralSystemEventsHandler::ChargePointRequestHandler> CentralSystemEventsHandler::waitForChargePoint()
{
    std::shared_ptr<ChargePointRequestHandler> ret;
    m_connected_queue.pop(ret);
    return ret;
}

/** @brief Notify the connection of a charge point */
void CentralSystemEventsHandler::notifyChargePointConnection(const std::string& identifier)
{
    auto iter_chargepoint = m_chargepoints.find(identifier);
    if (iter_chargepoint != m_chargepoints.end())
    {
        m_connected_queue.push(iter_chargepoint->second);
    }
}

/** @brief Remove a charge point from the connected charge points */
void CentralSystemEventsHandler::removeChargePoint(const std::string& identifier)
{
    std::thread t(
        [this, &identifier]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            auto iter_chargepoint = m_chargepoints.find(identifier);
            if (iter_chargepoint != m_chargepoints.end())
            {
                m_chargepoints.erase(iter_chargepoint);
            }
        });
    t.detach();
}

/** @brief Get the security profile of the instance of the central system associated to a charge point */
unsigned int CentralSystemEventsHandler::getCentralSystemSecurityProfile(
    std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint>& chargepoint)
{
    unsigned int                         security_profile          = 0;
    ocpp::centralsystem::ICentralSystem* chargepoint_centralsystem = &chargepoint->centralSystem();
    for (security_profile = 0; security_profile < m_central_systems.size(); security_profile++)
    {
        if (chargepoint_centralsystem == m_central_systems[security_profile])
        {
            return security_profile;
        }
    }
    return 0;
}

// ICentralSystemEventsHandler interface

/** @copydoc bool ICentralSystemEventsHandler::checkCredentials(const std::string&, const std::string&) */
bool CentralSystemEventsHandler::checkCredentials(const std::string& chargepoint_id, const std::string& password)
{
    bool ret = false;

    std::string hex_encoded_password = ocpp::helpers::toHexString(password);
    cout << "Check credentials for [" << chargepoint_id << "] : " << hex_encoded_password << endl;

    // HTTP Basic Authentication is for Charge Points configured with Security Profile 1 or 2 only

    // Get the charge's point authentication key
    std::string  serial_number;
    unsigned int security_profile;
    std::string  authent_key;
    if (m_chargepoint_db.getChargePoint(chargepoint_id, serial_number, security_profile, authent_key))
    {
        if ((security_profile == 1u) || (security_profile == 2u))
        {
            ret = (hex_encoded_password == authent_key);
        }
        else
        {
            cout << "[" << chargepoint_id << "] - Invalid security profile" << endl;
        }
    }
    else
    {
        cout << "[" << chargepoint_id << "] - Unknown charge point" << endl;
    }

    return ret;
}

/** @copydoc bool ICentralSystemEventsHandler::chargePointConnected(std::shared_ptr<ICentralSystem::IChargePoint>) */
void CentralSystemEventsHandler::chargePointConnected(std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint> chargepoint)
{
    bool        disconnect_chargepoint = false;
    bool        known_chargepoint      = false;
    std::string authent_key;

    cout << "Charge point [" << chargepoint->identifier() << "] connected" << endl;

    // Check if the charge point is already connected
    std::lock_guard<std::mutex> lock(m_mutex);

    auto iter_chargepoint = m_chargepoints.find(chargepoint->identifier());
    if (iter_chargepoint == m_chargepoints.end())
    {
        // Check if the charge point is connecting using the expected security profile
        std::string  serial_number;
        unsigned int security_profile = 0;
        if (m_chargepoint_db.getChargePoint(chargepoint->identifier(), serial_number, security_profile, authent_key))
        {
            known_chargepoint = true;
        }
        if (security_profile != getCentralSystemSecurityProfile(chargepoint))
        {
            disconnect_chargepoint = true;
            cout << "[" << chargepoint->identifier() << "] - Invalid security profile" << endl;
        }
    }
    else
    {
        disconnect_chargepoint = true;
        cout << "Charge point [" << chargepoint->identifier() << "] already connected" << endl;
    }
    if (!disconnect_chargepoint)
    {
        // Add the charge point to the list of connected charge points
        m_chargepoints[chargepoint->identifier()] =
            std::shared_ptr<ChargePointRequestHandler>(new ChargePointRequestHandler(*this, chargepoint, m_chargepoint_db, authent_key));
        if (known_chargepoint)
        {
            // We already know this chargepoint, notify the connection
            // Unknown charge point connection will be notified after the boot notification

            std::thread t(
                [this, chargepoint]
                {
                    // Wait for an eventual boot notification
                    std::this_thread::sleep_for(std::chrono::seconds(1));

                    notifyChargePointConnection(chargepoint->identifier());
                });
            t.detach();
        }
        else
        {
            chargepoint.reset();
        }
    }
}

// ChargePointRequestHandler

/** @brief Constructor */
CentralSystemEventsHandler::ChargePointRequestHandler::ChargePointRequestHandler(
    CentralSystemEventsHandler&                                         event_handler,
    std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint>& chargepoint,
    ChargePointDatabase&                                                chargepoint_db,
    std::string&                                                        authent_key)
    : DefaultCentralSystemEventsHandler::ChargePointRequestHandler(event_handler, chargepoint),
      m_event_handler(event_handler),
      m_chargepoint_db(chargepoint_db),
      m_authent_key(authent_key)
{
}

/** @brief Destructor */
CentralSystemEventsHandler::ChargePointRequestHandler::~ChargePointRequestHandler() { }

/** @copydoc void IChargePointRequestHandler::disconnected() */
void CentralSystemEventsHandler::ChargePointRequestHandler::disconnected()
{
    cout << "Charge point [" << this->proxy()->identifier() << "] disconnected" << endl;
    m_event_handler.removeChargePoint(this->proxy()->identifier());
}

/** @copydoc ocpp::types::ocpp16::RegistrationStatus IChargePointRequestHandler::bootNotification(const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&) */
ocpp::types::ocpp16::RegistrationStatus CentralSystemEventsHandler::ChargePointRequestHandler::bootNotification(
    const std::string& model,
    const std::string& serial_number,
    const std::string& vendor,
    const std::string& firmware_version,
    const std::string& iccid,
    const std::string& imsi,
    const std::string& meter_serial_number,
    const std::string& meter_type)
{
    // Default status is pending during the whole security profile configuration
    RegistrationStatus ret = RegistrationStatus::Pending;

    // Check if the charge point is already known
    std::string  db_serial_number;
    unsigned int db_security_profile;
    std::string  db_authent_key;
    if (m_chargepoint_db.getChargePoint(this->proxy()->identifier(), db_serial_number, db_security_profile, db_authent_key))
    {
        // Check if the serial number matches
        if (serial_number != db_serial_number)
        {
            // Charge point identifier re-use => disconnect
            cout << "Charge point [" << this->proxy()->identifier() << "] - Invalid serial number" << endl;
            this->proxy()->disconnect();
            ret = RegistrationStatus::Rejected;
        }
        else
        {
            // If charge point is already at the max security profile, accept connexion
            if (db_security_profile == 3u)
            {
                ret = RegistrationStatus::Accepted;
            }
        }
    }
    else
    {
        // Generate an authent key for the charge point : minimal 8 bytes, max : 20 bytes
        std::mt19937                                rand_gen;
        std::uniform_int_distribution<unsigned int> rand_distrib;
        std::random_device                          rd;
        rand_gen.seed(rd());

        std::array<uint8_t, 17u> authent_key_bytes;
        for (auto& val : authent_key_bytes)
        {
            val = static_cast<uint8_t>(rand_distrib(rand_gen));
        }
        m_authent_key = ocpp::helpers::toHexString(authent_key_bytes);

        // Add the charge point to the database
        m_chargepoint_db.addChargePoint(this->proxy()->identifier(), serial_number, vendor, model, 0, m_authent_key);

        // Notify the connection
        m_event_handler.notifyChargePointConnection(this->proxy()->identifier());
    }

    (void)firmware_version;
    (void)iccid;
    (void)imsi;
    (void)meter_serial_number;
    (void)meter_type;

    return ret;
}

/** @brief Get the serial number of the charge point */
std::string CentralSystemEventsHandler::ChargePointRequestHandler::getChargePointSerialNumber(const std::string& chargepoint_id)
{
    std::string  db_serial_number;
    unsigned int db_security_profile;
    std::string  db_authent_key;
    m_chargepoint_db.getChargePoint(chargepoint_id, db_serial_number, db_security_profile, db_authent_key);
    return db_serial_number;
}
