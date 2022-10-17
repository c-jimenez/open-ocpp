######################################################
#    Unit tests for Charge Point Authent classes     #
######################################################


# Unit tests for AuthentCache class
add_executable(test_authent_cache test_authent_cache.cpp)
target_link_libraries(test_authent_cache unit_tests_stubs doctest sqlite3 pthread dl )
add_test(
  NAME test_authent_cache
  COMMAND test_authent_cache
)

# Unit tests for AuthentLocalList class
add_executable(test_authent_locallist test_authent_locallist.cpp)
target_link_libraries(test_authent_locallist unit_tests_stubs doctest sqlite3 pthread dl )
add_test(
  NAME test_authent_locallist
  COMMAND test_authent_locallist
)

# Unit tests for Authent component
add_executable(test_authent test_authent.cpp)
target_link_libraries(test_authent unit_tests_stubs doctest sqlite3 pthread dl )
add_test(
  NAME test_authent
  COMMAND test_authent
)
