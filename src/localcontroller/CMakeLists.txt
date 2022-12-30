######################################################
#          OCPP localcontroller library              #
######################################################


# Library target
add_library(localcontroller OBJECT
    LocalController.cpp

    centralsystem/CentralSystemHandler.cpp
    centralsystem/CentralSystemProxy.cpp

    chargepoint/ChargePointHandler.cpp
    chargepoint/ChargePointProxy.cpp
)

# Exported includes
target_include_directories(localcontroller PUBLIC interface)

# Private includes
target_include_directories(localcontroller PRIVATE .
                                                   centralsystem
                                                   chargepoint
                                                   config)

# Dependencies
target_link_libraries(localcontroller
    config
    database
    messages
    rpc
    helpers
    log
    version
    x509
)
