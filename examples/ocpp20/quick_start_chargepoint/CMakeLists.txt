######################################################
#     Quick start charge point example project       #
######################################################

# Executable target
add_executable(quick_start_chargepoint20
    main.cpp
)

# Additionnal libraries path
target_link_directories(quick_start_chargepoint20 PRIVATE ${BIN_DIR})

# Dependencies
target_link_libraries(quick_start_chargepoint20
    examples_common20
)


# Copy to binary directory
ADD_CUSTOM_COMMAND(TARGET quick_start_chargepoint20
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/config/quick_start_chargepoint20.ini ${BIN_DIR}/
)
ADD_CUSTOM_COMMAND(TARGET quick_start_chargepoint20
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/config/quick_start_chargepoint20.json ${BIN_DIR}/
)
