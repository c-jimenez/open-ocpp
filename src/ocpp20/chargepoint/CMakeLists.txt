######################################################
#            OCPP chargepoint library                #
######################################################


# Library target
add_library(chargepoint20 OBJECT
    ChargePoint20.cpp
)

# Exported includes
target_include_directories(chargepoint20 PUBLIC interface)

# Private includes
target_include_directories(chargepoint20 PRIVATE config)

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
