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

# Subdirectories
add_subdirectory(3rdparty)
add_subdirectory(examples)
add_subdirectory(src)

# Tests
if(${BUILD_UNIT_TESTS})
    enable_testing()
    add_subdirectory(tests)
endif()
