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

#ifndef CENTRALSYSTEMEVENTSHANDLER_H
#define CENTRALSYSTEMEVENTSHANDLER_H

#include "DefaultCentralSystemEventsHandler.h"
#include "ICentralSystem.h"
#include "Queue.h"

#include <map>
#include <mutex>
#include <vector>

class ChargePointDatabase;

/** @brief Default central system event handlers implementation for the examples */
class CentralSystemEventsHandler : public DefaultCentralSystemEventsHandler
{
  public:
    class ChargePointRequestHandler;

    /** @brief Constructor */
    CentralSystemEventsHandler(ChargePointDatabase& chargepoint_db);

    /** @brief Destructor */
    virtual ~CentralSystemEventsHandler();

    /** @brief Set the list of central systems */
    void setCentralSystems(const std::vector<ocpp::centralsystem::ICentralSystem*>& central_systems)
    {
        m_central_systems = central_systems;
    }

    /** @brief Wait for a charge point to be connected */
    std::shared_ptr<ChargePointRequestHandler> waitForChargePoint();

    /** @brief Notify the connection of a charge point */
    void notifyChargePointConnection(const std::string& identifier);

    /** @brief Remove a charge point from the connected charge points */
    void removeChargePoint(const std::string& identifier);

    /** @brief Get the security profile of the instance of the central system associated to a charge point */
    unsigned int getCentralSystemSecurityProfile(std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint>& chargepoint);

    // ICentralSystemEventsHandler interface

    /** @copydoc bool ICentralSystemEventsHandler::checkCredentials(const std::string&, const std::string&) */
    bool checkCredentials(const std::string& chargepoint_id, const std::string& password) override;

    /** @copydoc bool ICentralSystemEventsHandler::chargePointConnected(std::shared_ptr<ICentralSystem::IChargePoint>) */
    void chargePointConnected(std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint> chargepoint) override;

    // API

    /** @brief Handle requests/events from charge points */
    class ChargePointRequestHandler : public DefaultCentralSystemEventsHandler::ChargePointRequestHandler
    {
      public:
        /** @brief Constructor */
        ChargePointRequestHandler(CentralSystemEventsHandler&                                         event_handler,
                                  std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint>& chargepoint,
                                  ChargePointDatabase&                                                chargepoint_db,
                                  std::string&                                                        authent_key);

        /** @brief Destructor */
        virtual ~ChargePointRequestHandler();

        /** @brief et the authent key */
        const std::string& authentKey() const { return m_authent_key; }

        // IChargePointRequestHandler interface

        /** @copydoc void IChargePointRequestHandler::disconnected() */
        void disconnected() override;

        /** @copydoc ocpp::types::ocpp16::RegistrationStatus IChargePointRequestHandler::bootNotification(const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&) */
        ocpp::types::ocpp16::RegistrationStatus bootNotification(const std::string& model,
                                                                 const std::string& serial_number,
                                                                 const std::string& vendor,
                                                                 const std::string& firmware_version,
                                                                 const std::string& iccid,
                                                                 const std::string& imsi,
                                                                 const std::string& meter_serial_number,
                                                                 const std::string& meter_type) override;

        // Security extensions

      protected:
        /** @brief Get the serial number of the charge point */
        std::string getChargePointSerialNumber(const std::string& chargepoint_id) override;

      private:
        /** @brief Event handler */
        CentralSystemEventsHandler& m_event_handler;
        /** @brief Charge point database */
        ChargePointDatabase& m_chargepoint_db;
        /** @brief Authent key */
        std::string m_authent_key;
    };

  private:
    /** @brief Charge point database */
    ChargePointDatabase& m_chargepoint_db;
    /** @brief Central system instances */
    std::vector<ocpp::centralsystem::ICentralSystem*> m_central_systems;
    /** @brief Mutex to protect concurrent access to the list of connected charge points */
    std::mutex m_mutex;
    /** @brief Connected charge points */
    std::map<std::string, std::shared_ptr<ChargePointRequestHandler>> m_chargepoints;
    /** @brief Queue of connected charge points */
    ocpp::helpers::Queue<std::shared_ptr<ChargePointRequestHandler>> m_connected_queue;
};

#endif // CENTRALSYSTEMEVENTSHANDLER_H
