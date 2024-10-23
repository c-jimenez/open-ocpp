######################################################
#            OCPP centralsystem library              #
######################################################


# Library target
add_library(centralsystem OBJECT
    CentralSystem.cpp

    chargepoint/ChargePointHandler.cpp
    chargepoint/ChargePointProxy.cpp
)

# Exported includes
target_include_directories(centralsystem PUBLIC interface)

# Private includes
target_include_directories(centralsystem PRIVATE chargepoint
                                                 config)

# Dependencies
target_link_libraries(centralsystem
    config
    database
    messages16
    rpc
    helpers
    log
    version
    x509
)
