# Quick start Basic Charge Point OCPP 2.0.1 example

## Description

The Basic Charge Point implements only the use cases defined in OCPP 2.0.1 Part 0 Specification §4.

This example simulates a charge point which start a charging session locally with an id tag.

The charge point loops on its connectors. For each connector it simulates the following operations :

* Id tag authorization
* Status notifications (Preparing, Charging, Finishing, Available)
* Transaction start/stop

Each charging sessions lasts 30s and there is a 10s break between 2 charging sessions.

## Command line

quick_start_basicchargepoint [-t id_tag] [-w working_dir] [-r] [-d]

* -t : Id tag to use (Default = AABBCCDDEEFF)
* -w : Working directory where to store the configuration file (Default = current directory)
* -r : Reset all the OCPP persistent data
* -d : Reset all the connector persistent data
