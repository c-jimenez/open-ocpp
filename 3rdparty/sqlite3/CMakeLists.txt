#################################################################################
#                              SQLite3 library                                  #
#################################################################################

# Use default flags for the SQLite3 library
if (NOT MSVC)
set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG_INIT} -O0 -g3 -ggdb3")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG_INIT} -O0 -g3 -ggdb3")
set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE_INIT} -O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE_INIT} -O2 -DNDEBUG")
else()
set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG_INIT} /WX-")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG_INIT} /WX-")
set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE_INIT} /WX-")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE_INIT} /WX-")
endif()

# Static library
add_library(sqlite3 sqlite3.c)
#target_link_libraries(test_install_dynamic sqlite3)
target_include_directories(sqlite3 PUBLIC .)
