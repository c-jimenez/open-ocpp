
# Common utility classes for examples libraries
add_library(examples_common STATIC
    simulators/MainMeterSimulator.cpp
    simulators/MeterSimulator.cpp
)
target_include_directories(examples_common PUBLIC . simulators)

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
