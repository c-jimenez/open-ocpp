#################################################################################
#                  Test installation of the OCPP library                        #
#################################################################################

cmake_minimum_required(VERSION 3.13)

# Build options
include(../../CMakeLists_Options.txt)
enable_testing()

# Target toolchain definition and specific compiler flags
if(NOT DEFINED TARGET)
    set(TARGET native)
endif()
include(../../CMakeLists_${TARGET}.txt)

# Look for Open-OCPP packages
find_package(PkgConfig)
set(ENV{PKG_CONFIG_PATH} "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig/")
pkg_search_module(PKG_OPEN_OCPP REQUIRED IMPORTED_TARGET libopen-ocpp)
if(${BUILD_STATIC_LIBRARY})
pkg_search_module(PKG_OPEN_OCPP_STATIC REQUIRED IMPORTED_TARGET libopen-ocpp_static)
pkg_search_module(PKG_WEBSOCKETS_STATIC REQUIRED IMPORTED_TARGET libwebsockets_static)
endif()

# Look for OpenSSL
find_package(OpenSSL REQUIRED COMPONENTS SSL Crypto)

# Test applications
add_executable(test_install_dynamic main.cpp)
target_link_libraries(test_install_dynamic
            PkgConfig::PKG_OPEN_OCPP
            OpenSSL::SSL
            OpenSSL::Crypto
            sqlite3
            pthread
            
)
add_test(
  NAME test_install_dynamic
  COMMAND test_install_dynamic
)

if(${BUILD_STATIC_LIBRARY})

add_executable(test_install_static main.cpp)
target_link_libraries(test_install_static
            PkgConfig::PKG_OPEN_OCPP_STATIC
)
target_link_libraries(PkgConfig::PKG_OPEN_OCPP_STATIC INTERFACE
            PkgConfig::PKG_WEBSOCKETS_STATIC
            OpenSSL::SSL
            OpenSSL::Crypto
            sqlite3
            pthread
            )
add_test(
  NAME test_install_static
  COMMAND test_install_static
)

endif()
