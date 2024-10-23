######################################################
#     Quick start charge point example project       #
######################################################

# Executable target
add_executable(quick_start_chargepoint
    main.cpp
)

# Additionnal libraries path
target_link_directories(quick_start_chargepoint PRIVATE ${BIN_DIR})

# Dependencies
target_link_libraries(quick_start_chargepoint
    examples_common16
)


# Copy to binary directory
ADD_CUSTOM_COMMAND(TARGET quick_start_chargepoint
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/config/quick_start_chargepoint.ini ${BIN_DIR}/
)
