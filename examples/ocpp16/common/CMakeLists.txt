
# Common utility class for examples library
add_library(examples_common16 OBJECT
    DefaultCentralSystemEventsHandler.cpp
    DefaultChargePointEventsHandler.cpp
    DefaultLocalControllerEventsHandler.cpp

    config/OcppConfig.cpp
)
target_include_directories(examples_common16 PUBLIC . config)

# Dependencies
target_link_libraries(examples_common16 examples_common)
