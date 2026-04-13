# Quick start Central System OCPP2.0.1 example

## Description

This example simulates a central system which accepts any charge point.

The central system loops on its connected charge points. For each charge point it simulates the following operations :

* Get configuration
* Set heartbeat interval to 10s
* Trigger messages : status notification, meter values, heartbeat

There is a 10s break between 2 charge points communication.

## Command line

quick_start_centralsystem20 [-w working_dir] [-r]

* -w : Working directory where to store the configuration file (Default = current directory)
* -r : Reset all the OCPP persistent data
