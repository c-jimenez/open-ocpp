# Security Charge Point example
## Disclaimer

The certificate management implemented in this example does not follow the state of the arts recommendations :

* Secure storage of private keys / authentication credentials
* No password on private keys
* ... and surely some more

The choosen implementation has only be made to have a simple and comprehensive example of how to use **Open OCPP** features.

## Description

This example simulates a charge point which uses the messages defined in the security extensions of the standard :

* Security events / logging
* Certificate management and secure connection
* Signed firmware update

This example must be used with the **security_centralsystem** example since 1 step of the implementation of the security profile change is non standard : the configuration of the connection URL of the charge point.

## Command line

security_chargepoint [-t id_tag] [-w working_dir] [-r] [-d]

* -w : Working directory where to store the configuration file and the certificates (Default = current directory)
* -r : Reset all the OCPP persistent data
* -d : Reset all the connector persistent data
