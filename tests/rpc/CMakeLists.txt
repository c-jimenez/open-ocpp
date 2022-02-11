######################################################
#             Unit tests for rpc classes             #
######################################################


# Unit tests for Url class
add_executable(test_rpc test_rpc.cpp)
target_link_libraries(test_rpc rpc ws doctest pthread dl stdc++fs)
add_test(
  NAME test_rpc
  COMMAND test_rpc
)
