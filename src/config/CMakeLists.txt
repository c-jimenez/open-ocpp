######################################################
#           OCPP configuration library               #
######################################################


# Library target
add_library(config INTERFACE)

# Exported includes
target_include_directories(config INTERFACE .)

# Dependencies
target_link_libraries(config INTERFACE
    types
    helpers
)
