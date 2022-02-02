# Open OCPP

**Open OCPP** is an Open Source C++ implementation of the OCPP 1.6 protocol ([Open Charge Alliance](https://www.openchargealliance.org/)).
This implementation targets only the Websocket/JSON version of this protocol.

This implementation is based on the following libraries :
* [OpenSSL](https://www.openssl.org) : TLS communications + certificates management
* [libwebsockets](https://libwebsockets.org) : Websocket layer
* [SQLite](https://www.sqlite.org/) : Database / persistency
* [rapidjson](https://rapidjson.org/) : JSON serialization/deserialization
* [doctest](https://github.com/doctest/doctest) : Unit tests

## Table of contents

* [Features](#features)
  + [Key features](#key-features)
  + [Supported OCPP feature profiles](#supported-ocpp-feature-profiles)
  + [Supported OCPP configuration keys](#supported-ocpp-configuration-keys)
  + [OCPP security extensions](#ocpp-security-extensions)
* [Build](#build)
* [Quick start](#quick-start)
  + [Charge Point role](#charge-point-role)
  + [Central System role](#central-system-role)
* [Contributing](#contributing)
* [Examples](./examples/README.md)

## Features

### Key features
**Open OCPP** is composed of the 4 layers defined by the protocol :

* Websockets (Client or Server)
* OCPP-J RPC
* JSON messages serialization/deserialization
* OCPP role (Charge Point or Central System)

As of this version :

* All the messages defined in the OCPP 1.6 edition 2 protocol have been implemented except GetCompositeSchedule for Charge Point role
* All the configuration keys defined in the OCPP 1.6 edition 2 protocol have been implemented for the Charge Point role
* Some Charge Point and Central System behavior related to the OCPP 1.6 security whitepaper edition 2 has been implemented (work in progress, see [OCPP security extensions](#ocpp-security-extensions))

The user application will have to implement some callbacks to provide the data needed by **Open OCPP** or to handle OCPP events (boot notification, remote start/stop notifications, meter values...).

The persistent data handled by **Open OCPP** is stored into a single file which is an [SQLite](https://www.sqlite.org/) database. It contains :

* For Charge Point role :

  + Internal configuration
  + Persistent data : Central System's registration status, connector state, OCPP transaction related messages when offline, StopTx meter values
  + Badge cache and local list
  + Smart charging profile
  + Logs

* For Central System role :

  + Internal configuration
  + Logs

The standard OCPP configuration persistency has to be handled by the user application.

### Supported OCPP feature profiles

| Profile | Description | Restrictions |
| :---:   |    :---    | :--- |
| Core | Basic Charge Point functionality comparable with OCPP 1.5 [OCPP1.5] without support for firmware updates, local authorization list management and reservations | OCPP confguration persistency has to be handled by the user application |
| Firmware Management | Support for firmware update management and diagnostic log file download | Actual file download/upload as well as firmware installation must be handled by the user application in the callbacks provided by **Open OCPP** |
| Local Auth List Management | Features to manage the local authorization list in Charge Points | None |
| Reservation | Support for reservation of a Charge Point. | None |
| Smart Charging | Support for basic Smart Charging, for instance using control pilot | GetCompositeSchedule is not supported for now in Charge Point role |
| Remote Trigger | Support for remote triggering of Charge Point initiated messages | None |

### Supported OCPP configuration keys

The OCPP configuration keys support applies to Charge Point role only.

In the "Owner" column, "S" means that the configuration key behavior is handled by the stack, "U" means that it must handled by the user application.

| Key | Owner | Restrictions |
| :---: | :---: | :--- |
| AllowOfflineTxForUnknownId | S | None |
| AuthorizationCacheEnabled | S | None |
| AuthorizeRemoteTxRequests | S | None |
| BlinkRepeat | U | None |
| ClockAlignedDataInterval | S | None |
| ConnectionTimeOut | S | None |
| ConnectorPhaseRotation | S | None |
| ConnectorPhaseRotationMaxLength | S | None |
| GetConfigurationMaxKeys | S | Must be set to the sum of OCPP configuration keys count (49) + user application configuration keys count to allow to export all the configuration in 1 message |
| HeartbeatInterval | S | Heartbeat are only sent if no messages have been exchanged since HeartbeatInterval seconds |
| LightIntensity | U | None |
| LocalAuthorizeOffline | S | None |
| LocalPreAuthorize | S | None |
| MaxEnergyOnInvalidId | U | None |
| MeterValuesAlignedData | S+U | User application must validate the requested meter value list according to its metering capacities when modified by the Central System |
| MeterValuesAlignedDataMaxLength | S | None |
| MeterValuesSampledData | S+U | User application must validate the requested meter value list according to its metering capacities when modified by the Central System |
| MeterValuesSampledDataMaxLength | S | None |
| MeterValueSampleInterval | S | None |
| MinimumStatusDuration | S | None |
| NumberOfConnectors | S | None |
| ResetRetries | U | None |
| StopTransactionOnEVSideDisconnect | U | None |
| StopTransactionOnInvalidId | U | None |
| StopTxnAlignedData | S+U | User application must validate the requested meter value list according to its metering capacities when modified by the Central System |
| StopTxnAlignedDataMaxLength | S | None |
| StopTxnSampledData | S+U | User application must validate the requested meter value list according to its metering capacities when modified by the Central System |
| StopTxnSampledDataMaxLength | S | None |
| SupportedFeatureProfiles | S | None |
| SupportedFeatureProfilesMaxLength | S | None |
| TransactionMessageAttempts | S | None |
| TransactionMessageRetryInterval | S | None |
| UnlockConnectorOnEVSideDisconnect | U | None |
| WebSocketPingInterval | S | Reboot required |
| LocalAuthListEnabled | S | None |
| LocalAuthListMaxLength | S | None |
| SendLocalListMaxLength | S | None |
| ReserveConnectorZeroSupported | S | None |
| ChargeProfileMaxStackLevel | S | None |
| ChargingScheduleAllowedChargingRateUnit | S | None |
| ChargingScheduleMaxPeriods | S | None |
| ConnectorSwitch3to1PhaseSupported | S | None |
| MaxChargingProfilesInstalled | S | None |
| AdditionalRootCertificateCheck | S | OCPP 1.6 security whitepaper edition 2 configuration key : not implemented yet |
| AuthorizationKey | S | None |
| CertificateSignedMaxChainSize | S | OCPP 1.6 security whitepaper edition 2 configuration key : not implemented yet |
| CertificateStoreMaxLength | S | OCPP 1.6 security whitepaper edition 2 configuration key : not implemented yet |
| CpoName | S | OCPP 1.6 security whitepaper edition 2 configuration key : not implemented yet |
| SecurityProfile | S | OCPP 1.6 security whitepaper edition 2 configuration key : not implemented yet |

### OCPP security extensions

#### Security profiles

**Open OCPP** support the following Security Profiles for both Charge Point and Central System roles :

* 0 : No security profile
* 1 : Unsecured Transport with HTTP Basic Authentication
* 2 : TLS with HTTP Basic Authentication
* 3 : TLS with Client Side Certificates

In Charge Point role, the stack will automatically disconnect and then reconnect to the Central System after one of the following parameters has been modified : 
* **AuthorizationKey**
* **Security Profile**

#### Security events

**Open OCPP** support the whole use cases of security events and logging. 

In Charge Point role, it can optionnaly handle the storage of the security event log and the generation of the security log export when the Central System asks it. To enable/disable this feature, you have to modify the **SecurityLogMaxEntriesCount** charge point configuration value :

* 0 = **Open OCPP** will not store security event and the security log must be generated by the user application
* \>0 = **Open OCPP** will store at max **SecurityLogMaxEntriesCount** (circular log) and will automatically generate the security log as a CSV file

In Charge Point role, the user application can generate custom security events and defines its criticity so that they are forwarded to the Central System.

#### Extended trigger messages

**Open OCPP** support this feature for both Charge Point and Central System roles.

#### Certificate management messages

**Open OCPP** support this feature for both Charge Point and Central System roles. 

The actual storage of the certificates and their keys must be done by the user application. 

**Open OCPP** provides callbacks and helper classes to ease certificate manipulation and installation.

## Build

### Pre-requisites

* A fully C++17 compliant compiler
* OpenSSL library v1.1.1 or greater
* SQLite 3 library
* CMake 3.13 or greater
* Make 4.1 or greater
* curl 7.70 or greater (for examples only, to allow diagnotics uploads)
* zip 3.0 or greater (for examples only, to allow diagnotics uploads)

For information, most of the development has been made on the following environment:

* Debian 11 (Bullseye)
* gcc 10.2 and clang 11.0
* OpenSSL 1.1.1k
* SQLite 3.34.1
* CMake 3.18
* Make 4.3

### Build options

The build is based on CMake, the following definitions must be passed to the CMake command to customize the build :

* **TARGET** : Allow to load the appropriate *CMakeLists_TARGET.txt* file
* **BIN_DIR** : Output directory for the generated binaries
* **DEBUG** : If set to ON, use DEBUG compilation flag, otherwise use release compilation flags

Additionnaly, the **CMakeLists_Options.txt** contains several options that can be switched on/off.

An helper makefile is available at project's level to simplify the use of CMake. Just use the one of the following commands to build using gcc or gcc without cross compilation :

```make gcc-native``` or ```make clang-native```

This makefile also contains the corresponding cleaning targets :

```make clean-gcc-native``` or ```make clean-clang-native```

And to run the unit tests :

```make tests-gcc-native``` or ```make tests-clang-native```

## Quick start

The best way to start is to take a look at the [examples](./examples/README.md) and more specifically at the [quick start Charge Point example](./examples/quick_start_chargepoint/README.md) and the [quick start Central System example](./examples/quick_start_centralsystem/README.md).

### Charge Point role

The implementation of a program using **Open OCPP** in Charge Point role is done in 3 steps :
* Implementation of the configuration interfaces [IOcppConfig](./src/config/IOcppConfig.h) and [IChargePointConfig](./src/chargepoint/interface/IChargePointConfig.h)
* Implementation of the event handler interface [IChargePointEventsHandler](./src/chargepoint/interface/IChargePointEventsHandler.h)
* Instanciation and use of the Charge Point object [IChargePoint](./src/chargepoint/interface/IChargePoint.h)

#### Configuration interface

The configuration interface allow **Open OCPP** to access to the values of the standard OCPP configuration keys and to the user application specific configuration keys.

The persistency of OCPP configuration key is not handled by **Open OCPP** for 2 main reasons :

* The user application will surely already have a configuration management component
* The user application may have to access the OCPP configuration keys or may want to export its own keys already managed by its configuration component

The configuration interface is split in 2 parts :

* IOcppConfig : interface to access OCPP configuration keys and user application specific configuration keys
* IChargePointConfig : interface to access Charge Point configuration (database paths, url, ...)

In the examples, the interfaces have been implemented to retrieve the values from a file stored into an INI format. This is a simple implementaton which is good to show how to implement these interfaces but which is not the most optimized one since every access to a configuration value implies a conversion from a string. Try to have a better implementation to boost the performances of the software ;)

#### Event handler interface

Most of the OCPP behavior is handled by **Open OCPP** but to complete the implementation of the OCPP standard, some information may be needed by **Open OCPP** (Meter values, change availability permissions...) or some operations may be done by the user application (File upload/download, firmware install...).

The event handler interface defines all these interaction as well as some usefull notifications to the user application : connection status, registration status, reservation status...

Most of the notifications/operations can be left empty if the corresponding OCPP feature is not used by the user application. See inside the examples how to implement unused functionalities.

Please keep in mind that all the calls to the event handler interface are made from different threads managed by **Open OCPP** depending the kind of notification/operation and that the treatment of theses calls must not be blocking (except for file upload/download) since it will have an impact to the global scheduling of **Open OCPP**.

#### Charge Point object

This is the easiest part :)

The Charge Point object is instanciated through a factory interface :

```
/**
 * @brief Instanciate a charge point
 * @param stack_config Stack configuration
 * @param ocpp_config Standard OCPP configuration
 * @param event_handler Stack event handler
 */
static std::unique_ptr<IChargePoint> create(const ocpp::config::IChargePointConfig& stack_config,
                                            ocpp::config::IOcppConfig&              ocpp_config,
                                            IChargePointEventsHandler&              events_handler);
```

The 3 parameters are the instances of the interfaces you have implemented in the previous steps.

Before starting the Charge Point object and thus the OCPP stack with the ```start()``` method, you can clear existing persistent data using the following methods:

* ```resetConnectorData()``` : clear all connector related data
* ```resetData()``` : clear all the persistent data

Once the Charge Point object has been started, **Open OCPP** will continuously try to connect/keep alive the communication with the Central System until a call to the ```stop()``` method which will disconnect and release the connection.

Connectivity status, registration status and Central System initiated operations will be notified through the event handler interface.

OCPP Charge Point operations are triggered by the Charge Point object interface.

Extract of a quick start main() :

```
int main()
{
    // Configuration
    ChargePointDemoConfig config("config.ini");

    // Event handler
    DefaultChargePointEventsHandler event_handler(config);

    // Instanciate charge point
    std::unique_ptr<IChargePoint> charge_point = IChargePoint::create(config.stackConfig(), config.ocppConfig(), event_handler);
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

        // Ask for authorization on a tag
        std::string               parent_id;
        std::string id_tag = "AABBCCDDEEFF";
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

    return 0;
}
```

### Central System role

The implementation of a program using **Open OCPP** in Central System role is done in 3 steps :
* Implementation of the configuration interface [ICentralSystemConfig](./src/centralsystem/interface/ICentralSystemConfig.h)
* Implementation of the event handler interfaces [ICentralSystemEventsHandler](./src/centralsystem/interface/ICentralSystemEventsHandler.h) and [IChargePointRequestHandler](./src/centralsystem/interface/IChargePointRequestHandler.h)
* Instanciation and use of the Central System object [ICentralSystem](./src/centralsystem/interface/ICentralSystem.h)

#### Configuration interface

The configuration interface allow **Open OCPP** to access to its configuration values.
The persistency of the configuration is not handled by **Open OCPP** for 2 main reasons :

* The user application will surely already have a configuration management component
* The user application may have to access the **Open OCPP** configuration

In the examples, the interface has been implemented to retrieve the values from a file stored into an INI format. This is a simple implementaton which is good to show how to implement this interface but which is not the most optimized one since every access to a configuration value implies a conversion from a string. Try to have a better implementation to boost the performances of the software ;)

#### Event handler interfaces

In Central System role, the OCPP behavior must be implemented by the user application. **Open OCPP** handles all the other layers of the stack (websocket, RPC, JSON serialization/deserialization).

The **Open OCPP** stack will interact with the user application through 2 interfaces :

* [ICentralSystemEventsHandler](./src/centralsystem/interface/ICentralSystemEventsHandler.h) : used for all connection related events (credentials check, connection notification...)
* [IChargePointRequestHandler](./src/centralsystem/interface/IChargePointRequestHandler.h) : used to handle incoming requests from a Charge Point (boot notification, status notification, start transaction...)

The **ICentralSystemEventsHandler** must be instanciated only once for a Central System implementation.

The **IChargePointRequestHandler** must be instanciated and registered to each connected Charge Point in the **ICentralSystemEventsHandler::chargePointConnected** method implementation.

Example of Charge Point connection handling :

```
/** @copydoc bool ICentralSystemEventsHandler::chargePointConnected(std::shared_ptr<ICentralSystem::IChargePoint>) */
void MyCentralSystemEventsHandler::chargePointConnected(std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint> chargepoint)
{
    cout << "Charge point [" << chargepoint->identifier() << "] connected" << endl;
    auto iter_chargepoint = m_chargepoints.find(chargepoint->identifier());
    if (iter_chargepoint == m_chargepoints.end())
    {
        MyChargePointRequestHandler* my_handler = new MyChargePointRequestHandler(*this, chargepoint);
        chargepoint->registerHandler(*my_handler);
        m_chargepoints[chargepoint->identifier()] =
            std::shared_ptr<ChargePointRequestHandler>(my_handler);
    }
    else
    {
        cout << "Charge point [" << chargepoint->identifier() << "] already connected" << endl;
        chargepoint.reset();
    }
}
```

Please keep in mind that all the calls to the event handler interface are made from different threads managed by **Open OCPP** depending the kind of notification/operation and that the treatment of theses calls must not be blocking since it will have an impact to the global scheduling of **Open OCPP**.

#### Central System object

This is the easiest part :)

The Central System object is instanciated through a factory interface :

```
/**
 * @brief Instanciate a central system
 * @param stack_config Stack configuration
 * @param event_handler Stack event handler
*/
static std::unique_ptr<ICentralSystem> create(const ocpp::config::ICentralSystemConfig& stack_config,
                                              ICentralSystemEventsHandler&              events_handler);
```

The 2 parameters are the instances of the interfaces you have implemented in the previous steps.

Before starting the Central System object and thus the OCPP stack with the ```start()``` method, you can clear existing persistent data using the following method:

```resetData()``` : clear all the persistent data

Once the Central System object has been started, **Open OCPP** will continuously listen to incoming connections from the Charge Points until a call to the ```stop()``` method which will disconnect all the Charge Points and release the connection.

Connectivity status and Charge Point initiated operations will be notified through the event handler interfaces.

OCPP Central System operations are triggered by the Charge Point proxy interface [ICentralSystem::IChargePoint](./src/centralsystem/interface/ICentralSystem.h) which is instanciated by **Open OCPP** for each connected Charge Point.

Extract of a quick start main() :

```
int main()
{
    // Configuration
    CentralSystemDemoConfig config("config.ini");

    // Event handler
    CentralSystemDemoConfig event_handler(config);

    // Instanciate central system
    std::unique_ptr<ICentralSystem> central_system = ICentralSystem::create(config.stackConfig(), event_handler);
    central_system->start();

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

                std::cout << "Configure heartbeat interval..." << std::endl;
                ConfigurationStatus config_status = chargepoint->changeConfiguration("HeartbeatInterval", "10");
                std::cout << ConfigurationStatusHelper.toString(config_status) << std::endl;

                std::cout << "Trigger status notification..." << std::endl;
                TriggerMessageStatus trigger_status =
                    chargepoint->triggerMessage(MessageTrigger::StatusNotification, Optional<unsigned int>());
                std::cout << TriggerMessageStatusHelper.toString(trigger_status) << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }

    return 0;
}
```

## Contributing

**Open OCPP** welcomes contributions. When contributing, please follow the code below.

### Coding rules

* The **.clang-format** file at the root of the source tree must not be modified (or **after** having a discussion between all the contributors)
* The code must formatted using the above mentionned file with a clang-format compliant tools (ex: Visual Studio Code)
* Every interface/class/method must be documented using the [Doxygen](https://www.doxygen.nl/) format
* Use of smart pointers for memory allocation is greatly recommended
* Use of C/C++ macros is discouraged
* Keep code simple to understand and don't be afraid to add comments!

### Issues

Feel free to submit issues and enhancement requests.

Please help us by providing minimal reproducible examples, because source code is easier to let other people understand what happens. For crash problems on certain platforms, please bring stack dump content with the detail of the OS, compiler, etc.

Please try breakpoint debugging first, tell us what you found, see if we can start exploring based on more information been prepared.

### Workflow

Follow the "fork-and-pull" Git workflow :

* Fork the repo on GitHub
* Clone the project to your own machine
* Checkout a new branch on your fork, start developing on the branch
* Test the change before commit, Make sure the changes pass all the tests, please add test case for each new feature or bug-fix if needed.
* Commit changes to your own branch
* Push your work back up to your fork
* Submit a Pull request so that we can review your changes

**Be sure to merge the latest from "upstream" before making a pull request!**
