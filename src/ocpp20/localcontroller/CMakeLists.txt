######################################################
#          OCPP 2.0 localcontroller library          #
######################################################


# Library target
add_library(localcontroller20 OBJECT
    LocalController20.cpp

    centralsystem/CentralSystemHandler20.cpp
    centralsystem/CentralSystemProxy20.cpp

    chargepoint/ChargePointHandler20.cpp
    chargepoint/ChargePointProxy20.cpp
)

# Exported includes
target_include_directories(localcontroller20 PUBLIC interface)

# Private includes
target_include_directories(localcontroller20 PRIVATE .
                                                     centralsystem
                                                     chargepoint
                                                     config)

# Dependencies
target_link_libraries(localcontroller20
    centralsystem20
)
