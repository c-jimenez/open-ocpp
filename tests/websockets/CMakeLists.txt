######################################################
#         Unit tests for websockets classes          #
######################################################

# Unit tests for Url class
add_executable(test_websockets_url test_websockets_url.cpp)
target_link_libraries(test_websockets_url ws ${OPENOCPP_COMMON_TEST_LIBS})
add_test(
  NAME test_websockets_url
  COMMAND test_websockets_url
)

# Uncomment for debug purpose only
# add_executable(test_websockets test_websockets.cpp)
# target_link_libraries(test_websockets ws ${OPENOCPP_COMMON_TEST_LIBS})
# add_test(
#   NAME test_websockets
#   COMMAND test_websockets
# )
