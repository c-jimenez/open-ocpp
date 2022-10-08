# Quick start Local Controller example

## Description

This example simulates a local controller which accepts any charge point and forwards all the requests from the Central System to the Charge Point and from the Charge Point to the Central System.

The local controller loops on its connected charge points. For each charge point it simulates the following operations :

* Get configuration
* Set heartbeat interval to 10s
* Trigger messages : status notification, meter values, heartbeat

There is a 10s break between 2 charge points communication.

## Command line

quick_start_localcontroller [-w working_dir] [-r]

* -w : Working directory where to store the configuration file (Default = current directory)
* -r : Reset all the OCPP persistent data
