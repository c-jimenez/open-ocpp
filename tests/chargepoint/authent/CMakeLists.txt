######################################################
#    Unit tests for Charge Point Authent classes     #
######################################################


# Unit tests for AuthentCache class
add_executable(test_authent_cache test_authent_cache.cpp)
target_link_libraries(test_authent_cache unit_tests_stubs doctest sqlite3 pthread dl stdc++fs)
add_test(
  NAME test_authent_cache
  COMMAND test_authent_cache
)
