# Security Central System example

## Description

This example simulates a central system which uses the messages defined in the security extensions of the standard.

The central system loops on its connected charge points. For each charge point it simulates the following operations :

* Configure security profile of the Charge Point from 0 to 3
* Security events / logging

This example must be used with the **security_chargepoint** example since 1 step of the implementation of the security profile change is non standard : the configuration of the connection URL of the charge point.

## Command line

security_centralsystem [-w working_dir] [-r]

* -w : Working directory where to store the configuration file and the local database (Default = current directory)
* -r : Reset all the OCPP persistent data
