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

#include "CentralSystemDemoConfig.h"
#include "ICentralSystem.h"

#include <cstring>
#include <filesystem>
#include <iostream>
#include <thread>

using namespace ocpp::centralsystem;

/** @brief EVent handler */
class CentralSystemEventHandler : public ICentralSystemEventsHandler
{
};

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
            std::cout << "Usage : quick_start_centralsystem [-w working_dir] [-r]" << std::endl;
            std::cout << "    -w : Working directory where to store the configuration file (Default = current directory)" << std::endl;
            std::cout << "    -r : Reset all the OCPP persistent data" << std::endl;
            return 1;
        }
    }

    std::cout << "Starting central system with :" << std::endl;
    std::cout << "  - working_dir = " << working_dir << std::endl;

    // Configuration
    std::filesystem::path path(working_dir);
    path /= "quick_start_centralsystem.ini";
    CentralSystemDemoConfig config(path);

    // Event handler
    CentralSystemEventHandler event_handler;

    // Instanciate central system
    std::unique_ptr<ICentralSystem> central_system = ICentralSystem::create(config.stackConfig(), event_handler);
    if (reset_all)
    {
        central_system->resetData();
    }
    central_system->start();

    // From now on the stack is alive :)

    std::this_thread::sleep_for(std::chrono::milliseconds(5000000));

    return 0;
}
