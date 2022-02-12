
# Common utility class for examples library
add_library(examples_common OBJECT
    DefaultCentralSystemEventsHandler.cpp
    DefaultChargePointEventsHandler.cpp

    config/OcppConfig.cpp

    simulators/MainMeterSimulator.cpp
    simulators/MeterSimulator.cpp
)
target_include_directories(examples_common PUBLIC . config simulators)

# Dependencies
target_link_libraries(examples_common
    open-ocpp-dynamic
    pthread
)
