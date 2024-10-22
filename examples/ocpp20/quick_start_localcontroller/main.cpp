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
#include "ILocalController20.h"
#include "LocalControllerDemoConfig.h"
#include "WebsocketFactory.h"

#include <cstring>
#include <filesystem>
#include <iostream>
#include <thread>

using namespace ocpp::localcontroller::ocpp20;
using namespace ocpp::messages;
using namespace ocpp::messages::ocpp20;
using namespace ocpp::types;
using namespace ocpp::types::ocpp20;

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
    path /= "quick_start_localcontroller20.ini";
    LocalControllerDemoConfig config(path.string());

    // Event handler
    DefaultLocalControllerEventsHandler event_handler(config.stackConfig());

    // Configure websocket pools => mandatory for local controller
    ocpp::websockets::WebsocketFactory::setClientPoolCount(2u);
    ocpp::websockets::WebsocketFactory::startClientPools();

    // Instanciate local controller
    std::unique_ptr<ILocalController20> local_controller = ILocalController20::create(config.stackConfig(), event_handler);
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
                std::string error;
                std::string error_msg;
                auto        chargepoint = iter_chargepoint.second->proxy();

                std::cout << "---------------------------------------------" << std::endl;
                std::cout << "Charge point : " << chargepoint->identifier() << std::endl;
                std::cout << "---------------------------------------------" << std::endl;

                std::cout << "Read whole charge point configuration..." << std::endl;
                GetVariables20Req  get_vars_req;
                GetVariables20Conf get_vars_conf;
                if (chargepoint->call(get_vars_req, get_vars_conf, error, error_msg))
                {
                    std::cout << "Configuration keys :" << std::endl;
                    for (const GetVariableResultType20& var : get_vars_conf.getVariableResult)
                    {
                        std::cout << " - " << var.variable.name.str() << " : "
                                  << "component = " << var.component.name.str()
                                  << " value = " << (var.attributeValue.isSet() ? var.attributeValue.value().str() : "");
                    }
                }
                else
                {
                    std::cout << "Failed : error = " << error << " error_msg = " << error_msg << std::endl;
                }

                std::cout << "Configure heartbeat interval..." << std::endl;
                SetVariables20Req     set_vars_req;
                SetVariables20Conf    set_vars_conf;
                SetVariableDataType20 var;
                var.variable.name.assign("HeartbeatInterval");
                var.component.name.assign("OCPPCommCtrlr");
                var.attributeValue.assign("10");
                set_vars_req.setVariableData.push_back(std::move(var));
                if (chargepoint->call(set_vars_req, set_vars_conf, error, error_msg))
                {
                    std::cout << "Done!" << std::endl;
                }
                else
                {
                    std::cout << "Failed : error = " << error << " error_msg = " << error_msg << std::endl;
                }

                std::cout << "Trigger status notification..." << std::endl;
                TriggerMessage20Req  trigger_msg_req;
                TriggerMessage20Conf trigger_msg_conf;
                trigger_msg_req.requestedMessage = MessageTriggerEnumType20::StatusNotification;
                if (chargepoint->call(trigger_msg_req, trigger_msg_conf, error, error_msg))
                {
                    std::cout << "Done!" << std::endl;
                }
                else
                {
                    std::cout << "Failed : error = " << error << " error_msg = " << error_msg << std::endl;
                }

                std::cout << "Trigger meter values on evse 1..." << std::endl;
                trigger_msg_req.evse.value().id  = 1;
                trigger_msg_req.requestedMessage = MessageTriggerEnumType20::MeterValues;
                if (chargepoint->call(trigger_msg_req, trigger_msg_conf, error, error_msg))
                {
                    std::cout << "Done!" << std::endl;
                }
                else
                {
                    std::cout << "Failed : error = " << error << " error_msg = " << error_msg << std::endl;
                }

                std::cout << "Trigger heartbeat..." << std::endl;
                trigger_msg_req.evse.clear();
                trigger_msg_req.requestedMessage = MessageTriggerEnumType20::Heartbeat;
                if (chargepoint->call(trigger_msg_req, trigger_msg_conf, error, error_msg))
                {
                    std::cout << "Done!" << std::endl;
                }
                else
                {
                    std::cout << "Failed : error = " << error << " error_msg = " << error_msg << std::endl;
                }
            }

            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }

    return 0;
}
