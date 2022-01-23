######################################################
#            OCPP centralsystem library              #
######################################################


# Library target
add_library(centralsystem STATIC
    CentralSystem.cpp
)

# Exported includes
target_include_directories(centralsystem PUBLIC interface)

# Private includes
target_include_directories(centralsystem PRIVATE authent
                                                 config)

# Dependencies
target_link_libraries(centralsystem
    config
    database
    messages
    rpc
    helpers
    log
    version

    dl
)
