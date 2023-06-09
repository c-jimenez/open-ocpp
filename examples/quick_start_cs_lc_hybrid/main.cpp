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
#include "ICentralSystem.h"
#include "LocalControllerDemoConfig.h"
#include "WebsocketFactory.h"

#include <cstring>
#include <filesystem>
#include <iostream>
#include <thread>

using namespace ocpp::centralsystem;
using namespace ocpp::types;
using namespace ocpp::messages;

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
            std::cout << "Usage : quick_start_cs_lc_hybrid [-w working_dir] [-r]" << std::endl;
            std::cout << "    -w : Working directory where to store the configuration file (Default = current directory)" << std::endl;
            std::cout << "    -r : Reset all the OCPP persistent data" << std::endl;
            return 1;
        }
    }

    std::cout << "Starting local controller with :" << std::endl;
    std::cout << "  - working_dir = " << working_dir << std::endl;

    // Configuration
    std::filesystem::path path(working_dir);
    path /= "quick_start_cs_lc_hybrid.ini";
    LocalControllerDemoConfig config(path.string());

    // Configure websocket pools => mandatory for local controller
    ocpp::websockets::WebsocketFactory::setClientPoolCount(2u);
    ocpp::websockets::WebsocketFactory::startClientPools();

    // Event handler
    HybridCentralSystemEventsHandler event_handler(config.stackConfig());

    // Instanciate central system
    std::unique_ptr<ICentralSystem> central_system = ICentralSystem::create(config.stackConfig(), event_handler);
    if (reset_all)
    {
        central_system->resetData();
    }
    central_system->start();

    // From now on the stack is alive :)

    // App loop
    while (true)
    {
        // Wait for at least 1 connected charge point
        while ((event_handler.chargePoints().size() == 0) && (event_handler.forwardedChargePoints().size() == 0))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // For each connected charge point
        for (auto& iter_chargepoint : event_handler.chargePoints())
        {
            auto chargepoint = iter_chargepoint.second->proxy();

            std::cout << "---------------------------------------------" << std::endl;
            std::cout << "Charge point : " << chargepoint->identifier() << std::endl;
            std::cout << "---------------------------------------------" << std::endl;

            std::cout << "Read whole charge point configuration..." << std::endl;

            std::vector<std::string> keys;
            std::vector<KeyValue>    config_keys;
            std::vector<std::string> unknown_keys;
            if (chargepoint->getConfiguration(keys, config_keys, unknown_keys))
            {
                std::cout << "Configuration keys :" << std::endl;
                for (const KeyValue& key_value : config_keys)
                {
                    std::cout << " - " << key_value.key.str() << " = " << (key_value.value.isSet() ? key_value.value.value().str() : "")
                              << " " << (key_value.readonly ? "(read-only)" : "") << std::endl;
                }
            }
            else
            {
                std::cout << "Failed!" << std::endl;
            }
        }

        // For each forwarded charge point
        for (auto& iter_chargepoint : event_handler.forwardedChargePoints())
        {
            auto chargepoint = iter_chargepoint.second->proxy();

            std::cout << "---------------------------------------------" << std::endl;
            std::cout << "Forwarded charge point : " << chargepoint->identifier() << std::endl;
            std::cout << "---------------------------------------------" << std::endl;

            std::cout << "Read whole charge point configuration..." << std::endl;

            GetConfigurationReq  req;
            GetConfigurationConf resp;
            std::string          error;
            std::string          message;
            if (chargepoint->call(req, resp, error, message))
            {
                std::cout << "Configuration keys :" << std::endl;
                for (const KeyValue& key_value : resp.configurationKey.value())
                {
                    std::cout << " - " << key_value.key.str() << " = " << (key_value.value.isSet() ? key_value.value.value().str() : "")
                              << " " << (key_value.readonly ? "(read-only)" : "") << std::endl;
                }
            }
            else
            {
                std::cout << "Failed!" << std::endl;
            }
        }
    }

    return 0;
}
