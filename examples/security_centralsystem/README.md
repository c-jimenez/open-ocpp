# Security Central System example
## Disclaimer

The certificate management implemented in this example does not follow the state of the arts recommendations :

* Secure storage of private keys / authentication credentials
* No password on some private keys
* ... and surely some more

The choosen implementation has only be made to have a simple and comprehensive example of how to use **Open OCPP** features.
## Description

This example simulates a central system which uses the messages defined in the security extensions of the standard.

The central system loops on its connected charge points. For each charge point it simulates the following operations :

* Configure security profile of the Charge Point from 0 to 3
* Security events / logging
* Signed firmware update
* Certificate management

This example must be used with the **security_chargepoint** example since 1 step of the implementation of the security profile change is non standard : the configuration of the connection URL of the charge point.

To have a fully working example, a FTP server must be configured and started on port 21 without credentials. It will allow to receive the security logs export from the Charge Point.
## Command line

security_centralsystem [-w working_dir] [-r]

* -w : Working directory where to store the configuration file and the local database (Default = current directory)
* -r : Reset all the OCPP persistent data
