/*
MIT License

Copyright (c) 2020 Cedric Jimenez

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sel
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

#include "CentralSystemDemoConfig.h"
#include "CentralSystemEventsHandler.h"
#include "ChargePointDatabase.h"
#include "Database.h"
#include "ICentralSystem.h"

#include <cstring>
#include <filesystem>
#include <iostream>
#include <thread>

using namespace ocpp::centralsystem;
using namespace ocpp::types;

/** @brief Indicate if a change of configuration has been accepted by the charge point */
static bool isConfigurationChangeAccepted(ConfigurationStatus status);

/** @brief Entry point */
int main(int argc, char* argv[])
{
    // Default parameters
    std::string working_dir = "";
    bool        reset_all   = false;

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
            std::cout << "Usage : security_centralsystem [-w working_dir] [-r]" << std::endl;
            std::cout
                << "    -w : Working directory where to store the configuration file and the local database (Default = current directory)"
                << std::endl;
            std::cout << "    -r : Reset all the OCPP persistent data" << std::endl;
            return 1;
        }
    }

    std::cout << "Starting central system with :" << std::endl;
    std::cout << "  - working_dir = " << working_dir << std::endl;

    // Database for persistency
    static constexpr const char* DATABASE_NAME = "security_centralsystem.db";
    if (reset_all)
    {
        std::filesystem::remove(DATABASE_NAME);
    }
    ocpp::database::Database database;
    if (!database.open(DATABASE_NAME))
    {
        std::cout << "Error while opening the database : " << database.lastError() << std::endl;
    }
    ChargePointDatabase chargepoint_db(database);

    // Configuration
    std::filesystem::path   path(working_dir);
    CentralSystemDemoConfig config_p0(path / "security_centralsystem_p0.ini");
    CentralSystemDemoConfig config_p1(path / "security_centralsystem_p1.ini");
    CentralSystemDemoConfig config_p2(path / "security_centralsystem_p2.ini");
    CentralSystemDemoConfig config_p3(path / "security_centralsystem_p3.ini");

    // Event handler
    CentralSystemEventsHandler event_handler(chargepoint_db);

    // Instanciate 1 central system per security profile has required by the specification
    std::unique_ptr<ICentralSystem> central_system_p0 = ICentralSystem::create(config_p0.stackConfig(), event_handler);
    std::unique_ptr<ICentralSystem> central_system_p1 = ICentralSystem::create(config_p1.stackConfig(), event_handler);
    std::unique_ptr<ICentralSystem> central_system_p2 = ICentralSystem::create(config_p2.stackConfig(), event_handler);
    std::unique_ptr<ICentralSystem> central_system_p3 = ICentralSystem::create(config_p3.stackConfig(), event_handler);
    if (reset_all)
    {
        central_system_p0->resetData();
        central_system_p1->resetData();
        central_system_p2->resetData();
        central_system_p3->resetData();
    }
    std::vector<ICentralSystem*> central_systems = {
        central_system_p0.get(), central_system_p1.get(), central_system_p2.get(), central_system_p3.get()};
    event_handler.setCentralSystems(central_systems);

    central_system_p0->start();
    central_system_p1->start();
    central_system_p2->start();
    central_system_p3->start();

    // From now on the stack is alive :)

    // App loop
    while (true)
    {
        // Wait for a charge point to be connected
        std::shared_ptr<CentralSystemEventsHandler::ChargePointRequestHandler> chargepoint_handler = event_handler.waitForChargePoint();
        std::shared_ptr<ICentralSystem::IChargePoint>                          chargepoint         = chargepoint_handler->proxy();
        std::string                                                            chargepoint_id      = chargepoint->identifier();
        unsigned int chargepoint_profile = event_handler.getCentralSystemSecurityProfile(chargepoint);
        switch (chargepoint_profile)
        {
            case 0:
            {
                // Configure for security profile 1 : HTTP Basic Authentication
                std::cout << "[" << chargepoint_id << "] - Configuring security profile 1" << std::endl;

                // Configure AuthorizationKey
                ConfigurationStatus configure_status =
                    chargepoint->changeConfiguration("AuthorizationKey", chargepoint_handler->authentKey());
                if (isConfigurationChangeAccepted(configure_status))
                {
                    // Configure new connection URL => Non OCPP standard but necessary to automatize the process
                    configure_status = chargepoint->changeConfiguration("ConnexionUrl", config_p1.stackConfig().listenUrl());
                    if (isConfigurationChangeAccepted(configure_status))
                    {
                        // Configure new security profile
                        configure_status = chargepoint->changeConfiguration("SecurityProfile", "1");
                        if (isConfigurationChangeAccepted(configure_status))
                        {
                            // Update security profile in database
                            chargepoint_db.setChargePointProfile(chargepoint_id, 1u);
                        }
                        else
                        {
                            std::cout << "[" << chargepoint_id << "] - Unable to configure SecurityProfile" << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "[" << chargepoint_id << "] - Unable to configure ConnexionUrl" << std::endl;
                    }
                }
                else
                {
                    std::cout << "[" << chargepoint_id << "] - Unable to configure AuthorizationKey" << std::endl;
                }
            }
            break;

            default:
            {
                std::cout << "[" << chargepoint_id << "] - Unknown security profile : " << chargepoint_profile << std::endl;
            }
            break;
        }
    }

    return 0;
}

/** @brief Indicate if a change of configuration has been accepted by the charge point */
static bool isConfigurationChangeAccepted(ConfigurationStatus status)
{
    return ((status == ConfigurationStatus::Accepted) || (status == ConfigurationStatus::RebootRequired));
}