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

#include "DefaultLocalControllerEventsHandler.h"
#include "ILocalController.h"
#include "LocalControllerDemoConfig.h"

#include <cstring>
#include <filesystem>
#include <iostream>
#include <thread>

using namespace ocpp::localcontroller;
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
            std::cout << "Usage : quick_start_localcontroller [-w working_dir] [-r]" << std::endl;
            std::cout << "    -w : Working directory where to store the configuration file (Default = current directory)" << std::endl;
            std::cout << "    -r : Reset all the OCPP persistent data" << std::endl;
            return 1;
        }
    }

    std::cout << "Starting local controller with :" << std::endl;
    std::cout << "  - working_dir = " << working_dir << std::endl;

    // Configuration
    std::filesystem::path path(working_dir);
    path /= "quick_start_localcontroller.ini";
    LocalControllerDemoConfig config(path);

    // Event handler
    DefaultLocalControllerEventsHandler event_handler(config.stackConfig());

    // Instanciate local controller
    std::unique_ptr<ILocalController> local_controller = ILocalController::create(config.stackConfig(), event_handler);
    if (reset_all)
    {
        local_controller->resetData();
    }
    local_controller->start();

    // From now on the stack is alive :)

    // App loop
    while (true)
    {
        // Wait for at least 1 connected charge point
        while (event_handler.chargePoints().size() == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // For each connected charge point
        for (auto& iter_chargepoint : event_handler.chargePoints())
        {
            {
                auto chargepoint = iter_chargepoint.second->proxy();

                std::cout << "---------------------------------------------" << std::endl;
                std::cout << "Charge point : " << chargepoint->identifier() << std::endl;
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

                std::cout << "Configure heartbeat interval..." << std::endl;
                ChangeConfigurationReq conf_req;
                conf_req.key.assign("HeartbeatInterval");
                conf_req.value.assign("10");
                ChangeConfigurationConf conf_resp;
                if (chargepoint->call(conf_req, conf_resp, error, message))
                {
                    std::cout << ConfigurationStatusHelper.toString(conf_resp.status) << std::endl;
                }
                else
                {
                    std::cout << "Failed!" << std::endl;
                }

                std::cout << "Trigger status notification..." << std::endl;
                TriggerMessageReq trigger_req;
                trigger_req.requestedMessage = MessageTrigger::StatusNotification;
                TriggerMessageConf trigger_conf;
                if (chargepoint->call(trigger_req, trigger_conf, error, message))
                {
                    std::cout << TriggerMessageStatusHelper.toString(trigger_conf.status) << std::endl;
                }
                else
                {
                    std::cout << "Failed!" << std::endl;
                }

                std::cout << "Trigger meter values on connector 0..." << std::endl;
                trigger_req.connectorId.value() = 0;
                trigger_req.requestedMessage    = MessageTrigger::MeterValues;
                if (chargepoint->call(trigger_req, trigger_conf, error, message))
                {
                    std::cout << TriggerMessageStatusHelper.toString(trigger_conf.status) << std::endl;
                }
                else
                {
                    std::cout << "Failed!" << std::endl;
                }

                std::cout << "Trigger heartbeat..." << std::endl;
                trigger_req.connectorId.clear();
                trigger_req.requestedMessage = MessageTrigger::Heartbeat;
                if (chargepoint->call(trigger_req, trigger_conf, error, message))
                {
                    std::cout << TriggerMessageStatusHelper.toString(trigger_conf.status) << std::endl;
                }
                else
                {
                    std::cout << "Failed!" << std::endl;
                }
            }

            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }

    return 0;
}
