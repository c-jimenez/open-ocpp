# Open OCPP

**Open OCPP** is an Open Source C++ implementation of the OCPP 1.6 and 2.0.1 protocols ([Open Charge Alliance](https://www.openchargealliance.org/)).
This implementation targets only the Websocket/JSON version of these protocols.

This implementation is based on the following libraries :
* [OpenSSL](https://www.openssl.org) : TLS communications + certificates management
* [libwebsockets](https://libwebsockets.org) : Websocket layer
* [SQLite](https://www.sqlite.org/) : Database / persistency
* [rapidjson](https://rapidjson.org/) : JSON serialization/deserialization
* [doctest](https://github.com/doctest/doctest) : Unit tests

## License

**Open OCPP** is distributed over the [GNU Lesser General Public License, version 2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html).
As such it can be used and distributed in any commercial and non-commercial product without affecting the original product's license as long as the **Open OCPP** source code used in the product is made available to anyone.

**Caution**: When using **Open OCPP** in a non GNU GPL/LGPL project, the shared library with dynamic linking is the preferred way of using **Open OCPP** to avoid to have to provide the proprietary parts of your software as a source code or binary object to any person wanting to rebuilt it with a modified version of **Open OCPP** as mentioned in the [GNU LGPL FAQ](https://www.gnu.org/licenses/gpl-faq.en.html#LGPLStaticVsDynamic)

## Table of contents

- [Open OCPP](#open-ocpp)
  - [License](#license)
  - [Table of contents](#table-of-contents)
  - [Features](#features)
  - [Branches and version management](#branches-and-version-management)
  - [Build](#build)
    - [Pre-requisites](#pre-requisites)
    - [Build options](#build-options)
    - [Linux build](#linux-build)
    - [Windows build](#windows-build)
  - [Install and use](#install-and-use)
    - [Installation](#installation)
    - [Use with CMake](#use-with-cmake)
  - [Contributing](#contributing)
    - [Coding rules](#coding-rules)
    - [Issues](#issues)
    - [Workflow](#workflow)

## Features

* [OCPP 1.6 features](./README_ocpp16.md)
* [OCPP 2.0.1 features](./README_ocpp20.md)

## Branches and version management

The development of the OCPP 2.X.Y features has introduced a breaking change in the API and namespaces, so there are now 2 development branches:

* The **develop** branch is now dedicated to the implementation of the OCPP 2.X.Y features and to bug fixes on the OCPP 1.6 features using the new API and namespaces
* The **develop16** branch is dedicated for maintenance of the legacy OCPP 1.6 features and will only be used for bug fixing (backports from **develop** branch)

The new **Open OCPP** releases created from the **develop** branch will have a version number **2.X.Y** starting with the **2.0.0** release.

The legacy **Open OCPP** releases created from the **develop16** branch will have a version number **1.X.Y** starting with the **1.6.0** release.

So if you are already working with **Open OCPP** without having any intention to migrate to the new API or to use the OCPP 2.X.Y features, you can keep following the **1.X.Y** releases. All the OCPP1.6 bug fixes will be backported on these releases.

## Build

### Pre-requisites

* A fully C++17 compliant compiler
* OpenSSL library v1.1.1 or greater
* CMake 3.13 or greater
* Make 4.1 or greater (for Linux build only)
* curl 7.70 or greater (for examples only, to allow diagnotics uploads)
* zip 3.0 or greater (for examples only, to allow diagnotics uploads)

For information, most of the development has been made on the following environment:

* Debian 11 (Bullseye)
* gcc 10.2 and clang 11.0
* OpenSSL 1.1.1k
* CMake 3.18
* Make 4.3

### Build options

The build is based on CMake, the following definitions must be passed to the CMake command to customize the build :

* **TARGET** : Allow to load the appropriate *CMakeLists_TARGET.txt* file => not needed for native GCC/CLang or MSVC since it will be automatically detected
* **BIN_DIR** : Output directory for the generated binaries
* **CMAKE_BUILD_TYPE** : Can be set to either Debug or Release (Release build produces optimized stripped binaries)

Additionnaly, the **CMakeLists_Options.txt** contains several options that can be switched on/off.

The build generates 2 flavors of the **Open OCPP** library depending on the needs of your project :
* Shared : libopen-ocpp.so
* Static : libopen-ocpp_static.a

### Linux build

An helper makefile is available at project's level to simplify the use of CMake. Just use the one of the following commands to build using gcc or gcc without cross compilation :

```make gcc``` or ```make clang``` or ```make gcc BUILD_TYPE=Debug``` or ```make clang BUILD_TYPE=Debug```

This makefile also contains the corresponding cleaning targets :

```make clean-gcc``` or ```make clean-clang```

And to run the unit tests :

```make tests-gcc``` or ```make tests-clang```

### Windows build

**Open OCPP** can be generated on Window plateform using Visual Studio 2022 Community Edition. To open and build the project please follow the standard instructions from Microsoft to [build a CMake based project in Visual Studio](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-170).

You will have to install first a full Windows package of the OpenSSL library (including headers). One can be found on this website : https://slproweb.com/products/Win32OpenSSL.html (do not download the "Light" versions which are not embedding the headers).

Then the following environment variables must be defined to allow Visual Studio/CMake to find the installed OpenSSL package :

* OPENSSL_INCLUDE_DIR
* OPENSSL_ROOT_DIR

Example for a 64-bit OpenSSL package download from the mentioned website and installed in the default folder :

```OPENSSL_INCLUDE_DIR = C:\Program Files\OpenSSL-Win64\include```

```OPENSSL_ROOT_DIR = C:\Program Files\OpenSSL-Win64```

**Note** : Do not forget to close and re-open Visual Studio after having modified the environment variables to have them taken into account

## Install and use
### Installation

**Open OCPP** generated libraries and their includes can be installed in the standard system directories using the CMake command :

```cmake --install [build_dir] --strip```

The makefile contains helper targets which can be called if the installation needs to be done in a non standard directory using the variable *INSTALL_PREFIX* :

```make install-gcc INSTALL_PREFIX=/your/directory``` or ```make install-clang INSTALL_PREFIX=/your/directory```

If run without the *INSTALL_PREFIX* variable, it will install in the standard system directories.

**Note**: If *INSTALL_PREFIX* is used, it must also be defined when building the project with the makefile helper targets.

**Open OCPP** needs the JSON schemas of the OCPP messages during execution. The schemas are installed in the : *INSTALL_DIR*/include/openocpp/schemas directory where *INSTALL_DIR* can be either the standard system directories or the custom directory specified by *INSTALL_PREFIX*.

### Use with CMake

**Open OCPP** installs 2 pkg-config configurations files to ease the use of the library when compiling a CMake project.

To import the library in a CMake project, use the following commands in your CMakeLists.txt file :

```
find_package(PkgConfig)
pkg_search_module(PKG_OPEN_OCPP REQUIRED IMPORTED_TARGET libopen-ocpp)
pkg_search_module(PKG_OPEN_OCPP_STATIC REQUIRED IMPORTED_TARGET libopen-ocpp_static)
```

Then you wil be able to use the following targets as dependencies for your project :

```target_link_libraries(my_proj PRIVATE PkgConfig::PKG_OPEN_OCPP)``` or ```target_link_libraries(my_proj PRIVATE PkgConfig::PKG_OPEN_OCPP_STATIC)```

**Note**: If **Open OCPP** has been installed in a non standard directory, the search path for the ```pkg_search_module``` command must be specified using the following command => ```set(ENV{PKG_CONFIG_PATH} "/your/directory/containing/the/.pc/files")```

See the deploy test [CMakeLists.txt](./tests/deploy/CMakeLists.txt) as an example

## Contributing

**Open OCPP** welcomes contributions. When contributing, please follow the code below.

### Coding rules

* The **.clang-format** file at the root of the source tree must not be modified (or **after** having a discussion between all the contributors)
* The code must formatted using the above mentionned file with a clang-format compliant tools (ex: Visual Studio Code)
* Every interface/class/method must be documented using the [Doxygen](https://www.doxygen.nl/) format
* Use of smart pointers for memory allocation is greatly recommended
* Use of C/C++ macros is discouraged
* Keep code simple to understand and don't be afraid to add comments!

### Issues

Feel free to submit issues and enhancement requests.

Please help us by providing minimal reproducible examples, because source code is easier to let other people understand what happens. For crash problems on certain platforms, please bring stack dump content with the detail of the OS, compiler, etc.

Please try breakpoint debugging first, tell us what you found, see if we can start exploring based on more information been prepared.

### Workflow

Follow the "fork-and-pull" Git workflow :

* Fork the repo on GitHub
* Clone the project to your own machine
* Checkout a new branch on your fork, start developing on the branch
* Test the change before commit, Make sure the changes pass all the tests, please add test case for each new feature or bug-fix if needed.
* Commit changes to your own branch
* Push your work back up to your fork
* Submit a Pull request so that we can review your changes

**Be sure to merge the latest from "upstream" before making a pull request!**
