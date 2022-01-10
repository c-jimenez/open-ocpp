######################################################
#         Unit tests for websockets classes          #
######################################################


# Unit tests for Url class
add_executable(test_websockets_url test_websockets_url.cpp)
target_link_libraries(test_websockets_url ws doctest)
add_test(
  NAME test_websockets_url
  COMMAND test_websockets_url
)
