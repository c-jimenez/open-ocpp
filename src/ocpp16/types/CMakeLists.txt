######################################################
#             OCPP 1.6 types library                 #
######################################################


# This is an header only library
add_library(types16 INTERFACE)
target_include_directories(types16 INTERFACE .)
target_link_libraries(types16 INTERFACE types)