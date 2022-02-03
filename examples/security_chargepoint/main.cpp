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

#include <algorithm>
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
            std::cout << "Usage : security_chargepoint [-t id_tag] [-w working_dir] [-r] [-d]" << std::endl;
            std::cout
                << "    -w : Working directory where to store the configuration file and the certificates (Default = current directory)"
                << std::endl;
            std::cout << "    -r : Reset all the OCPP persistent data" << std::endl;
            std::cout << "    -d : Reset all the connector persistent data" << std::endl;
            return 1;
        }
    }

    std::cout << "Starting charge point with :" << std::endl;
    std::cout << "  - working_dir = " << working_dir << std::endl;

    // Configuration
    std::filesystem::path path(working_dir);
    path /= "security_chargepoint.ini";
    ChargePointDemoConfig config(path);

    // Check if certificate management is handled by Open OCPP
    if (!config.stackConfig().internalCertificateManagementEnabled())
    {
        // Security Profile >= 2 : Look for all the Central System CA certificates installed and choose the most recent
        std::vector<std::pair<std::string, ocpp::x509::Certificate>> certs;
        if (config.ocppConfig().securityProfile() >= 2)
        {
            for (auto const& dir_entry : std::filesystem::directory_iterator{std::filesystem::current_path()})
            {
                if (!dir_entry.is_directory())
                {
                    std::string filename = dir_entry.path().filename();
                    if (ocpp::helpers::startsWith(filename, "cs_") && ocpp::helpers::endsWith(filename, ".pem"))
                    {
                        certs.emplace_back(dir_entry.path().string(), dir_entry.path());
                    }
                }
            }
            std::cout << certs.size() << " Central System certificates found" << std::endl;
            if (certs.size() != 0)
            {
                std::sort(
                    certs.begin(),
                    certs.end(),
                    [](const std::pair<std::string, ocpp::x509::Certificate>& a, const std::pair<std::string, ocpp::x509::Certificate>& b)
                    { return a.second.validityFrom() > b.second.validityFrom(); });
                config.setStackConfigValue("TlsServerCertificateCa", certs[0].first);

                std::cout << "Using : " << certs[0].first
                          << " - validity from : " << ocpp::types::DateTime(certs[0].second.validityFrom()).str() << std::endl;
            }
            else
            {
                std::cout << "Security Profile >= 2 without Central System certificate installed : the connection will likely failed"
                          << std::endl;
            }
        }

        // Security Profile = 3 : Look for all the Charge Point certificates installed and choose the most recent
        certs.clear();
        if (config.ocppConfig().securityProfile() == 3)
        {
            for (auto const& dir_entry : std::filesystem::directory_iterator{std::filesystem::current_path()})
            {
                if (!dir_entry.is_directory())
                {
                    std::string filename = dir_entry.path().filename();
                    if (ocpp::helpers::startsWith(filename, "cp_") && ocpp::helpers::endsWith(filename, ".pem"))
                    {
                        certs.emplace_back(dir_entry.path().string(), dir_entry.path());
                    }
                }
            }
            std::cout << certs.size() << " Charge Point certificate(s) found" << std::endl;
            if (certs.size() != 0)
            {
                std::sort(
                    certs.begin(),
                    certs.end(),
                    [](const std::pair<std::string, ocpp::x509::Certificate>& a, const std::pair<std::string, ocpp::x509::Certificate>& b)
                    { return a.second.validityFrom() > b.second.validityFrom(); });
                config.setStackConfigValue("TlsClientCertificate", certs[0].first);
                config.setStackConfigValue("TlsClientCertificatePrivateKey", certs[0].first + ".key");

                std::cout << "Using : " << certs[0].first
                          << " - validity from : " << ocpp::types::DateTime(certs[0].second.validityFrom()).str() << std::endl;
            }
            else
            {
                std::cout << "Security Profile == 3 without Charge Point certificate installed : the connection will likely failed"
                          << std::endl;
            }
        }
    }

    // Event handler
    DefaultChargePointEventsHandler event_handler(config);

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

    // Security event can be sent even when the stack has not been started
    // => they are queued (and stored if the functionnality has been enabled) and will automatically forwarded to Central System on connection
    charge_point->logSecurityEvent(SECEVT_STARTUP_OF_DEVICE, "");

    // Start the charge point
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

        // Send custom security events => criticity must be set since they are not known by the stack
        // and only critical events must be formwarded to Central System
        charge_point->logSecurityEvent("MyCustomNotCriticalEvent", "Which has parameters", false);
        charge_point->logSecurityEvent("MyCustomCriticalEvent", "Which also has parameters", true);

        // Test loop
        while (true)
        {

            std::this_thread::sleep_for(std::chrono::seconds(10u));
        }
    }

    return 0;
}
