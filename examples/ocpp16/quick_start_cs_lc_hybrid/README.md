# Quick start Hybrid Central System / Local Controller example

## Description

This example simulates a central system which can act as a local controller and which accepts any charge point.

When the charge point identifier ends with "lc", it forwards all the requests from the Central System to the Charge Point and from the Charge Point to the Central System.

Other wise it acts as a central system.

The hybrid central system loops on its connected charge points. For each charge point it simulates the following operations :

* Get configuration

There is a 5s break between loop iterations.

## Command line

quick_start_localcontroller [-w working_dir] [-r]

* -w : Working directory where to store the configuration file (Default = current directory)
* -r : Reset all the OCPP persistent data

## Quick start with existing examples

1. Modify the ListenUrl parameter in the configuration file of the Quick Start Central System example to : ```ListenUrl=wss://127.0.0.1:8081/openocpp/```
2. Start the Quick Start Central System Example
3. Start the Quick Start Hybrid Central System / Local Controller Example
4. Start the Quick Start Charge Point Example

The Charge Point will be connected to the Central System through the Local Controller.
