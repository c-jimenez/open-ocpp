
# Path to the JSON schemas
add_definitions(-DSCHEMAS_DIR="${CMAKE_SOURCE_DIR}/schemas")

# Common libraries
if (NOT MSVC)
	set(OPENOCPP_COMMON_TEST_LIBS unit_tests_stubs doctest sqlite3 pthread dl)
else()
	set(OPENOCPP_COMMON_TEST_LIBS unit_tests_stubs doctest sqlite3)
endif()

# Doctest wrapper
include_directories(.)

# Subdirectories
add_subdirectory(chargepoint)
add_subdirectory(rpc)
add_subdirectory(stubs)
add_subdirectory(tools)
add_subdirectory(websockets)
