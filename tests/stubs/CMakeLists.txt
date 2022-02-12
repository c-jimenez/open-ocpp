
# Stubs for the unit tests
add_library(unit_tests_stubs OBJECT
    WebsocketClientStub.cpp
)
target_include_directories(unit_tests_stubs PUBLIC . )

# Dependencies
target_link_libraries(unit_tests_stubs
    ws
)
