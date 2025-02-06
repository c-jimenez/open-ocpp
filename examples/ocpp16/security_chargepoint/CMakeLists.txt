######################################################
#       Security charge point example project        #
######################################################

# Executable target
add_executable(security_chargepoint
    main.cpp
)

# Additionnal libraries path
target_link_directories(security_chargepoint PRIVATE ${BIN_DIR})

# Dependencies
target_link_libraries(security_chargepoint
    examples_common16
)


# Copy to binary directory
ADD_CUSTOM_COMMAND(TARGET security_chargepoint
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/config/security_chargepoint.ini ${BIN_DIR}/
)
