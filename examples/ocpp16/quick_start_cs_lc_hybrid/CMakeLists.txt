######################################################
#    Quick start local controller example project    #
######################################################

# Executable target
add_executable(quick_start_cs_lc_hybrid
    main.cpp
    HybridCentralSystemEventsHandler.cpp
)

# Additionnal libraries path
target_link_directories(quick_start_cs_lc_hybrid PRIVATE ${BIN_DIR})

# Dependencies
target_link_libraries(quick_start_cs_lc_hybrid
    examples_common16
    localcontroller
)


# Copy to binary directory
ADD_CUSTOM_COMMAND(TARGET quick_start_cs_lc_hybrid
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/config/quick_start_cs_lc_hybrid.ini ${BIN_DIR}/
)
