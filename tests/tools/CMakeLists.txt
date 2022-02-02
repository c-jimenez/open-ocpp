######################################################
#            Unit tests for helper classes           #
######################################################

# Unit tests for Database class
add_executable(test_database test_database.cpp)
target_link_libraries(test_database database doctest pthread dl -lstdc++fs)
add_test(
  NAME test_database
  COMMAND test_database
)

# Unit tests for log utilities
add_executable(test_logs test_logs.cpp)
target_link_libraries(test_logs log doctest pthread dl -lstdc++fs)
add_test(
  NAME test_logs
  COMMAND test_logs
)

# Unit tests for IniFile class
add_executable(test_inifile test_inifile.cpp)
target_link_libraries(test_inifile helpers doctest)
add_test(
  NAME test_inifile
  COMMAND test_inifile
)

# Unit tests for Queue class
add_executable(test_queue test_queue.cpp)
target_link_libraries(test_queue helpers doctest pthread)
add_test(
  NAME test_queue
  COMMAND test_queue
)

# Unit tests for Timer class
add_executable(test_timers test_timers.cpp)
target_link_libraries(test_timers helpers doctest pthread)
add_test(
  NAME test_timers
  COMMAND test_timers
)

# Unit tests for Timer class
add_executable(test_workerthreadpool test_workerthreadpool.cpp)
target_link_libraries(test_workerthreadpool helpers doctest pthread)
add_test(
  NAME test_workerthreadpool
  COMMAND test_workerthreadpool
)

# Unit tests for X509 library classes
add_definitions(-DCERT_DIR="${CMAKE_CURRENT_LIST_DIR}")
add_executable(test_x509 test_x509.cpp)
target_link_libraries(test_x509 x509 doctest -lstdc++fs)
add_test(
  NAME test_x509
  COMMAND test_x509
)
