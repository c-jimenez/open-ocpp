######################################################
#         Unit tests for websockets classes          #
######################################################


# Unit tests for Certificate class
add_definitions(-DCERT_DIR="${CMAKE_CURRENT_LIST_DIR}")
add_executable(test_websockets_certificate test_websockets_certificate.cpp)
target_link_libraries(test_websockets_certificate ws doctest -lstdc++fs)
add_test(
  NAME test_websockets_certificate
  COMMAND test_websockets_certificate
)

# Unit tests for Url class
add_executable(test_websockets_url test_websockets_url.cpp)
target_link_libraries(test_websockets_url ws doctest)
add_test(
  NAME test_websockets_url
  COMMAND test_websockets_url
)
