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

#ifndef HYBRIDCENTRALSYSTEMEVENTSHANDLER_H
#define HYBRIDCENTRALSYSTEMEVENTSHANDLER_H

#include "DefaultCentralSystemEventsHandler.h"
#include "IChargePointProxy.h"
#include "LocalControllerConfig.h"
#include "RpcPool.h"

/** @brief Hybrid central system event handlers implementation for the examples */
class HybridCentralSystemEventsHandler : public DefaultCentralSystemEventsHandler
{
  public:
    /** @brief Constructor */
    HybridCentralSystemEventsHandler(LocalControllerConfig& config,
                                     std::filesystem::path  iso_v2g_root_ca    = "",
                                     std::filesystem::path  iso_mo_root_ca     = "",
                                     bool                   set_pending_status = false);

    /** @brief Destructor */
    virtual ~HybridCentralSystemEventsHandler();

    // ICentralSystemEventsHandler interface

    /** @copydoc bool ICentralSystemEventsHandler::chargePointConnected(std::shared_ptr<ICentralSystem::IChargePoint>) */
    void chargePointConnected(std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint> chargepoint) override;

    /** @brief Handle events from local controller proxys */
    class LocalControllerProxyEventsHandler : public ocpp::localcontroller::ILocalControllerProxyEventsHandler
    {
      public:
        /** @brief Constructor */
        LocalControllerProxyEventsHandler(HybridCentralSystemEventsHandler&                          event_handler,
                                          std::shared_ptr<ocpp::localcontroller::IChargePointProxy>& chargepoint);

        /** @brief Destructor */
        virtual ~LocalControllerProxyEventsHandler();

        /** @brief Get the charge point proxy */
        std::shared_ptr<ocpp::localcontroller::IChargePointProxy> proxy() { return m_chargepoint; }

        // ocpp::localcontroller::ILocalControllerProxyEventsHandler interface

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
        HybridCentralSystemEventsHandler& m_event_handler;

        /** @brief Charge point proxy */
        std::shared_ptr<ocpp::localcontroller::IChargePointProxy> m_chargepoint;
    };

    /** @brief Get the list of the forwarded charge points */
    std::map<std::string, std::shared_ptr<LocalControllerProxyEventsHandler>>& forwardedChargePoints() { return m_fowarded_chargepoints; }

    /** @brief Remove a charge point from the forwarded charge points */
    void removeForwardedChargePoint(const std::string& identifier);

  private:
    /** @brief Configuration */
    LocalControllerConfig& m_config;
    /** @brief RPC pool*/
    ocpp::rpc::RpcPool m_rpc_pool;
    /** @brief Forwared charge points */
    std::map<std::string, std::shared_ptr<LocalControllerProxyEventsHandler>> m_fowarded_chargepoints;
};

#endif // HYBRIDCENTRALSYSTEMEVENTSHANDLER_H
