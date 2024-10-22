######################################################
#      ISO15118 central system example project       #
######################################################

# Executable target
add_executable(iso15118_centralsystem
    main.cpp
)

# Additionnal libraries path
target_link_directories(iso15118_centralsystem PRIVATE ${BIN_DIR})

# Dependencies
target_link_libraries(iso15118_centralsystem
    examples_common16
    centralsystem
)


# Copy to binary directory
ADD_CUSTOM_COMMAND(TARGET iso15118_centralsystem
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/config/iso15118_centralsystem.ini ${BIN_DIR}/
)
