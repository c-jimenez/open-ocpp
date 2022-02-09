
# Log library
add_library(log OBJECT
    LogDatabase.cpp
    Logger.cpp
)
target_include_directories(log PUBLIC .)
target_link_libraries(log PUBLIC database)
