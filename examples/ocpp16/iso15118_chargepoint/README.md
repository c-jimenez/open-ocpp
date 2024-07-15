# ISO15118 Charge Point example

## Disclaimer

The certificate management implemented in this example does not follow the state of the arts recommendations :

* Secure storage of private keys / authentication credentials
* No password on some private keys
* ... and surely some more

The choosen implementation has only be made to have a simple and comprehensive example of how to use **Open OCPP** features.

## Description

This example simulates a charge point which start an ISO15118 charging session a token id and a contract certificate.

The charge point loops on its connectors. For each connector it simulates the following operations :

* Id tag authorization
* Status notifications (Preparing, Charging, Finishing, Available)
* Transaction start/stop

Each charging sessions lasts 30s and there is a 10s break between 2 charging sessions.

**Note :** Some of the ISO15118 messages are not containing real data since it would need an actual ISO15118-2 stack and an EV simulator. They
are just provided to demonstrate how to use them.

## Command line

iso15118_chargepoint [-t id_tag] [-w working_dir] [-r] [-d]

* -t : Id tag to use (Default = AABBCCDDEEFF)
* -w : Working directory where to store the configuration file (Default = current directory)
* -r : Reset all the OCPP persistent data
* -d : Reset all the connector persistent data
