
# Rapidjson is an header only library
if(${INSTALL_RAPID_JSON})
    add_library(rapidjson INTERFACE)
    if("${CMAKE_CXX_STANDARD}" GREATER_EQUAL 17)
        # use of std::iterator is deprecated since c++ 17
        target_compile_definitions(rapidjson INTERFACE -DRAPIDJSON_NOMEMBERITERATORCLASS=1)
    endif()
    target_include_directories(rapidjson INTERFACE rapidjson/include)
endif()

# Doctest is an header only on library
if(${INSTALL_DOCTEST})
    add_library(doctest INTERFACE)
    target_include_directories(doctest INTERFACE doctest/doctest)
endif()

# SQLite 3
if(${BUILD_SQLITE})
    add_subdirectory(sqlite3)
endif()

# libwebsockets
if(${BUILD_LWS_LIBRARY})
    # Use default flags for the libwebsockets library
    if (NOT MSVC)
    set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG_INIT} -O0 -g3 -ggdb3")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG_INIT} -O0 -g3 -ggdb3")
    set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE_INIT} -O2 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE_INIT} -O2 -DNDEBUG")
    else()
    set(DISABLED_WARNING_LWS    "/WX- /wd4191 /wd4996")
    set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG_INIT} ${DISABLED_WARNING_LWS}")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG_INIT} ${DISABLED_WARNING_LWS}")
    set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE_INIT} ${DISABLED_WARNING_LWS}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE_INIT} ${DISABLED_WARNING_LWS}")
    endif()

    set(LWS_WITH_NETLINK OFF CACHE BOOL "Monitor Netlink for Routing Table changes")
    SET(LWS_WITH_SYS_ASYNC_DNS ON CACHE BOOL "Nonblocking internal IPv4 + IPv6 DNS resolver")
    add_subdirectory(libwebsockets)
endif()
