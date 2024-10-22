######################################################
#       Securiy central system example project       #
######################################################

# Executable target
add_executable(security_centralsystem
    main.cpp

    CentralSystemEventsHandler.cpp
    ChargePointDatabase.cpp
)

# Additionnal libraries path
target_link_directories(security_centralsystem PRIVATE ${BIN_DIR})

# Dependencies
target_link_libraries(security_centralsystem
    examples_common16
    centralsystem
)


# Copy to binary directory
ADD_CUSTOM_COMMAND(TARGET security_centralsystem
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/config/security_centralsystem_p0.ini ${BIN_DIR}/
)
ADD_CUSTOM_COMMAND(TARGET security_centralsystem
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/config/security_centralsystem_p1.ini ${BIN_DIR}/
)
ADD_CUSTOM_COMMAND(TARGET security_centralsystem
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/config/security_centralsystem_p2.ini ${BIN_DIR}/
)
ADD_CUSTOM_COMMAND(TARGET security_centralsystem
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/config/security_centralsystem_p3.ini ${BIN_DIR}/
)
