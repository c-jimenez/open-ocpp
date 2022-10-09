
# Path to the JSON schemas
add_definitions(-DSCHEMAS_DIR="${CMAKE_SOURCE_DIR}/schemas")

# Subdirectories
add_subdirectory(chargepoint)
add_subdirectory(rpc)
add_subdirectory(stubs)
add_subdirectory(tools)
add_subdirectory(websockets)
