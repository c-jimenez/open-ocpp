#################################################################################
#                               OCPP library                                    #
#################################################################################

cmake_minimum_required(VERSION 3.13)

project(OpenOCPP DESCRIPTION "Open Source C++ implementation of the OCPP 1.6 protocol"
                 VERSION 0.5.0
)

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

# Examples
if(${BUILD_EXAMPLES})
    add_subdirectory(examples)
endif()

# Open OCPP library sources
add_subdirectory(src)

# Open OCPP static library
add_library(open-ocpp-static STATIC
    src/version.cpp)
target_link_libraries(open-ocpp-static
    centralsystem
    chargepoint
    config
    database
    messages
    rpc
    helpers
    log
    version
    x509
    json
    ws
    websockets

    dl
)
set_target_properties(open-ocpp-static PROPERTIES OUTPUT_NAME "open-ocpp_static")

# Open OCPP dynamic library
add_library(open-ocpp-dynamic SHARED
    src/version.cpp)
target_link_libraries(open-ocpp-dynamic
    centralsystem
    chargepoint
    config
    database
    messages
    rpc
    helpers
    log
    version
    x509
    json
    ws
    websockets

    dl
)
set_target_properties(open-ocpp-dynamic PROPERTIES OUTPUT_NAME "open-ocpp")

# Install commands
include(GNUInstallDirs)

file(GLOB_RECURSE PUBLIC_HEADERS RELATIVE ${CMAKE_SOURCE_DIR} "src/*.h")

install(TARGETS open-ocpp-static open-ocpp-dynamic
        EXPORT LibOpenOcppTargets

        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openocpp)

# Generate pkgconfig files
set(PKG_CONFIG_LIBDIR       "\${prefix}/lib")
set(PKG_CONFIG_INCLUDEDIR   "\${prefix}/openocpp")
set(PKG_CONFIG_LIBS         "-L\${libdir}")
set(PKG_CONFIG_CFLAGS       "-I\${includedir}")

set(LIB_NAME                "open-ocpp")
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/libopen-ocpp.pc.in"
  "${CMAKE_CURRENT_BINARY_DIR}/libopen-ocpp.pc"
)
install(FILES "${CMAKE_BINARY_DIR}/libopen-ocpp.pc" DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

set(LIB_NAME                "open-ocpp_static")
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/libopen-ocpp.pc.in"
  "${CMAKE_CURRENT_BINARY_DIR}/libopen-ocpp_static.pc"
)
install(FILES "${CMAKE_BINARY_DIR}/libopen-ocpp_static.pc" DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
