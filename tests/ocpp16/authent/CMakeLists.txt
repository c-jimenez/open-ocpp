######################################################
#    Unit tests for Charge Point Authent classes     #
######################################################


# Unit tests for AuthentCache class
add_executable(test_authent_cache test_authent_cache.cpp)
target_link_libraries(test_authent_cache ${OPENOCPP_COMMON_TEST_LIBS})
add_test(
  NAME test_authent_cache
  COMMAND test_authent_cache
)

# Unit tests for AuthentLocalList class
add_executable(test_authent_locallist test_authent_locallist.cpp)
target_link_libraries(test_authent_locallist ${OPENOCPP_COMMON_TEST_LIBS})
add_test(
  NAME test_authent_locallist
  COMMAND test_authent_locallist
)

# Unit tests for Authent component
add_executable(test_authent test_authent.cpp)
target_link_libraries(test_authent ${OPENOCPP_COMMON_TEST_LIBS})
add_test(
  NAME test_authent
  COMMAND test_authent
)
