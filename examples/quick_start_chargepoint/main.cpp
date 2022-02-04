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
#include "DefaultChargePointEventsHandler.h"
#include "IChargePoint.h"

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
    std::string id_tag           = "AABBCCDDEEFF";
    std::string working_dir      = "";
    bool        reset_all        = false;
    bool        reset_connectors = false;

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
            else if ((strcmp(*argv, "-t") == 0) && (argc > 1))
            {
                argv++;
                argc--;
                id_tag = *argv;
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
            std::cout << "Usage : quick_start_chargepoint [-t id_tag] [-w working_dir] [-r] [-d]" << std::endl;
            std::cout << "    -t : Id tag to use (Default = AABBCCDDEEFF)" << std::endl;
            std::cout << "    -w : Working directory where to store the configuration file (Default = current directory)" << std::endl;
            std::cout << "    -r : Reset all the OCPP persistent data" << std::endl;
            std::cout << "    -d : Reset all the connector persistent data" << std::endl;
            return 1;
        }
    }

    std::cout << "Starting charge point with :" << std::endl;
    std::cout << "  - id_tag = " << id_tag << std::endl;
    std::cout << "  - working_dir = " << working_dir << std::endl;

    // Configuration
    std::filesystem::path path(working_dir);
    path /= "quick_start_chargepoint.ini";
    ChargePointDemoConfig config(path);

    // Event handler
    DefaultChargePointEventsHandler event_handler(config, working_dir);

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
            // For each connector
            for (unsigned int connector_id = 1; connector_id <= config.ocppConfig().numberOfConnectors(); connector_id++)
            {
                // Ask for authorization on a tag
                std::string         parent_id;
                AuthorizationStatus status = charge_point->authorize(connector_id, id_tag, parent_id);
                if (status == AuthorizationStatus::Accepted)
                {
                    std::cout << "Id tag authorized, parent id = " << parent_id << std::endl;

                    // Preparing state
                    charge_point->statusNotification(connector_id, ChargePointStatus::Preparing);
                    std::this_thread::sleep_for(std::chrono::seconds(1u));

                    // Try to start charging session
                    status = charge_point->startTransaction(connector_id, id_tag);
                    if (status == AuthorizationStatus::Accepted)
                    {
                        std::cout << "Transaction authorized, start charging" << std::endl;

                        // Charging state
                        charge_point->statusNotification(connector_id, ChargePointStatus::Charging);
                        std::this_thread::sleep_for(std::chrono::seconds(30u));

                        // End transaction
                        charge_point->stopTransaction(connector_id, id_tag, Reason::Local);

                        // Finishing state
                        charge_point->statusNotification(connector_id, ChargePointStatus::Finishing);
                        std::this_thread::sleep_for(std::chrono::seconds(1u));
                    }
                    else
                    {
                        std::cout << "Transaction not authorized by Central System : " << AuthorizationStatusHelper.toString(status)
                                  << std::endl;
                    }

                    // Available state
                    charge_point->statusNotification(connector_id, ChargePointStatus::Available);
                }
                else
                {
                    std::cout << "Id tag not authorized by Central System : " << AuthorizationStatusHelper.toString(status) << std::endl;
                }

                // Wait before next charging session
                std::this_thread::sleep_for(std::chrono::seconds(10u));
            }
        }
    }

    return 0;
}
