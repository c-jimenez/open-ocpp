
# Log library
add_library(log STATIC
    LogDatabase.cpp
    Logger.cpp
)
target_include_directories(log PUBLIC .)
target_link_libraries(log database)
