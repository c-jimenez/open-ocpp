# Open OCPP

**Open OCPP** is an Open Source C++ implementation of the OCPP 1.6 protocol written by the [Open Charge Alliance](https://www.openchargealliance.org/).
This implementation targets only the Websocket/JSON version of this protocol.

This implementation is based on the following libraries :
* [libwebsockets](https://libwebsockets.org) : Websocket layer
* [OpenSSL](https://www.openssl.org) : TLS communications
* [SQLite](https://www.sqlite.org/) : Database / persistency
* [rapidjson](https://rapidjson.org/) : JSON serialization/deserialization
* [doctest](https://github.com/doctest/doctest) : Unit tests

**Table of contents**

1. [Features](#features)
2. [Build](#build)
3. [Quick start](#quick-start)
4. [Contributing](#contributing)
5. [Examples](./examples/README.md)

## Features

### Key features
**Open OCPP** is composed of the 4 layers defined by the protocol :

* Websockets (Client or Server)
* OCPP-J RPC
* JSON messages serialization/deserialization
* OCPP role (Charge Point or Central System)

As of this version :

* Only websocket client has been implemented
* Only Charge Point role has been implemented
* No Charge Point behavior related to the OCPP 1.6 security whitepaper edition 2 has been implemented (work in progress)
* Nearly all the messages defined in the OCPP 1.6 edition 2 protocol have been implemented
* Nearly all the configuration keys defined in the OCPP 1.6 edition 2 protocol have been implemented for the Charge Point role

The user application will have to implement some callbacks to provide the data needed by **Open OCPP** or to handle OCPP events (boot notification, remote start/stop notifications, meter values...).

The persistent data handled by **Open OCPP** is stored into a single file which is an [SQLite](https://www.sqlite.org/) database. It contains :

* Internal configuration
* Persistent data : Central System's registration status, connector state, OCPP transaction related messages when offline, StopTx meter values
* Badge cache and local list
* Smart charging profile
* Logs

The standard OCPP configuration persistency has to be handled by the user application.

### Supported OCPP feature profiles

| Profile | Description | Restrictions |
| :---:   |    :---    | :--- |
| Core | Basic Charge Point functionality comparable with OCPP 1.5 [OCPP1.5] without support for firmware updates, local authorization list management and reservations | OCPP confguration persistency has to be handled by the user application |
| Firmware Management | Support for firmware update management and diagnostic log file download | Actual file download/upload as well as firmware installation must be handled by the user application in the callbacks provided by **Open OCPP** |
| Local Auth List Management | Features to manage the local authorization list in Charge Points | None |
| Reservation | Support for reservation of a Charge Point. | None |
| Smart Charging | Support for basic Smart Charging, for instance using control pilot | GetCompositeSchedule and number of phases parameter in charging profile are not supported for now |
| Remote Trigger | Support for remote triggering of Charge Point initiated messages | None |

### Supported OCPP configuration keys

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
| GetConfigurationMaxKeys | S | Must be set to the sum of OCPP configuration keys count (99) + user application configuration keys count to allow to export all the configuration in 1 message |
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
| WebSocketPingInterval | S | Not implemented yet, websocket ping interval is set to 3s |
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

The best way to start is to take a look at the [examples](./examples/README.md) and more specifically at the [quick start example](./examples/quick_start_chargepoint/README.md).

The implementation of a program using **Open OCPP** is done in 3 steps :
* Implementation of the configuration interfaces [IOcppConfig](./src/config/IOcppConfig.h) and [IChargePointConfig](./src/chargepoint/interface/IChargePointConfig.h)
* Implementation of the event handler interface [IChargePointEventsHandler](./src/chargepoint/interface/IChargePointEventsHandler.h)
* Instanciation and use of the Charge Point object [IChargePoint](./src/chargepoint/interface/IChargePoint.h)

### Configuration interface

The configuration interface allow **Open OCPP** to access to the values of the standard OCPP configuration keys and to the user application specific configuration keys.

The persistency of OCPP configuration key is not handled by **Open OCPP** for 2 main reasons :

* The user application will surely already have a configuration management component
* The user application may have to access the OCPP configuration keys or may want to export its own keys already managed by its configuration component

The configuration interface is split in 2 parts :

* IOcppConfig : interface to access OCPP configuration keys and user application specific configuration keys
* IChargePointConfig : interface to access Charge Point configuration (database paths, url, ...)

In the examples, the interfaces have been implemented to retrieve the values from a file stored into an INI format. This is a simple implementaton which is good to show how to implement these interfaces but which is not the most optimized one since every access to a configuration value implies a conversion from a string. Try to have a better implementation to boost the performances of the software ;)

### Event handler interface

Most of the OCPP behavior is handled by **Open OCPP** but to complete the implementation of the OCPP standard, some information may be needed by **Open OCPP** (Meter values, change availability permissions...) or some operations may be done by the user application (File upload/download, firmware install...).

The event handler interface defines all these interaction as well as some usefull notifications to the user application : connection status, registration status, reservation status...

Most of the notifications/operations can be left empty if the corresponding OCPP feature is not used by the user application. See inside the examples how to implement unused functionalities.

Please keep in mind that all the calls to the event handler interface are made from different threads managed by **Open OCPP** depending the kind of notification/operation and that the treatment of theses calls must not be blocking (except for file upload/download) since it will have an impact to the global scheduling of **Open OCPP**.

### Charge Point object

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

OCPP Charge Point operation are triggered by the Charge Point object interface.

Extract of a quick start main() :

```
int main()
{
    // Configuration
    ChargePointConfig config("config.ini");

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
