
# Rapidjson is an header only library
add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE rapidjson/include)

# Doctest is an header only on library
add_library(doctest INTERFACE)
target_include_directories(doctest INTERFACE doctest/doctest)

# Use default flags for the libwebsockets library
set(CMAKE_C_FLAGS_DEBUG     "-O0 -g3 -ggdb3")
set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3 -ggdb3")
set(CMAKE_C_FLAGS_RELEASE   "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

add_subdirectory(libwebsockets)
