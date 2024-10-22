######################################################
#     ISO15118 charge point example project       #
######################################################

# Executable target
add_executable(iso15118_chargepoint
    main.cpp
)

# Additionnal libraries path
target_link_directories(iso15118_chargepoint PRIVATE ${BIN_DIR})

# Dependencies
target_link_libraries(iso15118_chargepoint
    examples_common16
)


# Copy to binary directory
ADD_CUSTOM_COMMAND(TARGET iso15118_chargepoint
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/config/iso15118_chargepoint.ini ${BIN_DIR}/
)
