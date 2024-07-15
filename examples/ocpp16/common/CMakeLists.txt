
# Common utility class for examples library
add_library(examples_common OBJECT
    DefaultCentralSystemEventsHandler.cpp
    DefaultChargePointEventsHandler.cpp
    DefaultLocalControllerEventsHandler.cpp

    config/OcppConfig.cpp

    simulators/MainMeterSimulator.cpp
    simulators/MeterSimulator.cpp
)
target_include_directories(examples_common PUBLIC . config simulators)

# Dependencies
if (NOT MSVC)
target_link_libraries(examples_common
    open-ocpp-dynamic
    pthread
)
else()
target_link_libraries(examples_common
    open-ocpp-static
)
endif()
