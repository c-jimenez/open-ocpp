
# Stubs for the unit tests
add_library(unit_tests_stubs STATIC
    ChargePointEventsHandlerStub.cpp
    MessageDispatcherStub.cpp
    RequestFifoStub.cpp
    RpcStub.cpp
    StatusManagerStub.cpp
    TestableTimerPool.cpp
    WebsocketClientStub.cpp
)
target_include_directories(unit_tests_stubs PUBLIC . 
    ${CMAKE_SOURCE_DIR}/src/chargepoint/authent
    ${CMAKE_SOURCE_DIR}/src/chargepoint/config
    ${CMAKE_SOURCE_DIR}/src/chargepoint/connector
    ${CMAKE_SOURCE_DIR}/src/chargepoint/datatransfer
    ${CMAKE_SOURCE_DIR}/src/chargepoint/maintenance
    ${CMAKE_SOURCE_DIR}/src/chargepoint/metervalues
    ${CMAKE_SOURCE_DIR}/src/chargepoint/requestfifo
    ${CMAKE_SOURCE_DIR}/src/chargepoint/reservation
    ${CMAKE_SOURCE_DIR}/src/chargepoint/security
    ${CMAKE_SOURCE_DIR}/src/chargepoint/smartcharging
    ${CMAKE_SOURCE_DIR}/src/chargepoint/status
    ${CMAKE_SOURCE_DIR}/src/chargepoint/transaction
    ${CMAKE_SOURCE_DIR}/src/chargepoint/trigger
)

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
