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

#ifndef DEFAULTLOCALCONTROLLEREVENTSHANDLER_H
#define DEFAULTLOCALCONTROLLEREVENTSHANDLER_H

#include "ILocalControllerEventsHandler20.h"
#include "ILocalControllerProxyEventsHandler20.h"
#include "LocalControllerConfig.h"

#include <map>
#include <mutex>

/** @brief Default local controller event handlers implementation for the examples */
class DefaultLocalControllerEventsHandler : public ocpp::localcontroller::ocpp20::ILocalControllerEventsHandler20
{
  public:
    /** @brief Constructor */
    DefaultLocalControllerEventsHandler(LocalControllerConfig& config);

    /** @brief Destructor */
    virtual ~DefaultLocalControllerEventsHandler();

    // ILocalControllerEventsHandler interface

    /** @copydoc bool ILocalControllerEventsHandler::acceptConnection(const char*) */
    bool acceptConnection(const char* ip_address) override;

    /** @copydoc void ILocalControllerEventsHandler::clientFailedToConnect(const char*) */
    void clientFailedToConnect(const char* ip_address) override;

    /** @copydoc bool ILocalControllerEventsHandler::checkCredentials(const std::string&, const std::string&) */
    bool checkCredentials(const std::string& chargepoint_id, const std::string& password) override;

    /** @copydoc bool ILocalControllerEventsHandler::chargePointConnected(std::shared_ptr<IChargePointProxy> */
    void chargePointConnected(std::shared_ptr<ocpp::localcontroller::ocpp20::IChargePointProxy20> chargepoint) override;

    // API

    /** @brief Handle events from local controller proxys */
    class LocalControllerProxyEventsHandler : public ocpp::localcontroller::ocpp20::ILocalControllerProxyEventsHandler20
    {
      public:
        /** @brief Constructor */
        LocalControllerProxyEventsHandler(DefaultLocalControllerEventsHandler&                                 event_handler,
                                          std::shared_ptr<ocpp::localcontroller::ocpp20::IChargePointProxy20>& chargepoint);

        /** @brief Destructor */
        virtual ~LocalControllerProxyEventsHandler();

        /** @brief Get the charge point proxy */
        std::shared_ptr<ocpp::localcontroller::ocpp20::IChargePointProxy20> proxy() { return m_chargepoint; }

        // ocpp::localcontroller::ocpp20::ILocalControllerProxyEventsHandler20

        /** @brief Called to notify the disconnection of the charge point */
        void disconnectedFromChargePoint() override;

        /** @brief Called to notify the connection to the central system */
        void connectedToCentralSystem() override;

        /** @brief Called to notify the failure of the connection to the central system */
        void failedToConnectToCentralSystem() override;

        /** @brief Called to notify the disconnection from the central system */
        void disconnectedFromCentralSystem() override;

      private:
        /** @brief Event handler */
        DefaultLocalControllerEventsHandler& m_event_handler;

        /** @brief Charge point proxy */
        std::shared_ptr<ocpp::localcontroller::ocpp20::IChargePointProxy20> m_chargepoint;
    };

    /** @brief Get the list of the connected charge points */
    std::map<std::string, std::shared_ptr<DefaultLocalControllerEventsHandler::LocalControllerProxyEventsHandler>> chargePoints()
    {
        return m_chargepoints;
    }

    /** @brief Remove a charge point from the connected charge points */
    void removeChargePoint(const std::string& identifier);

  private:
    /** @brief Configuration */
    LocalControllerConfig& m_config;
    /** @brief Mutex to protect the list of the charge points */
    std::mutex m_mutex;
    /** @brief Connected charge points */
    std::map<std::string, std::shared_ptr<DefaultLocalControllerEventsHandler::LocalControllerProxyEventsHandler>> m_chargepoints;
};

#endif // DEFAULTLOCALCONTROLLEREVENTSHANDLER_H
