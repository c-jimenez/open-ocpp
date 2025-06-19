# Open OCPP 2.0.1

**Work in progress, we welcome any new contributor!**

## Table of contents

- [Open OCPP 2.0.1](#open-ocpp-2.0.1)
  - [Table of contents](#table-of-contents)
  - [Features](#features)

## Features

**Open OCPP** is composed of the 4 layers defined by the protocol :

* Websockets (Client or Server)
* OCPP-J RPC
* JSON messages serialization/deserialization
* OCPP role (Charge Point, Central System or Local Controller)

As of this version :

* All the messages defined in the OCPP 2.0.1 protocol have been implemented (automatic generation from the JSON schemas provided by the OCA)
* A JSON based device model has been implemented
* The following roles have been implemented:
    * Charge Point
    * Central System
    * Local Controller

No behavior has been implemented yet for these roles, the user application will have to implement behaviors in the callbacks provided by the **Open OCPP** stack.
