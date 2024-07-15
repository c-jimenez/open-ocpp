######################################################
# Unit tests for Charge Point Smart Charging classes #
######################################################


# Unit tests for the composite schedule feature
add_executable(test_composite_schedule1 test_composite_schedule1.cpp)
target_link_libraries(test_composite_schedule1 ${OPENOCPP_COMMON_TEST_LIBS})
add_test(
  NAME test_composite_schedule1
  COMMAND test_composite_schedule1
)
add_executable(test_composite_schedule2 test_composite_schedule2.cpp)
target_link_libraries(test_composite_schedule2 ${OPENOCPP_COMMON_TEST_LIBS})
add_test(
  NAME test_composite_schedule2
  COMMAND test_composite_schedule2
)
add_executable(test_composite_schedule3 test_composite_schedule3.cpp)
target_link_libraries(test_composite_schedule3 ${OPENOCPP_COMMON_TEST_LIBS})
add_test(
  NAME test_composite_schedule3
  COMMAND test_composite_schedule3
)

# Unit tests for ProfileDatabase class
add_executable(test_profile_database test_profile_database.cpp)
target_link_libraries(test_profile_database ${OPENOCPP_COMMON_TEST_LIBS})
add_test(
  NAME test_profile_database
  COMMAND test_profile_database
)

# Unit tests for smartcharging setpoint
add_executable(test_smartcharging_setpoint test_smartcharging_setpoint.cpp)
target_link_libraries(test_smartcharging_setpoint ${OPENOCPP_COMMON_TEST_LIBS})
add_test(
  NAME test_smartcharging_setpoint
  COMMAND test_smartcharging_setpoint
)
