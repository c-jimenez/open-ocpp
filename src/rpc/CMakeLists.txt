######################################################
#              OCPP RPC protocol library             #
######################################################


# Library target
add_library(rpc OBJECT
    RpcBase.cpp
    RpcClient.cpp
    RpcServer.cpp
)

# Exported includes
target_include_directories(rpc PUBLIC .)

# Dependencies
target_link_libraries(rpc PUBLIC
    ws
    json
    helpers
)
