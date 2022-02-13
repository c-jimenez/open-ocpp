
# Stubs for the unit tests
add_library(unit_tests_stubs STATIC
    MessageDispatcherStub.cpp
    WebsocketClientStub.cpp
)
target_include_directories(unit_tests_stubs PUBLIC . )

# Dependencies
target_link_libraries(unit_tests_stubs
    centralsystem
    chargepoint
    config 
    types
    messages
    log
    database
    json
    rpc
    ws
    helpers
    x509
)
