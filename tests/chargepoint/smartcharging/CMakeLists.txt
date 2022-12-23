######################################################
# Unit tests for Charge Point Smart Charging classes #
######################################################


# Unit tests for the composite schedule feature
add_executable(test_composite_schedule1 test_composite_schedule1.cpp)
target_link_libraries(test_composite_schedule1 unit_tests_stubs doctest sqlite3 pthread dl )
add_test(
  NAME test_composite_schedule1
  COMMAND test_composite_schedule1
)
add_executable(test_composite_schedule2 test_composite_schedule2.cpp)
target_link_libraries(test_composite_schedule2 unit_tests_stubs doctest sqlite3 pthread dl )
add_test(
  NAME test_composite_schedule2
  COMMAND test_composite_schedule2
)

# Unit tests for ProfileDatabase class
add_executable(test_profile_database test_profile_database.cpp)
target_link_libraries(test_profile_database unit_tests_stubs doctest sqlite3 pthread dl )
add_test(
  NAME test_profile_database
  COMMAND test_profile_database
)
