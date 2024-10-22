######################################################
#               OCPP messages library                #
######################################################


# Library target
add_library(messages OBJECT
    MessageDispatcher.cpp
    GenericMessageSender.cpp
)

# Exported includes
target_include_directories(messages PUBLIC . types)

# Dependencies
target_link_libraries(messages PUBLIC
    rpc
    json
    helpers
    types
    log
)
