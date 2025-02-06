######################################################
#            OCPP chargepoint library                #
######################################################


# Library target
add_library(chargepoint20 OBJECT
    ChargePoint20.cpp
    BasicChargePoint20.cpp

    connector/Connectors20.cpp
    devicemodel/DeviceModelManager20.cpp
    status/StatusManager20.cpp
    trigger/TriggerMessageManager20.cpp
)

# Exported includes
target_include_directories(chargepoint20 PUBLIC interface devicemodel connector)

# Private includes
target_include_directories(chargepoint20 PRIVATE config status trigger)

# Dependencies
target_link_libraries(chargepoint20 PUBLIC
    config
    database
    messages20
    rpc
    helpers
    log
    version
    x509
)
