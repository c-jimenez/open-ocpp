######################################################
#   Unit tests for Charge Point Connectors class     #
######################################################


# Unit tests for Connectors class
add_executable(test_connectors test_connectors.cpp)
target_link_libraries(test_connectors ${OPENOCPP_COMMON_TEST_LIBS})
add_test(
  NAME test_connectors
  COMMAND test_connectors
)
