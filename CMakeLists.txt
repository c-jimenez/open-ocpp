#################################################################################
#                               OCPP library                                    #
#################################################################################

cmake_minimum_required(VERSION 3.13)

project(OpenOCPP)

# Build options
include(CMakeLists_Options.txt)

# Target toolchain definition and specific compiler flags
if(NOT DEFINED TARGET)
    set(TARGET native)
endif()
include(CMakeLists_${TARGET}.txt)

# Enable position independant code for dynamic library generation
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# 3rd party
add_subdirectory(3rdparty)

# OpenSSL is mandatory
find_package(OpenSSL REQUIRED COMPONENTS SSL Crypto) 

# Tests
if(${BUILD_UNIT_TESTS})
    enable_testing()
    add_subdirectory(tests)
endif()


# Default output directories
if(NOT DEFINED BIN_DIR)
    set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin/${TARGET})
endif()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

# Subdirectories
add_subdirectory(examples)
add_subdirectory(src)
