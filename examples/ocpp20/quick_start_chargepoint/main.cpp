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
#include "DeviceModelManager20.h"
#include "IChargePoint20.h"

#include <chrono>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <thread>

using namespace ocpp::chargepoint::ocpp20;
using namespace ocpp::types;
using namespace ocpp::types::ocpp20;
using namespace ocpp::messages;
using namespace ocpp::messages::ocpp20;

/** @brief Entry point */
int main(int argc, char* argv[])
{
    // Default parameters
    std::string id_tag      = "AABBCCDDEEFF";
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
            return 1;
        }
    }

    std::cout << "Starting charge point with :" << std::endl;
    std::cout << "  - id_tag = " << id_tag << std::endl;
    std::cout << "  - working_dir = " << working_dir << std::endl;

    // Configuration
    std::filesystem::path path(working_dir);
    path /= "quick_start_chargepoint20.ini";
    ChargePointDemoConfig config(path.string());

    // Device model
    std::filesystem::path device_model_path(working_dir);
    device_model_path /= "quick_start_chargepoint20.json";
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
    DefaultChargePointEventsHandler event_handler(config, device_model_mgr, working_dir);

    // Instanciate charge point
    std::unique_ptr<IChargePoint20> charge_point = IChargePoint20::create(config.stackConfig(), event_handler);
    if (reset_all)
    {
        charge_point->resetData();
    }
    event_handler.setChargePoint(*charge_point.get());
    charge_point->start();

    // From now on the stack is alive :)

    // App loop
    std::string error;
    std::string error_msg;

    auto                       last_boot_notif        = std::chrono::steady_clock::time_point();
    std::chrono::seconds       hb_boot_notif_interval = std::chrono::seconds(10);
    RegistrationStatusEnumType registration_status    = RegistrationStatusEnumType::Rejected;

    unsigned int transaction_id = std::chrono::system_clock::now().time_since_epoch().count();

    while (true)
    {
        // Wait to be connected to the Central System
        std::cout << "Waiting connection to Central System..." << std::endl;
        while (!event_handler.isConnected())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100u));
        }
        std::cout << "Connected to Central System!" << std::endl;

        // Wait to be accepted by Central System
        while (registration_status != RegistrationStatusEnumType::Accepted)
        {
            // Send boot notification message periodically
            auto now = std::chrono::steady_clock::now();
            if ((now - last_boot_notif) >= hb_boot_notif_interval)
            {
                std::cout << "Sending BootNotification request..." << std::endl;

                BootNotificationReq  boot_notif_req;
                BootNotificationConf boot_notif_conf;
                boot_notif_req.reason = BootReasonEnumType::PowerUp;
                boot_notif_req.chargingStation.vendorName.assign(config.stackConfig().chargePointVendor());
                boot_notif_req.chargingStation.model.assign(config.stackConfig().chargePointModel());
                boot_notif_req.chargingStation.firmwareVersion.value().assign(config.stackConfig().firmwareVersion());
                if (!config.stackConfig().iccid().empty())
                {
                    boot_notif_req.chargingStation.modem.value().iccid.value().assign(config.stackConfig().iccid());
                }
                if (!config.stackConfig().imsi().empty())
                {
                    boot_notif_req.chargingStation.modem.value().imsi.value().assign(config.stackConfig().imsi());
                }
                if (charge_point->call(boot_notif_req, boot_notif_conf, error, error_msg))
                {
                    registration_status    = boot_notif_conf.status;
                    hb_boot_notif_interval = std::chrono::seconds(boot_notif_conf.interval);
                }
                else
                {
                    std::cout << "Failed : error = " << error << " error_msg = " << error_msg << std::endl;
                }
                last_boot_notif = std::chrono::steady_clock::now();
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100u));
            }
        }

        // Test loop
        while (event_handler.isConnected())
        {
            // For each evse
            for (unsigned int evse_id = 1u; evse_id <= 2u; evse_id++)
            {
                // For each connector
                for (unsigned int connector_id = 1u; connector_id <= 3u; connector_id++)
                {
                    // Ask for authorization on a tag
                    std::cout << "Checking for id tag " << id_tag << " authorization..." << std::endl;
                    AuthorizeReq  authorize_req;
                    AuthorizeConf authorize_conf;
                    authorize_req.idToken.idToken.assign(id_tag);
                    authorize_req.idToken.type = IdTokenEnumType::ISO14443;
                    if (charge_point->call(authorize_req, authorize_conf, error, error_msg))
                    {
                        if (authorize_conf.idTokenInfo.status == AuthorizationStatusEnumType::Accepted)
                        {
                            std::cout << "Id tag authorized" << std::endl;

                            // Occupied state
                            StatusNotificationReq  status_req;
                            StatusNotificationConf status_conf;
                            status_req.timestamp       = DateTime::now();
                            status_req.connectorStatus = ConnectorStatusEnumType::Occupied;
                            status_req.evseId          = evse_id;
                            status_req.connectorId     = connector_id;
                            charge_point->call(status_req, status_conf, error, error_msg);
                            std::this_thread::sleep_for(std::chrono::seconds(1u));

                            // Try to start charging session
                            transaction_id++;

                            TransactionEventReq  tx_event_req;
                            TransactionEventConf tx_event_conf;
                            tx_event_req.seqNo                    = 1;
                            tx_event_req.eventType                = TransactionEventEnumType::Started;
                            tx_event_req.timestamp                = DateTime::now();
                            tx_event_req.triggerReason            = TriggerReasonEnumType::Authorized;
                            tx_event_req.evse.value().id          = evse_id;
                            tx_event_req.evse.value().connectorId = connector_id;
                            tx_event_req.transactionInfo.transactionId.assign(std::to_string(transaction_id));
                            tx_event_req.transactionInfo.chargingState.value() = ChargingStateEnumType::EVConnected;
                            tx_event_req.idToken.value().idToken.assign(id_tag);
                            tx_event_req.idToken.value().type = IdTokenEnumType::ISO14443;
                            if (charge_point->call(tx_event_req, tx_event_conf, error, error_msg))
                            {
                                if (!tx_event_conf.idTokenInfo.isSet() ||
                                    (tx_event_conf.idTokenInfo.value().status == AuthorizationStatusEnumType::Accepted))
                                {
                                    std::cout << "Transaction authorized, start charging" << std::endl;

                                    // Charging state
                                    tx_event_req.seqNo++;
                                    tx_event_req.eventType                             = TransactionEventEnumType::Updated;
                                    tx_event_req.timestamp                             = DateTime::now();
                                    tx_event_req.triggerReason                         = TriggerReasonEnumType::CablePluggedIn;
                                    tx_event_req.transactionInfo.chargingState.value() = ChargingStateEnumType::Charging;
                                    tx_event_req.idToken.clear();
                                    charge_point->call(tx_event_req, tx_event_conf, error, error_msg);
                                    std::this_thread::sleep_for(std::chrono::seconds(30u));

                                    // End transaction
                                    tx_event_req.seqNo++;
                                    tx_event_req.eventType                             = TransactionEventEnumType::Ended;
                                    tx_event_req.timestamp                             = DateTime::now();
                                    tx_event_req.triggerReason                         = TriggerReasonEnumType::EnergyLimitReached;
                                    tx_event_req.transactionInfo.chargingState.value() = ChargingStateEnumType::Idle;
                                    charge_point->call(tx_event_req, tx_event_conf, error, error_msg);

                                    std::this_thread::sleep_for(std::chrono::seconds(1u));
                                }
                                else
                                {
                                    std::cout << "Transaction not authorized by Central System : "
                                              << AuthorizationStatusEnumTypeHelper.toString(tx_event_conf.idTokenInfo.value().status)
                                              << std::endl;
                                }
                            }
                            else
                            {
                                std::cout << "Failed : error = " << error << " error_msg = " << error_msg << std::endl;
                            }

                            // Available state
                            status_req.timestamp       = DateTime::now();
                            status_req.connectorStatus = ConnectorStatusEnumType::Available;
                            charge_point->call(status_req, status_conf, error, error_msg);
                        }
                        else
                        {
                            std::cout << "Id tag not authorized by Central System : "
                                      << AuthorizationStatusEnumTypeHelper.toString(authorize_conf.idTokenInfo.status) << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "Failed : error = " << error << " error_msg = " << error_msg << std::endl;
                    }

                    // Wait before next charging session
                    if (event_handler.isConnected())
                    {
                        std::this_thread::sleep_for(std::chrono::seconds(10u));
                    }
                }
            }
        }
    }

    return 0;
}
