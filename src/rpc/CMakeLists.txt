######################################################
#              OCPP RPC protocol library             #
######################################################


# Library target
add_library(rpc STATIC
    RpcClient.cpp
)

# Exported includes
target_include_directories(rpc PUBLIC .)

# Dependencies
target_link_libraries(rpc
    ws
    json
    helpers
)
