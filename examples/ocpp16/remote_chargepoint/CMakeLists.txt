######################################################
#        Remote charge point example project         #
######################################################

# Executable target
add_executable(remote_chargepoint
    main.cpp
    ChargePointEventsHandler.cpp
    SetpointManager.cpp
)

# Additionnal libraries path
target_link_directories(remote_chargepoint PRIVATE ${BIN_DIR})

# Dependencies
target_link_libraries(remote_chargepoint
    examples_common16
)


# Copy to binary directory
ADD_CUSTOM_COMMAND(TARGET remote_chargepoint
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/config/remote_chargepoint.ini ${BIN_DIR}/
)
