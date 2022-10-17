######################################################
# Unit tests for Charge Point Smart Charging classes #
######################################################


# Unit tests for ProfileDatabase class
add_executable(test_profile_database test_profile_database.cpp)
target_link_libraries(test_profile_database unit_tests_stubs doctest sqlite3 pthread dl )
add_test(
  NAME test_profile_database
  COMMAND test_profile_database
)
