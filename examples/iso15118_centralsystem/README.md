# ISO15118 Central System example

## Disclaimer

The certificate management implemented in this example does not follow the state of the arts recommendations :

* Secure storage of private keys / authentication credentials
* No password on some private keys
* ... and surely some more

The choosen implementation has only be made to have a simple and comprehensive example of how to use **Open OCPP** features.

## Description

This example simulates a central system which accepts any charge point and implements the ISO15118 extensions.

The central system loops on its connected charge points. For each charge point it simulates the following operations :

* Get configuration to check if ISO15118 is enabled on charge point side
* List the ISO15118 certificates installed on the charge point
* Delete the ISO15118 certificates installed on the charge point
* Install new ISO1518 root certificates on the charge point
* Trigger an ISO15118 sign certificate request on charge point's side

**Note :** Some of the ISO15118 messages are not containing real data since it would need an actual ISO15118-2 stack and an EV simulator. They
are just provided to demonstrate how to use them.

## Command line

iso15118_centralsystem [-w working_dir] [-r]

* -w : Working directory where to store the configuration file (Default = current directory)
* -r : Reset all the OCPP persistent data
