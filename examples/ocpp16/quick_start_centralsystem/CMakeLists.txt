######################################################
#     Quick start central system example project     #
######################################################

# Executable target
add_executable(quick_start_centralsystem
    main.cpp
)

# Additionnal libraries path
target_link_directories(quick_start_centralsystem PRIVATE ${BIN_DIR})

# Dependencies
target_link_libraries(quick_start_centralsystem
    examples_common16
    centralsystem
)


# Copy to binary directory
ADD_CUSTOM_COMMAND(TARGET quick_start_centralsystem
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/config/quick_start_centralsystem.ini ${BIN_DIR}/
)
