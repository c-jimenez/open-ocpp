######################################################
#           OCPP configuration library               #
######################################################


# Library target
add_library(config InternalConfigManager.cpp)

# Exported includes
target_include_directories(config INTERFACE .)

# Dependencies
target_link_libraries(config
    types
    helpers
    database
    log
)
