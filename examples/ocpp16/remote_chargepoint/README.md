# Remote Charge Point example

## Description

This example simulates a charge point which accepts only remote start/stop request.

On reception of a RemoteStartTransaction request on one of its connector, the Charge Point will start charging and "consuming" its maximum current capacity unless an OCPP charging profile has been set. If so, it will follow the maximum current allowed by the charging profile.

On reception of a RemoteStopTransaction request on the charging connector, the charging session will stop. If no stop request has been received, the charging session will stop after 60s.

## Command line

remote_chargepoint [-m charge_point_max_current] [-c connector_max_current] [-w working_dir] [-r] [-d]

* -m : Maximum current in A for the whole Charge Point (Default = 32A)
* -c : Maximum current in A for a connector (Default = 32A)
* -w : Working directory where to store the configuration file (Default = current directory)
* -r : Reset all the OCPP persistent data
* -d : Reset all the connector persistent data
