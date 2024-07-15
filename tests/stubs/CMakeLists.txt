
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
    ${CMAKE_SOURCE_DIR}/src/ocpp16/chargepoint/authent
    ${CMAKE_SOURCE_DIR}/src/ocpp16/chargepoint/config
    ${CMAKE_SOURCE_DIR}/src/ocpp16/chargepoint/connector
    ${CMAKE_SOURCE_DIR}/src/ocpp16/chargepoint/datatransfer
    ${CMAKE_SOURCE_DIR}/src/ocpp16/chargepoint/maintenance
    ${CMAKE_SOURCE_DIR}/src/ocpp16/chargepoint/metervalues
    ${CMAKE_SOURCE_DIR}/src/ocpp16/chargepoint/requestfifo
    ${CMAKE_SOURCE_DIR}/src/ocpp16/chargepoint/reservation
    ${CMAKE_SOURCE_DIR}/src/ocpp16/chargepoint/security
    ${CMAKE_SOURCE_DIR}/src/ocpp16/chargepoint/smartcharging
    ${CMAKE_SOURCE_DIR}/src/ocpp16/chargepoint/status
    ${CMAKE_SOURCE_DIR}/src/ocpp16/chargepoint/transaction
    ${CMAKE_SOURCE_DIR}/src/ocpp16/chargepoint/trigger
)

# Dependencies
target_link_libraries(unit_tests_stubs
    centralsystem
    chargepoint
    config 
    types16
    messages16
    log
    database
    json
    rpc
    ws
    helpers
    x509
)
