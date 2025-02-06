######################################################
#    Quick start local controller example project    #
######################################################

# Executable target
add_executable(quick_start_localcontroller
    main.cpp
)

# Additionnal libraries path
target_link_directories(quick_start_localcontroller PRIVATE ${BIN_DIR})

# Dependencies
target_link_libraries(quick_start_localcontroller
    examples_common16
    localcontroller
)


# Copy to binary directory
ADD_CUSTOM_COMMAND(TARGET quick_start_localcontroller
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/config/quick_start_localcontroller.ini ${BIN_DIR}/
)
