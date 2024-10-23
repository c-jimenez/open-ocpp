# Quick start Local Controller OCPP2.0.1 example

## Description

This example simulates a local controller which accepts any charge point and forwards all the requests from the Central System to the Charge Point and from the Charge Point to the Central System.

The local controller loops on its connected charge points. For each charge point it simulates the following operations :

* Get configuration
* Set heartbeat interval to 10s
* Trigger messages : status notification, meter values, heartbeat

There is a 10s break between 2 charge points communication.

## Command line

quick_start_localcontroller20 [-w working_dir] [-r]

* -w : Working directory where to store the configuration file (Default = current directory)
* -r : Reset all the OCPP persistent data

## Quick start with existing examples

1. Modify the ListenUrl parameter in the configuration file of the Quick Start Central System example to : ```ListenUrl=wss://127.0.0.1:8081/openocpp/```
2. Start the Quick Start Central System Example
3. Start the Quick Start Local Controller Example
4. Start the Quick Start Charge Point Example

The Charge Point will be connected to the Central System through the Local Controller.
