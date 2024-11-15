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
#include "DefaultBasicChargePointEventsHandler.h"
#include "DeviceModelManager20.h"
#include "IBasicChargePoint20.h"

#include <chrono>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <thread>

using namespace ocpp::chargepoint::ocpp20;
using namespace ocpp::types;
using namespace ocpp::types::ocpp20;
// using namespace ocpp::messages;
// using namespace ocpp::messages::ocpp20;

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
            std::cout << "Usage : quick_start_chargepoint20 [-t id_tag] [-w working_dir] [-r] [-d]" << std::endl;
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
    path /= "quick_start_basicchargepoint20.ini";
    ChargePointDemoConfig config(path.string());

    // Device model
    std::filesystem::path device_model_path(working_dir);
    device_model_path /= "quick_start_basicchargepoint20.json";
    DeviceModelManager device_model_mgr(config.stackConfig());
    if (device_model_mgr.init())
    {
        if (!device_model_mgr.load(device_model_path))
        {
            std::cout << "Unable to load device model : " << device_model_mgr.lastError() << std::endl;
        }
    }
    else
    {
        std::cout << "Unable to initialize device model loader : " << device_model_mgr.lastError() << std::endl;
    }

    // Event handler
    DefaultBasicChargePointEventsHandler event_handler(config, working_dir);
    device_model_mgr.registerListener(event_handler);
    config.save();

    // Instanciate charge point
    std::unique_ptr<IBasicChargePoint20> charge_point = IBasicChargePoint20::create(config.stackConfig(), device_model_mgr, event_handler);
    if (reset_connectors)
    {
        charge_point->resetConnectorData();
    }
    if (reset_all)
    {
        charge_point->resetData();
    }
    //event_handler.setChargePoint(*charge_point.get());
    charge_point->start(BootReasonEnumType::PowerUp);

    // From now on the stack is alive :)

    // App loop
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        charge_point->statusNotification(1u, 1u, ConnectorStatusEnumType::Occupied);
    }

    return 0;
}
