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

#include "ChargePointDemoConfig.h"
#include "ChargePointEventsHandler.h"
#include "IChargePoint.h"
#include "MainMeterSimulator.h"
#include "MeterSimulator.h"
#include "SetpointManager.h"
#include "String.h"

#include <cstring>
#include <filesystem>
#include <iostream>
#include <thread>

using namespace ocpp::chargepoint;
using namespace ocpp::types;

/** @brief Entry point */
int main(int argc, char* argv[])
{
    // Default parameters
    unsigned int charge_point_max_current = 32u;
    unsigned int connector_max_current    = 32u;
    std::string  working_dir              = "";
    bool         reset_all                = false;
    bool         reset_connectors         = false;

    // Check parameters
    if (argc > 1)
    {
        const char* param     = nullptr;
        bool        bad_param = false;
        argv++;
        while ((argc != 1) && !bad_param)
        {
            if (strcmp(*argv, "-h") == 0)
            {
                bad_param = true;
            }
            else if ((strcmp(*argv, "-m") == 0) && (argc > 1))
            {
                argv++;
                argc--;
                charge_point_max_current = static_cast<unsigned int>(std::atoi(*argv));
            }
            else if ((strcmp(*argv, "-c") == 0) && (argc > 1))
            {
                argv++;
                argc--;
                connector_max_current = static_cast<unsigned int>(std::atoi(*argv));
            }
            else if ((strcmp(*argv, "-w") == 0) && (argc > 1))
            {
                argv++;
                argc--;
                working_dir = *argv;
            }
            else if (strcmp(*argv, "-r") == 0)
            {
                reset_all = true;
            }
            else if (strcmp(*argv, "-d") == 0)
            {
                reset_connectors = true;
            }
            else
            {
                param     = *argv;
                bad_param = true;
            }

            // Next param
            argc--;
            argv++;
        }
        if (bad_param)
        {
            if (param)
            {
                std::cout << "Invalid parameter : " << param << std::endl;
            }
            std::cout << "Usage : remote_chargepoint [-m charge_point_max_current] [-c connector_max_current] [-w working_dir] [-r] [-d]"
                      << std::endl;
            std::cout << "    -m : Maximum current in A for the whole Charge Point (Default = 32A)" << std::endl;
            std::cout << "    -c : Maximum current in A for a connector (Default = 32A)" << std::endl;
            std::cout << "    -w : Working directory where to store the configuration file (Default = current directory)" << std::endl;
            std::cout << "    -r : Reset all the OCPP persistent data" << std::endl;
            std::cout << "    -d : Reset all the connector persistent data" << std::endl;
            return 1;
        }
    }

    std::cout << "Starting charge point with :" << std::endl;
    std::cout << "  - charge_point_max_current = " << charge_point_max_current << "A" << std::endl;
    std::cout << "  - connector_max_current = " << connector_max_current << "A" << std::endl;
    std::cout << "  - working_dir = " << working_dir << std::endl;

    // Configuration
    std::filesystem::path path(working_dir);
    path /= "remote_chargepoint.ini";
    ChargePointDemoConfig config(path);

    // // Event handler
    ChargePointEventsHandler event_handler(config, working_dir);

    // Instanciate charge point
    std::unique_ptr<IChargePoint> charge_point = IChargePoint::create(config.stackConfig(), config.ocppConfig(), event_handler);
    if (reset_connectors)
    {
        charge_point->resetConnectorData();
    }
    if (reset_all)
    {
        charge_point->resetData();
    }
    event_handler.setChargePoint(*charge_point.get());
    charge_point->start();

    // From now on the stack is alive :)

    // Meter simulators
    std::vector<std::unique_ptr<IMeter>> meter_simulators;
    meter_simulators.push_back(nullptr);
    for (size_t i = 0; i < config.ocppConfig().numberOfConnectors(); i++)
    {
        meter_simulators.emplace_back(new MeterSimulator(charge_point->getTimerPool(), 3u));
        meter_simulators[meter_simulators.size() - 1u]->setVoltages({230u, 230u, 230u});
    }
    std::vector<IMeter*> child_meters;
    for (size_t i = 1; i < meter_simulators.size(); i++)
    {
        child_meters.push_back(meter_simulators[i].get());
    }
    meter_simulators[0] = std::unique_ptr<IMeter>(new MainMeterSimulator(child_meters));
    event_handler.setMeterSimulators(meter_simulators);

    // Setpoint manager
    SetpointManager setpoint_manager(
        *(charge_point.get()), config.ocppConfig().numberOfConnectors(), charge_point_max_current, connector_max_current);
    event_handler.setSetpointManager(setpoint_manager);

    // App loop
    while (true)
    {
        // Wait to be accepted by Central System
        while (charge_point->getRegistrationStatus() != RegistrationStatus::Accepted)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100u));
        }

        // Test loop
        while (true)
        {
            // Check connector state
            unsigned int connector_id;
            std::string  id_tag;
            bool         restore = false;
            for (unsigned int i = 1; i <= config.ocppConfig().numberOfConnectors(); i++)
            {
                if ((charge_point->getConnectorStatus(i) == ChargePointStatus::Charging) ||
                    (charge_point->getConnectorStatus(i) == ChargePointStatus::SuspendedEV) ||
                    (charge_point->getConnectorStatus(i) == ChargePointStatus::SuspendedEVSE))
                {
                    connector_id = i;
                    restore      = true;
                    break;
                }
            }
            if (!restore)
            {
                // Wait for remote start transaction
                bool pending = false;
                while (!pending)
                {
                    for (unsigned int i = 1; i <= config.ocppConfig().numberOfConnectors(); i++)
                    {
                        pending = event_handler.isRemoteStartPending(i);
                        if (pending)
                        {
                            connector_id = i;
                            break;
                        }
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(100u));
                }
                id_tag = event_handler.remoteStartIdTag(connector_id);
                event_handler.resetRemoteStartPending(connector_id);

                // Notify preparing
                charge_point->statusNotification(connector_id, ChargePointStatus::Preparing);
            }

            // Authorize
            std::string parent_id;
            if (restore || !config.ocppConfig().authorizeRemoteTxRequests() ||
                (charge_point->authorize(connector_id, id_tag, parent_id) == AuthorizationStatus::Accepted))
            {
                // Start transaction
                if (restore || charge_point->startTransaction(connector_id, id_tag) == AuthorizationStatus::Accepted)
                {
                    // Notify suspent
                    if (!restore)
                    {
                        charge_point->statusNotification(connector_id, ChargePointStatus::SuspendedEVSE);
                    }

                    // Start meter
                    meter_simulators[connector_id]->setCurrents({0u, 0u, 0u});
                    meter_simulators[connector_id]->start();

                    // Wait for remote stop transaction or end of transaction
                    auto start   = std::chrono::steady_clock::now();
                    bool pending = false;
                    while (!pending && ((std::chrono::steady_clock::now() - start) < std::chrono::seconds(60)))
                    {
                        for (unsigned int i = 1; i <= config.ocppConfig().numberOfConnectors(); i++)
                        {
                            pending = event_handler.isRemoteStopPending(i);
                            if (pending)
                            {
                                break;
                            }
                        }
                        {
                            auto         setpoints = setpoint_manager.getSetpoints();
                            unsigned int setpoint  = static_cast<unsigned int>(setpoints[connector_id]);
                            meter_simulators[connector_id]->setCurrents({setpoint, setpoint, setpoint});
                            if ((setpoint == 0) && (charge_point->getConnectorStatus(connector_id) != ChargePointStatus::SuspendedEVSE))
                            {
                                // Notify suspent
                                charge_point->statusNotification(connector_id, ChargePointStatus::SuspendedEVSE);
                            }
                            if ((setpoint != 0) && (charge_point->getConnectorStatus(connector_id) != ChargePointStatus::Charging))
                            {
                                // Notify charging
                                charge_point->statusNotification(connector_id, ChargePointStatus::Charging);
                            }
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(100u));
                    }
                    Reason reason = Reason::Local;
                    if (event_handler.isRemoteStopPending(connector_id))
                    {
                        id_tag = "";
                        reason = Reason::Remote;
                        event_handler.resetRemoteStopPending(connector_id);
                    }

                    // Stop meter
                    meter_simulators[connector_id]->stop();
                    meter_simulators[connector_id]->setCurrents({0u, 0u, 0u});

                    // Stop transaction
                    charge_point->stopTransaction(connector_id, id_tag, reason);

                    // Notify finishing
                    charge_point->statusNotification(connector_id, ChargePointStatus::Finishing);
                }
            }

            // Notify available
            charge_point->statusNotification(connector_id, ChargePointStatus::Available);
        }
    }

    return 0;
}
