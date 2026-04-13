######################################################
#            OCPP 2.0 centralsystem library          #
######################################################


# Library target
add_library(centralsystem20 OBJECT
    CentralSystem20.cpp

    chargepoint/ChargePointHandler20.cpp
    chargepoint/ChargePointProxy20.cpp
)

# Exported includes
target_include_directories(centralsystem20 PUBLIC interface)

# Private includes
target_include_directories(centralsystem20 PRIVATE chargepoint
                                                   config)

# Dependencies
target_link_libraries(centralsystem20
    config
    database
    messages20
    rpc
    helpers
    log
    version
    x509
)
