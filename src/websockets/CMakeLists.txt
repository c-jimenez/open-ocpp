######################################################
#     Websockets library based on libwebsockets      #
#           (https://libwebsockets.org/)             #
######################################################


# Library target
add_library(ws OBJECT
    Url.cpp
    WebsocketFactory.cpp
    libwebsockets/LibWebsocketClient.cpp
    libwebsockets/LibWebsocketClientPool.cpp
    libwebsockets/LibWebsocketServer.cpp
 )

# Private includes
target_include_directories(ws PRIVATE libwebsockets)

# Exported includes
target_include_directories(ws PUBLIC . stubs)

# Dependencies
target_link_libraries(ws PUBLIC
    websockets
    helpers
)
