
# Common utility class for examples library
add_library(examples_common20 OBJECT
    DefaultChargePointEventsHandler.cpp
    DefaultBasicChargePointEventsHandler.cpp
    DefaultCentralSystemEventsHandler.cpp
    DefaultLocalControllerEventsHandler.cpp
)
target_include_directories(examples_common20 PUBLIC . config simulators)

# Dependencies
target_link_libraries(examples_common20 examples_common)
