#################################################################################
#                               OCPP library                                    #
#################################################################################

cmake_minimum_required(VERSION 3.13)

project(OpenOCPP DESCRIPTION "Open Source C++ implementation of the OCPP 1.6 protocol"
                 VERSION 1.5.6
)

# Definitions for Version.h file
add_definitions(-DLIBOPENOCPP_MAJOR="${PROJECT_VERSION_MAJOR}")
add_definitions(-DLIBOPENOCPP_MINOR="${PROJECT_VERSION_MINOR}")
add_definitions(-DLIBOPENOCPP_FIX="${PROJECT_VERSION_PATCH}")

# Build options
include(CMakeLists_Options.txt)

# Target toolchain definition and specific compiler flags
if(NOT DEFINED TARGET)
    if (NOT MSVC)
        set(TARGET "gcc")
    else()
        set(TARGET "msvc")
    endif()
endif()
include(CMakeLists_${TARGET}.txt)

# Enable position independant code for dynamic library generation
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# 3rd party
add_subdirectory(3rdparty)

# OpenSSL is mandatory
find_package(OpenSSL REQUIRED COMPONENTS SSL Crypto)

# libwesockets is mandatory if not built along the Open OCPP library
if(NOT ${BUILD_LWS_LIBRARY})
    find_package(PkgConfig)
    pkg_check_modules(LIB_WEBSOCKETS REQUIRED libwebsockets)
endif()

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
if(${BUILD_STATIC_LIBRARY})
    add_library(open-ocpp-static STATIC
        src/version.cpp)
    target_link_libraries(open-ocpp-static
        centralsystem
        chargepoint
        localcontroller
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
    )
    set_target_properties(open-ocpp-static PROPERTIES OUTPUT_NAME "open-ocpp_static")
    set(OPEN_OCPP_STATIC_TARGET open-ocpp-static)
endif()

# Open OCPP dynamic library
if (${BUILD_SHARED_LIBRARY})
    add_library(open-ocpp-dynamic SHARED
        src/version.cpp)
    target_link_libraries(open-ocpp-dynamic
        centralsystem
        chargepoint
        localcontroller
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
    )
    set_target_properties(open-ocpp-dynamic PROPERTIES
        OUTPUT_NAME "open-ocpp"
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
    set(OPEN_OCPP_SHARED_TARGET open-ocpp-dynamic)
endif()

# Install commands
include(GNUInstallDirs)

file(GLOB_RECURSE PUBLIC_HEADERS
     LIST_DIRECTORIES false RELATIVE ${CMAKE_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/src/*.h")
file(GLOB OCPP_SCHEMAS
     LIST_DIRECTORIES false RELATIVE ${CMAKE_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/schemas/*.json")

install(TARGETS ${OPEN_OCPP_SHARED_TARGET} ${OPEN_OCPP_STATIC_TARGET}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openocpp)
install(FILES ${OCPP_SCHEMAS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openocpp/schemas)

file(GLOB RAPIDJSON_HEADERS
     LIST_DIRECTORIES false RELATIVE ${CMAKE_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/3rdparty/rapidjson/include/rapidjson/*.h")
file(GLOB RAPIDJSON_INTERNAL_HEADERS
     LIST_DIRECTORIES false RELATIVE ${CMAKE_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/3rdparty/rapidjson/include/rapidjson/internal/*.h")
file(GLOB RAPIDJSON_ERROR_HEADERS
     LIST_DIRECTORIES false RELATIVE ${CMAKE_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/3rdparty/rapidjson/include/rapidjson/error/*.h")
install(FILES ${RAPIDJSON_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openocpp/rapidjson)
install(FILES ${RAPIDJSON_INTERNAL_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openocpp/rapidjson/internal)
install(FILES ${RAPIDJSON_ERROR_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openocpp/rapidjson/error)

# Generate pkgconfig files
set(PKG_CONFIG_LIBDIR       "\${prefix}/lib")
set(PKG_CONFIG_INCLUDEDIR   "\${prefix}/include/openocpp")
set(PKG_CONFIG_LIBS         "-L\${libdir}")
set(PKG_CONFIG_CFLAGS       "-I\${includedir}")

if(${BUILD_SHARED_LIBRARY})
    set(LIB_NAME                "open-ocpp")
    configure_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/deploy/libopen-ocpp.pc.in"
      "${CMAKE_CURRENT_BINARY_DIR}/libopen-ocpp.pc"
    )
    install(FILES "${CMAKE_BINARY_DIR}/libopen-ocpp.pc" DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()

if(${BUILD_STATIC_LIBRARY})
    set(LIB_NAME                "open-ocpp_static")
    configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/deploy/libopen-ocpp.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/libopen-ocpp_static.pc"
    )
    install(FILES "${CMAKE_BINARY_DIR}/libopen-ocpp_static.pc" DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()
