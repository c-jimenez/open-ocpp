
# Subdirectories
add_subdirectory(centralsystem)
add_subdirectory(chargepoint)
add_subdirectory(config)
add_subdirectory(messages)
add_subdirectory(rpc)
add_subdirectory(tools)
add_subdirectory(types)
add_subdirectory(version)
add_subdirectory(websockets)

# Open OCPP static library
add_library(open-ocpp-static STATIC
    version.cpp)
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
set_target_properties(open-ocpp-static PROPERTIES OUTPUT_NAME "open-ocpp")

# Open OCPP dynamic library
add_library(open-ocpp-dynamic SHARED
    version.cpp)
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
