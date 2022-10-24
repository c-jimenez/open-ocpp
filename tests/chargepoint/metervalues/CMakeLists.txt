######################################################
#  Unit tests for Charge Point Metervalues classes   #
######################################################


# Unit tests for MeterValues component
add_executable(test_metervalues test_metervalues.cpp)
target_link_libraries(test_metervalues unit_tests_stubs doctest sqlite3 pthread dl )
add_test(
  NAME test_metervalues
  COMMAND test_metervalues
)
